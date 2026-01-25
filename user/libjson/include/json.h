#pragma once
#include <stdbool.h>


typedef struct json_reader {
    char* data;
    char* current;
    char* end;
    int depth;
    char* error;
} json_reader_t;

json_reader_t* json_new_reader(char* data, int length);
void json_destroy_reader(json_reader_t* reader);

enum {
    VALUE_ERROR,
    VALUE_END,
    VALUE_ARRAY,
    VALUE_OBJECT,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_BOOL,
    VALUE_NULL
};
typedef struct json_value {
    int type;
    char* start;
    char* end;
    int depth;
} json_value_t;

json_value_t* json_new_value(int type);
void json_destroy_value(json_value_t* value);
void json_print_value(json_value_t* value);

bool json_is_number_cont(char c);
bool json_is_string(char* current, char* end, char* expect);
bool json_is_key(json_value_t* value, char* s);
void json_discard_until(json_reader_t* r, int depth);
bool json_iter_array(json_reader_t* reader, json_value_t* array, json_value_t** outValue);
bool json_iter_object(json_reader_t* reader, json_value_t* object, json_value_t** outKey, json_value_t** outValue);
json_value_t* json_read(json_reader_t* r);