#include <net/http.h>
#include <net/dns.h>
#include <net/socket.h>
#include <stdint.h>
#include <sys/net.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

http_request_t* http_request_create(char* method, char* domain, char* path) {
    http_request_t* request = malloc(sizeof(http_request_t));
    request->method = method;
    request->domain = domain;
    request->path = path;
    request->headers = NULL;
    request->headers_count = 0;
    request->body = NULL;
    request->body_length = 0;

    request->response = NULL;
    request->response_length = 0;
    request->response_code = -1;
    request->response_status_line = NULL;
    request->response_headers = NULL;
    request->response_headers_count = 0;
    request->response_body = NULL;
    request->response_body_length = 0;

    request->port = 80;

    http_request_add_header(request, "User-Agent: MicroOS-libc-http/1.0");
    http_request_add_header(request, "Connection: close");

    return request;
}

void http_request_set_body(http_request_t* request, char* body, int length) {
    request->body = body;
    request->body_length = length;
}

void http_request_add_header(http_request_t* request, char* header) {
    char** new_headers = realloc(request->headers, sizeof(char*) * (request->headers_count + 1));
    request->headers = new_headers;
    request->headers[request->headers_count] = header;
    request->headers_count++;
}

void http_request_free(http_request_t* request) {
    free(request->headers);
    free(request->response_headers);
    free(request->response);
    free(request);
}

int http_parse_status_code(const char* status_line, int status_line_len) {
    // Expected: HTTP/1.1 200 OK

    int i = 0;
    while (i < status_line_len && status_line[i] != ' ' && status_line[i] != '\r' && status_line[i] != '\n') {
        i++;
    }
    while (i < status_line_len && status_line[i] == ' ') {
        i++;
    }

    int code = 0;
    int digits = 0;
    while (i < status_line_len) {
        char c = status_line[i];
        if (c < '0' || c > '9') {
            break;
        }
        code = code * 10 + (c - '0');
        digits++;
        i++;
    }

    return (digits > 0) ? code : -1;
}

void http_parse_response(http_request_t* request) {
    request->response_code = -1;
    request->response_status_line = NULL;

    request->response_headers = NULL;
    request->response_headers_count = 0;

    request->response_body = NULL;
    request->response_body_length = 0;

    if (!request->response || request->response_length <= 0) {
        return;
    }

    char* new_resp = realloc(request->response, request->response_length + 1);
    request->response = new_resp;
    request->response[request->response_length] = '\0';

    char* header_end = NULL;
    for (int i = 0; i + 3 < request->response_length; i++) {
        if (request->response[i] == '\r' && request->response[i + 1] == '\n' &&
            request->response[i + 2] == '\r' && request->response[i + 3] == '\n') {
            header_end = request->response + i;
            break;
        }
    }

    char* headers_start = request->response;
    int headers_len = request->response_length;

    if (header_end) {
        headers_len = (int)(header_end - headers_start);
        request->response_body = header_end + 4;
        request->response_body_length = (int)(request->response + request->response_length - request->response_body);

        header_end[0] = '\0';
        header_end[1] = '\0';
        header_end[2] = '\0';
        header_end[3] = '\0';
    }

    char* p = headers_start;
    char* endp = headers_start + headers_len;

    int got_status_line = 0;

    while (p < endp) {
        while (p < endp && (*p == '\r' || *p == '\n' || *p == '\0')) {
            p++;
        }
        if (p >= endp) {
            break;
        }

        char* line_start = p;
        while (p < endp && *p != '\r' && *p != '\n') {
            p++;
        }

        if (p < endp) {
            if (*p == '\r') {
                *p = '\0';
                p++;
                if (p < endp && *p == '\n') {
                    *p = '\0';
                    p++;
                }
            } else if (*p == '\n') {
                *p = '\0';
                p++;
            }
        }

        if (line_start[0] == '\0') {
            continue;
        }

        if (!got_status_line) {
            got_status_line = 1;
            request->response_status_line = line_start;
            request->response_code = http_parse_status_code(request->response_status_line, (int)strlen(request->response_status_line));
            continue;
        }

        char** new_resp_headers = realloc(request->response_headers, sizeof(char*) * (request->response_headers_count + 1));
        request->response_headers = new_resp_headers;
        request->response_headers[request->response_headers_count] = line_start;
        request->response_headers_count++;
    }
}

void http_buffer_grow(char** buf, int* cap, int need) {
    if (need <= *cap) {
        return;
    }

    int new_cap = (*cap <= 0) ? 512 : *cap;
    while (new_cap < need) {
        new_cap *= 2;
    }

    char* nb = realloc(*buf, new_cap);

    *buf = nb;
    *cap = new_cap;
}

void http_buffer_append(char** buf, int* cap, int* len, const char* s) {
    int sl = strlen(s);
    http_buffer_grow(buf, cap, *len + sl);
    memcpy(*buf + *len, s, sl);
    *len += sl;
}

void http_buffer_append_n(char** buf, int* cap, int* len, const void* data, int n) {
    if (n <= 0) {
        return;
    }
    http_buffer_grow(buf, cap, *len + n);
    memcpy(*buf + *len, data, n);
    *len += n;
}

void http_buffer_append_int(char** buf, int* cap, int* len, int v) {
    char tmp[32];
    int tl = sprintf(tmp, "%d", v);
    http_buffer_append_n(buf, cap, len, tmp, tl);
}

int http_request_perform(int nic, http_request_t* request) {
    ip_u ip = parse_ip(request->domain);
	if (ip.ip == 0) {
		ip = dns_resolve_A(nic, request->domain);
		if (ip.ip == 0) {
			printf("Error: Could not resolve %s\n", request->domain);
			abort();
		}
	}

    char* http_request_buffer = NULL;
    int cap = 0;
    int offset = 0;

    http_buffer_append(&http_request_buffer, &cap, &offset, request->method);
    http_buffer_append(&http_request_buffer, &cap, &offset, " ");
    http_buffer_append(&http_request_buffer, &cap, &offset, request->path);
    http_buffer_append(&http_request_buffer, &cap, &offset, " HTTP/1.1\r\n");
    http_buffer_append(&http_request_buffer, &cap, &offset, "Host: ");
    http_buffer_append(&http_request_buffer, &cap, &offset, request->domain);
    http_buffer_append(&http_request_buffer, &cap, &offset, "\r\n");

    for (int i = 0; i < request->headers_count; i++) {
        http_buffer_append(&http_request_buffer, &cap, &offset, request->headers[i]);
        http_buffer_append(&http_request_buffer, &cap, &offset, "\r\n");
    }

    if (request->body && request->body_length > 0) {
        http_buffer_append(&http_request_buffer, &cap, &offset, "Content-Length: ");
        http_buffer_append_int(&http_request_buffer, &cap, &offset, request->body_length);
        http_buffer_append(&http_request_buffer, &cap, &offset, "\r\n");
    }

    http_buffer_append(&http_request_buffer, &cap, &offset, "\r\n");

    if (request->body && request->body_length > 0) {
        http_buffer_append_n(&http_request_buffer, &cap, &offset, request->body, request->body_length);
    }

    int sock = sync_connect(nic, SOCKET_TCP, ip, request->port);
    send(sock, (uint8_t*)http_request_buffer, offset);
    free(http_request_buffer);

    async_t async = { .state = STATE_INIT };
	uint8_t buf[512];
	int idle_ticks = 0;

    uint8_t* resp_buffer = NULL;
    int resp_offset = 0;

	while (1) {
		int n = recv(sock, &async, buf, (int)sizeof(buf) - 1);
		if (n > 0) {
            resp_buffer = realloc(resp_buffer, resp_offset + n);
            memcpy(resp_buffer + resp_offset, buf, n);
            resp_offset += n;

            async.state = STATE_INIT;
			idle_ticks = 0;
			continue;
		}

		sleep_ms(1);
		if (++idle_ticks > 300) {
			break;
		}
	}
    
    sync_disconnect(sock);

    request->response = (char*)resp_buffer;
    request->response_length = resp_offset;

    http_parse_response(request);

    return request->response_code;
}
