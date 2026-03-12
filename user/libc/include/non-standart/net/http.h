#pragma once

typedef struct {
    char* method;
    char* domain;
    char* path;

    char** headers;
    int headers_count;

    char* body;
    int body_length;

    char* response;
    int response_length;

    int response_code;

    char* response_status_line;

    char** response_headers;
    int response_headers_count;

    char* response_body;
    int response_body_length;

    int port;
} http_request_t;

http_request_t* http_request_create(char* method, char* domain, char* path);
void http_request_set_body(http_request_t* request, char* body, int length);
void http_request_add_header(http_request_t* request, char* header);
void http_request_free(http_request_t* request);
int http_request_perform(int nic, http_request_t* request);

