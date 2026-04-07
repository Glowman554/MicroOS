#include <json.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

json_value_t* json_new_value(int type) {
    json_value_t* value = malloc(sizeof(json_value_t));
    value->type = type;
    value->start = NULL;
    value->end = NULL;
    value->depth = 0;
    return value;
}


void json_destroy_value(json_value_t* value) {
    free(value);
}

void json_print_value(json_value_t* value) {
    int size = (int)(value->end - value->start);

    char* output = malloc(size + 1);
    memcpy(output, value->start, size);
    output[size] = '\0';
    printf("%s", output);
    free(output);
}