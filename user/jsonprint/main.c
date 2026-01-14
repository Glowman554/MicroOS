#include <stdio.h>
#include <stdlib.h>
#include <json.h>

void indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

void error(json_reader_t* r) {
    printf("%s", r->error);
    exit(1);
}

void recursive_print(json_reader_t* r, json_value_t* object, int depth) {
    json_value_t* out_value = NULL;
    json_value_t* out_key = NULL;

    int type = object->type;

    if (type == VALUE_ERROR) {
        error(r);
    } else if (type == VALUE_ARRAY) {
        printf("[");
        while (json_iter_array(r, object, &out_value)) {
            printf("\n");
            indent(depth + 1);
            recursive_print(r, out_value, depth + 1);
            json_destroy_value(out_value);
        }

        if (r->error) {
            error(r);
        }

        printf("\n");
        indent(depth);
        printf("]");
    } else if (type == VALUE_OBJECT) {
        printf("{");
        while (json_iter_object(r, object, &out_key, &out_value)) {
            printf("\n");
            indent(depth + 1);
            recursive_print(r, out_key, depth + 1);
            printf(": ");
            recursive_print(r, out_value, depth + 1);

            json_destroy_value(out_key);
            json_destroy_value(out_value);
        }

        if (r->error) {
            error(r);
        }

        printf("\n");
        indent(depth);
        printf("}");
    } else if (type == VALUE_NUMBER) {
        json_print_value(object);
    } else if (type == VALUE_STRING) {
        json_print_value(object);
    } else if (type == VALUE_NULL) {
        json_print_value(object);
    } else if (type == VALUE_BOOL) {
        json_print_value(object);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: printer <.json>\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Could not open file for read!\n");
        return 1;
    }

    fsize(file, size);

    char* buffer = malloc(size);
    fread(buffer, size, 1, file);
    fclose(file);

    json_reader_t* r = json_new_reader(buffer, size);
    json_value_t* object = json_read(r);

    recursive_print(r, object, 0);
    printf("\n");

    json_destroy_value(object);
    json_destroy_reader(r);
    free(buffer);

    return 0;
}