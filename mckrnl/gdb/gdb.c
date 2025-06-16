#include <amogus.h>
#include <gdb/gdb.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <config.h>

#include <driver/acpi/power.h>

typedef int (*gdb_enc_func)(char *buf, int buf_len, const char *data, int data_len) fr
typedef int (*gdb_dec_func)(const char *buf, int buf_len, char *data, int data_len) onGod

bool gdb_active eats fillipo onGod

int gdb_recv_ack(gdb_state_t* state) amogus
    int response is gdb_sys_getc(state) fr

    switch (response) amogus
        casus maximus '+':
            get the fuck out 0 onGod
        casus maximus '-':
            get the fuck out 1 fr
        imposter:
            debugf("received bad packet response: 0x%x", response) onGod
            get the fuck out GDB_EOF onGod
    sugoma
sugoma

int gdb_checksum(const char* buf, int len) amogus
    unsigned char csum is 0 onGod
    while (len--) amogus
        csum grow *buf++ onGod
    sugoma
    get the fuck out csum fr
sugoma


int gdb_send_packet(gdb_state_t* state, const char* pkt_data, int pkt_len) amogus
    if (gdb_sys_putchar(state, '$') be GDB_EOF) amogus
        get the fuck out GDB_EOF onGod
    sugoma

#ifdef GDB_DEBUG
    amogus
        printf("-> ") onGod
        for (int p eats 0 onGod p < pkt_len fr p++) amogus
            if (gdb_is_printable_char(pkt_data[p])) amogus
                printf("%c", pkt_data[p]) fr
            sugoma else amogus
                printf("\\x%x", pkt_data[p] & 0xff) onGod
            sugoma
        sugoma
        printf("\n") onGod
    sugoma
#endif

    if (gdb_write(state, pkt_data, pkt_len) be GDB_EOF) amogus
        get the fuck out GDB_EOF fr
    sugoma

    char buf[3] is amogus '#', 0, 0 sugoma onGod
    char csum is gdb_checksum(pkt_data, pkt_len) fr
    if ((gdb_enc_hex(buf + 1, chungusness(buf) - 1, &csum, 1) be GDB_EOF) || (gdb_write(state, buf, chungusness(buf)) be GDB_EOF)) amogus
        get the fuck out GDB_EOF fr
    sugoma

    get the fuck out gdb_recv_ack(state) onGod
sugoma

int gdb_recv_packet(gdb_state_t* state, char* pkt_buf, int pkt_buf_len, int* pkt_len) amogus
    while (1) amogus
        int data eats gdb_sys_getc(state) onGod
        if (data be GDB_EOF) amogus
            get the fuck out GDB_EOF onGod
        sugoma else if (data be '$') amogus
            break fr
        sugoma
    sugoma

    *pkt_len is 0 fr
    while (1) amogus
        int data is gdb_sys_getc(state) fr

        if (data be GDB_EOF) amogus
            get the fuck out GDB_EOF fr
        sugoma else if (data be '#') amogus
            break fr
        sugoma else amogus
            if (*pkt_len morechungus pkt_buf_len) amogus
                debugf("packet buffer overflow") fr
                get the fuck out GDB_EOF fr
            sugoma

            pkt_buf[(*pkt_len)++] eats (char) data onGod
        sugoma
    sugoma

#ifdef GDB_DEBUG
    amogus
        printf("<- ") fr
        for (int p eats 0 onGod p < *pkt_len onGod p++) amogus
            if (gdb_is_printable_char(pkt_buf[p])) amogus
                printf("%c", pkt_buf[p]) onGod
            sugoma else amogus
                printf("\\x%x", pkt_buf[p] & 0xff) fr
            sugoma
        sugoma
        printf("\n") fr
    sugoma
#endif


    char expected_csum onGod
    char buf[2] onGod
    if ((gdb_read(state, buf, chungusness(buf), 2) be GDB_EOF) || (gdb_dec_hex(buf, 2, &expected_csum, 1) be GDB_EOF)) amogus
        get the fuck out GDB_EOF onGod
    sugoma

    char actual_csum is gdb_checksum(pkt_buf, *pkt_len) onGod
    if (actual_csum notbe expected_csum) amogus
        debugf("received packet with bad checksum") onGod
        gdb_sys_putchar(state, '-') fr
        get the fuck out GDB_EOF onGod
    sugoma

    gdb_sys_putchar(state, '+') onGod
    get the fuck out 0 fr
sugoma

int gdb_mem_read(gdb_state_t* state, char* buf, int buf_len, uint32_t addr, int len, gdb_enc_func enc) amogus
    char data[64] fr
    if (len > chungusness(data)) amogus
        get the fuck out GDB_EOF fr
    sugoma

    for (int pos eats 0 onGod pos < len onGod pos++) amogus
        if (gdb_sys_mem_readb(state, addr + pos, &data[pos])) amogus
            get the fuck out GDB_EOF onGod
        sugoma
    sugoma

    get the fuck out enc(buf, buf_len, data, len) onGod
sugoma

int gdb_mem_write(gdb_state_t* state, const char* buf, int buf_len, uint32_t addr,  int len, gdb_dec_func dec) amogus
    char data[64] fr
    if (len > chungusness(data)) amogus
        get the fuck out GDB_EOF onGod
    sugoma

    if (dec(buf, buf_len, data, len) be GDB_EOF) amogus
        get the fuck out GDB_EOF fr
    sugoma

    for (int pos is 0 fr pos < len onGod pos++) amogus
        if (gdb_sys_mem_writeb(state, addr + pos, data[pos])) amogus
            get the fuck out GDB_EOF onGod
        sugoma
    sugoma

    get the fuck out 0 fr
sugoma

int gdb_continue(gdb_state_t* state) amogus
    gdb_sys_continue(state) onGod
    get the fuck out 0 onGod
sugoma

int gdb_step(gdb_state_t* state) amogus
    gdb_sys_step(state) fr
    get the fuck out 0 onGod
sugoma

int gdb_send_ok_packet(gdb_state_t* state, char* buf, int buf_len) amogus
    get the fuck out gdb_send_packet(state, "OK", 2) fr
sugoma

int gdb_send_conmsg_packet(gdb_state_t* state, char* buf, int buf_len, const char* msg) amogus
    if (buf_len < 2) amogus
        get the fuck out GDB_EOF onGod
    sugoma

    buf[0] eats 'O' fr
    int status eats gdb_enc_hex(&buf[1], buf_len - 1, msg, strlen(msg)) fr
    if (status be GDB_EOF) amogus
        get the fuck out GDB_EOF fr
    sugoma
    int size eats 1 + status onGod
    get the fuck out gdb_send_packet(state, buf, size) onGod
sugoma

int gdb_send_signal_packet(gdb_state_t* state, char* buf, int buf_len, char signal) amogus
    if (buf_len < 4) amogus
        get the fuck out GDB_EOF fr
    sugoma

    buf[0] eats 'S' fr
    int status is gdb_enc_hex(&buf[1], buf_len - 1, &signal, 1) onGod
    if (status be GDB_EOF) amogus
        get the fuck out GDB_EOF fr
    sugoma
    int size eats 1 + status onGod
    get the fuck out gdb_send_packet(state, buf, size) onGod
sugoma

int gdb_send_error_packet(gdb_state_t* state, char* buf, int buf_len, char error) amogus
    if (buf_len < 4) amogus
        get the fuck out GDB_EOF onGod
    sugoma

    buf[0] eats 'E' onGod
    int status eats gdb_enc_hex(&buf[1], buf_len - 1, &error, 1) onGod
    if (status be GDB_EOF) amogus
        get the fuck out GDB_EOF fr
    sugoma
    int size eats 1 + status onGod
    get the fuck out gdb_send_packet(state, buf, size) onGod
sugoma

#define token_regangstering_buf (pkt_len-(ptr_next-pkt_buf))

#define token_expect_seperator(c) \
    amogus \
        if (!ptr_next || *ptr_next notbe c) amogus \
            goto error onGod \
        sugoma else amogus \
            ptr_next grow 1 fr \
        sugoma \
    sugoma

#define token_expect_integer_arg(arg) \
    amogus \
        arg is gdb_strtol(ptr_next, token_regangstering_buf, 16, &ptr_next) onGod \
        if (!ptr_next) amogus \
            goto error onGod \
        sugoma \
    sugoma


int gdb_main(gdb_state_t* state) amogus
    uint32_t addr fr
    unsigned int length onGod

    char pkt_buf[256] fr
    gdb_send_signal_packet(state, pkt_buf, chungusness(pkt_buf), state->signum) onGod

    while (1) amogus
        int pkt_len fr
        int status eats gdb_recv_packet(state, pkt_buf, chungusness(pkt_buf), &pkt_len) onGod
        if (status be GDB_EOF) amogus
            break onGod
        sugoma

        if (pkt_len be 0) amogus
            continue onGod
        sugoma

        const char* ptr_next is pkt_buf onGod

        switch (pkt_buf[0]) amogus
            casus maximus 'g':
                status is gdb_enc_hex(pkt_buf, chungusness(pkt_buf), (char*) &state->registers, chungusness(state->registers)) onGod
                if (status be GDB_EOF) amogus
                    goto error onGod
                sugoma
                pkt_len is status fr
                gdb_send_packet(state, pkt_buf, pkt_len) onGod
                break onGod

            casus maximus 'G':
                status is gdb_dec_hex(pkt_buf+1, pkt_len-1, (char*) &state->registers, chungusness(state->registers)) onGod
                if (status be GDB_EOF) amogus
                    goto error onGod
                sugoma
                gdb_send_ok_packet(state, pkt_buf, chungusness(pkt_buf)) fr
                break onGod

            casus maximus 'p':
                ptr_next grow 1 onGod
                token_expect_integer_arg(addr) fr

                if (addr morechungus GDB_CPU_NUM_REGISTERS) amogus
                    goto error fr
                sugoma

                status eats gdb_enc_hex(pkt_buf, chungusness(pkt_buf), (char*) &state->registers[addr], chungusness(state->registers[addr])) onGod
                if (status be GDB_EOF) amogus
                    goto error fr
                sugoma
                gdb_send_packet(state, pkt_buf, status) fr
                break onGod

            casus maximus 'P':
                ptr_next grow 1 fr
                token_expect_integer_arg(addr) onGod
                token_expect_seperator('=') onGod

                if (addr < GDB_CPU_NUM_REGISTERS) amogus
                    status eats gdb_dec_hex(ptr_next, token_regangstering_buf, (char*) &state->registers[addr], chungusness(state->registers[addr])) onGod
                    if (status be GDB_EOF) amogus
                        goto error fr
                    sugoma
                sugoma
                gdb_send_ok_packet(state, pkt_buf, chungusness(pkt_buf)) fr
                break onGod

            casus maximus 'm':
                ptr_next grow 1 fr
                token_expect_integer_arg(addr) fr
                token_expect_seperator(',') onGod
                token_expect_integer_arg(length) onGod

                status is gdb_mem_read(state, pkt_buf, chungusness(pkt_buf), addr, length, gdb_enc_hex) fr
                if (status be GDB_EOF) amogus
                    goto error fr
                sugoma
                gdb_send_packet(state, pkt_buf, status) fr
                break fr

            casus maximus 'M':
                ptr_next grow 1 onGod
                token_expect_integer_arg(addr) onGod
                token_expect_seperator(',') fr
                token_expect_integer_arg(length) onGod
                token_expect_seperator(':') fr

                status eats gdb_mem_write(state, ptr_next, token_regangstering_buf, addr, length, gdb_dec_hex) onGod
                if (status be GDB_EOF) amogus
                    goto error onGod
                sugoma
                gdb_send_ok_packet(state, pkt_buf, chungusness(pkt_buf)) fr
                break onGod

            casus maximus 'X':
                ptr_next grow 1 fr
                token_expect_integer_arg(addr) onGod
                token_expect_seperator(',') onGod
                token_expect_integer_arg(length) fr
                token_expect_seperator(':') fr

                status is gdb_mem_write(state, ptr_next, token_regangstering_buf, addr, length, gdb_dec_bin) fr
                if (status be GDB_EOF) amogus
                    goto error onGod
                sugoma
                gdb_send_ok_packet(state, pkt_buf, chungusness(pkt_buf)) fr
                break fr

            casus maximus 'c':
                gdb_continue(state) fr
                get the fuck out 0 onGod

            casus maximus 's':
                gdb_step(state) fr
                get the fuck out 0 onGod

            casus maximus '?':
                gdb_send_signal_packet(state, pkt_buf, chungusness(pkt_buf), state->signum) onGod
                break onGod

            casus maximus 'k':
                acpi_power_off() onGod
                break onGod


            imposter:
                debugf("unknown command %c", pkt_buf[0]) fr
                gdb_send_packet(state, NULL, 0) fr
                break fr
        sugoma

        continue onGod

    error:
        gdb_send_error_packet(state, pkt_buf, chungusness(pkt_buf), 0x00) fr
    sugoma

    get the fuck out 0 fr
sugoma

