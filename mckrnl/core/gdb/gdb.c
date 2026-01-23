#include <gdb/gdb.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <config.h>

#include <driver/acpi/power.h>

typedef int (*gdb_enc_func)(char *buf, int buf_len, const char *data, int data_len);
typedef int (*gdb_dec_func)(const char *buf, int buf_len, char *data, int data_len);

bool gdb_active = false;

int gdb_recv_ack(gdb_state_t* state) {
    int response = gdb_sys_getc(state);

    switch (response) {
        case '+':
            return 0;
        case '-':
            return 1;
        default:
            debugf("received bad packet response: 0x%x", response);
            return GDB_EOF;
    }
}

int gdb_checksum(const char* buf, int len) {
    unsigned char csum = 0;
    while (len--) {
        csum += *buf++;
    }
    return csum;
}


int gdb_send_packet(gdb_state_t* state, const char* pkt_data, int pkt_len) {
    if (gdb_sys_putchar(state, '$') == GDB_EOF) {
        return GDB_EOF;
    }

#ifdef GDB_DEBUG
    {
        printf("-> ");
        for (int p = 0; p < pkt_len; p++) {
            if (gdb_is_printable_char(pkt_data[p])) {
                printf("%c", pkt_data[p]);
            } else {
                printf("\\x%x", pkt_data[p] & 0xff);
            }
        }
        printf("\n");
    }
#endif

    if (gdb_write(state, pkt_data, pkt_len) == GDB_EOF) {
        return GDB_EOF;
    }

    char buf[3] = { '#', 0, 0 };
    char csum = gdb_checksum(pkt_data, pkt_len);
    if ((gdb_enc_hex(buf + 1, sizeof(buf) - 1, &csum, 1) == GDB_EOF) || (gdb_write(state, buf, sizeof(buf)) == GDB_EOF)) {
        return GDB_EOF;
    }

    return gdb_recv_ack(state);
}

int gdb_recv_packet(gdb_state_t* state, char* pkt_buf, int pkt_buf_len, int* pkt_len) {
    while (1) {
        int data = gdb_sys_getc(state);
        if (data == GDB_EOF) {
            return GDB_EOF;
        } else if (data == '$') {
            break;
        }
    }

    *pkt_len = 0;
    while (1) {
        int data = gdb_sys_getc(state);

        if (data == GDB_EOF) {
            return GDB_EOF;
        } else if (data == '#') {
            break;
        } else {
            if (*pkt_len >= pkt_buf_len) {
                debugf("packet buffer overflow");
                return GDB_EOF;
            }

            pkt_buf[(*pkt_len)++] = (char) data;
        }
    }

#ifdef GDB_DEBUG
    {
        printf("<- ");
        for (int p = 0; p < *pkt_len; p++) {
            if (gdb_is_printable_char(pkt_buf[p])) {
                printf("%c", pkt_buf[p]);
            } else {
                printf("\\x%x", pkt_buf[p] & 0xff);
            }
        }
        printf("\n");
    }
#endif


    char expected_csum;
    char buf[2];
    if ((gdb_read(state, buf, sizeof(buf), 2) == GDB_EOF) || (gdb_dec_hex(buf, 2, &expected_csum, 1) == GDB_EOF)) {
        return GDB_EOF;
    }

    char actual_csum = gdb_checksum(pkt_buf, *pkt_len);
    if (actual_csum != expected_csum) {
        debugf("received packet with bad checksum");
        gdb_sys_putchar(state, '-');
        return GDB_EOF;
    }

    gdb_sys_putchar(state, '+');
    return 0;
}

int gdb_mem_read(gdb_state_t* state, char* buf, int buf_len, uint32_t addr, int len, gdb_enc_func enc) {
    char data[64];
    if (len > sizeof(data)) {
        return GDB_EOF;
    }

    for (int pos = 0; pos < len; pos++) {
        if (gdb_sys_mem_readb(state, addr + pos, &data[pos])) {
            return GDB_EOF;
        }
    }

    return enc(buf, buf_len, data, len);
}

int gdb_mem_write(gdb_state_t* state, const char* buf, int buf_len, uint32_t addr,  int len, gdb_dec_func dec) {
    char data[64];
    if (len > sizeof(data)) {
        return GDB_EOF;
    }

    if (dec(buf, buf_len, data, len) == GDB_EOF) {
        return GDB_EOF;
    }

    for (int pos = 0; pos < len; pos++) {
        if (gdb_sys_mem_writeb(state, addr + pos, data[pos])) {
            return GDB_EOF;
        }
    }

    return 0;
}

int gdb_continue(gdb_state_t* state) {
    gdb_sys_continue(state);
    return 0;
}

int gdb_step(gdb_state_t* state) {
    gdb_sys_step(state);
    return 0;
}

int gdb_send_ok_packet(gdb_state_t* state, char* buf, int buf_len) {
    return gdb_send_packet(state, "OK", 2);
}

int gdb_send_conmsg_packet(gdb_state_t* state, char* buf, int buf_len, const char* msg) {
    if (buf_len < 2) {
        return GDB_EOF;
    }

    buf[0] = 'O';
    int status = gdb_enc_hex(&buf[1], buf_len - 1, msg, strlen(msg));
    if (status == GDB_EOF) {
        return GDB_EOF;
    }
    int size = 1 + status;
    return gdb_send_packet(state, buf, size);
}

int gdb_send_signal_packet(gdb_state_t* state, char* buf, int buf_len, char signal) {
    if (buf_len < 4) {
        return GDB_EOF;
    }

    buf[0] = 'S';
    int status = gdb_enc_hex(&buf[1], buf_len - 1, &signal, 1);
    if (status == GDB_EOF) {
        return GDB_EOF;
    }
    int size = 1 + status;
    return gdb_send_packet(state, buf, size);
}

int gdb_send_error_packet(gdb_state_t* state, char* buf, int buf_len, char error) {
    if (buf_len < 4) {
        return GDB_EOF;
    }

    buf[0] = 'E';
    int status = gdb_enc_hex(&buf[1], buf_len - 1, &error, 1);
    if (status == GDB_EOF) {
        return GDB_EOF;
    }
    int size = 1 + status;
    return gdb_send_packet(state, buf, size);
}

#define token_remaining_buf (pkt_len-(ptr_next-pkt_buf))

#define token_expect_seperator(c) \
    { \
        if (!ptr_next || *ptr_next != c) { \
            goto error; \
        } else { \
            ptr_next += 1; \
        } \
    }

#define token_expect_integer_arg(arg) \
    { \
        arg = gdb_strtol(ptr_next, token_remaining_buf, 16, &ptr_next); \
        if (!ptr_next) { \
            goto error; \
        } \
    }


int gdb_main(gdb_state_t* state) {
    uint32_t addr;
    unsigned int length;

    char pkt_buf[256];
    gdb_send_signal_packet(state, pkt_buf, sizeof(pkt_buf), state->signum);

    while (1) {
        int pkt_len;
        int status = gdb_recv_packet(state, pkt_buf, sizeof(pkt_buf), &pkt_len);
        if (status == GDB_EOF) {
            break;
        }

        if (pkt_len == 0) {
            continue;
        }

        const char* ptr_next = pkt_buf;

        switch (pkt_buf[0]) {
            case 'g':
                status = gdb_enc_hex(pkt_buf, sizeof(pkt_buf), (char*) &state->registers, sizeof(state->registers));
                if (status == GDB_EOF) {
                    goto error;
                }
                pkt_len = status;
                gdb_send_packet(state, pkt_buf, pkt_len);
                break;

            case 'G':
                status = gdb_dec_hex(pkt_buf+1, pkt_len-1, (char*) &state->registers, sizeof(state->registers));
                if (status == GDB_EOF) {
                    goto error;
                }
                gdb_send_ok_packet(state, pkt_buf, sizeof(pkt_buf));
                break;

            case 'p':
                ptr_next += 1;
                token_expect_integer_arg(addr);

                if (addr >= GDB_CPU_NUM_REGISTERS) {
                    goto error;
                }

                status = gdb_enc_hex(pkt_buf, sizeof(pkt_buf), (char*) &state->registers[addr], sizeof(state->registers[addr]));
                if (status == GDB_EOF) {
                    goto error;
                }
                gdb_send_packet(state, pkt_buf, status);
                break;

            case 'P':
                ptr_next += 1;
                token_expect_integer_arg(addr);
                token_expect_seperator('=');

                if (addr < GDB_CPU_NUM_REGISTERS) {
                    status = gdb_dec_hex(ptr_next, token_remaining_buf, (char*) &state->registers[addr], sizeof(state->registers[addr]));
                    if (status == GDB_EOF) {
                        goto error;
                    }
                }
                gdb_send_ok_packet(state, pkt_buf, sizeof(pkt_buf));
                break;

            case 'm':
                ptr_next += 1;
                token_expect_integer_arg(addr);
                token_expect_seperator(',');
                token_expect_integer_arg(length);

                status = gdb_mem_read(state, pkt_buf, sizeof(pkt_buf), addr, length, gdb_enc_hex);
                if (status == GDB_EOF) {
                    goto error;
                }
                gdb_send_packet(state, pkt_buf, status);
                break;

            case 'M':
                ptr_next += 1;
                token_expect_integer_arg(addr);
                token_expect_seperator(',');
                token_expect_integer_arg(length);
                token_expect_seperator(':');

                status = gdb_mem_write(state, ptr_next, token_remaining_buf, addr, length, gdb_dec_hex);
                if (status == GDB_EOF) {
                    goto error;
                }
                gdb_send_ok_packet(state, pkt_buf, sizeof(pkt_buf));
                break;

            case 'X':
                ptr_next += 1;
                token_expect_integer_arg(addr);
                token_expect_seperator(',');
                token_expect_integer_arg(length);
                token_expect_seperator(':');

                status = gdb_mem_write(state, ptr_next, token_remaining_buf, addr, length, gdb_dec_bin);
                if (status == GDB_EOF) {
                    goto error;
                }
                gdb_send_ok_packet(state, pkt_buf, sizeof(pkt_buf));
                break;

            case 'c':
                gdb_continue(state);
                return 0;

            case 's':
                gdb_step(state);
                return 0;

            case '?':
                gdb_send_signal_packet(state, pkt_buf, sizeof(pkt_buf), state->signum);
                break;

            case 'k':
                acpi_power_off();
                break;


            default:
                debugf("unknown command %c", pkt_buf[0]);
                gdb_send_packet(state, NULL, 0);
                break;
        }

        continue;

    error:
        gdb_send_error_packet(state, pkt_buf, sizeof(pkt_buf), 0x00);
    }

    return 0;
}

