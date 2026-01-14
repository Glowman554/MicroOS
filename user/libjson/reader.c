#include <json.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// function jsonIsNumberCont(chr c) -> int {
//     return ((c >= '0') & (c <= '9')) |  (c == 'e') | (c == 'E') | (c == '.') | (c == '-') | (c == '+');
// }

json_value_t* json_read(json_reader_t* r);

json_reader_t* json_new_reader(char* data, int length) {
    json_reader_t* reader = malloc(sizeof(json_reader_t));
    reader->data = data;
    reader->current = data;
    reader->end = data + length;
    reader->depth = 0;
    reader->error = NULL;
    return reader;
}

void json_destroy_reader(json_reader_t* reader) {
    free(reader);
}


bool json_is_number_cont(char c) {
    return ((c >= '0') & (c <= '9')) ||  (c == 'e') || (c == 'E') || (c == '.') || (c == '-') || (c == '+');
}

// function jsonIsString(chr[] current, chr[] end, chr[] expect) -> int {
//     while expect[0] {
//         if (current == end) | (current[0] != expect[0]) {
//             return 0;
//         }
//         expect++;
//         current++;
//     }

//     return 1;
// }

bool json_is_string(char* current, char* end, char* expect) {
    while (expect[0]) {
        if ((current == end) || (current[0] != expect[0])) {
            return false;
        }
        expect++;
        current++;
    }

    return true;
}

// function jsonIsKey(ptr value, chr[] s) -> int {
//     ptr start = jsonValueGetStart(value);
//     ptr end = jsonValueGetEnd(value);
//     int len = end - start;

//     int sLen = string_length(s);
//     if sLen != len {
//         return 0;
//     }

//     return memory_compare_8(start, s, len);
// }

bool json_is_key(json_value_t* value, char* s) {
    int len = (int)(value->end - value->start);

    int sLen = strlen(s);
    if (sLen != len) {
        return false;
    }

    return memcmp(value->start, s, len) == 0;
}

// function jsonDiscardUntil(ptr r, int depth) -> void {
//     while (jsonReaderGetCurrent(r) != jsonReaderGetEnd(r)) & (jsonReaderGetDepth(r) > depth) {
//         ptr value = jsonRead(r);
//         int type = jsonValueGetType(value);
//         jsonDestroyValue(value); 
//         if type == VALUE_ERROR {
//             return;
//         }
//     }
// }

void json_discard_until(json_reader_t* r, int depth) {
    while ((r->current != r->end) && (r->depth > depth)) {
        json_value_t* value = json_read(r);
        int type = value->type;
        json_destroy_value(value); 
        if (type == VALUE_ERROR) {
            return;
        }
    }
}


// function jsonIterArray(ptr reader, ptr array, ptr outValue) -> int {
//     jsonDiscardUntil(reader, jsonValueGetDepth(array));

//     ptr value = jsonRead(reader);
//     set_ptr(outValue, value);
//     int type = jsonValueGetType(value);

//     if (type == VALUE_ERROR) | (type == VALUE_END) {
//         return 0;
//     }

//     return 1;    
// }

bool json_iter_array(json_reader_t* reader, json_value_t* array, json_value_t** outValue) {
    json_discard_until(reader, array->depth);

    json_value_t* value = json_read(reader);
    *outValue = value;
    int type = value->type;

    if ((type == VALUE_ERROR) || (type == VALUE_END)) {
        return false;
    }

    return true;    
}

// function jsonIterObject(ptr reader, ptr object, ptr outKey, ptr outValue) -> int {
//     jsonDiscardUntil(reader, jsonValueGetDepth(object));

//     ptr key = jsonRead(reader);
//     set_ptr(outKey, key);
//     int type = jsonValueGetType(key);
//     if (type == VALUE_ERROR) | (type == VALUE_END) {
//         return 0;
//     }    

//     ptr value = jsonRead(reader);
//     set_ptr(outValue, value);
//     type = jsonValueGetType(value);
    
//     if type == VALUE_ERROR {
//         jsonReaderSetError(reader, "unexpected object end");
//         return 0;
//     } 

//     if type == VALUE_END {
//         return 0;
//     }

//     return 1;
// }

bool json_iter_object(json_reader_t* reader, json_value_t* object, json_value_t** outKey, json_value_t** outValue) {
    json_discard_until(reader, object->depth);

    json_value_t* key = json_read(reader);
    *outKey = key;
    int type = key->type;
    if ((type == VALUE_ERROR) || (type == VALUE_END)) {
        return false;
    }    

    json_value_t* value = json_read(reader);
    *outValue = value;
    type = value->type;
    
    if (type == VALUE_ERROR) {
        reader->error = "unexpected object end";
        return false;
    } 

    if (type == VALUE_END) {
        return false;
    }

    return true;
}

// function jsonRead(ptr r) -> ptr {
//     loop {
//         if jsonReaderGetError(r) != 0 {
//             ptr current = jsonReaderGetCurrent(r);
//             ptr val = jsonNewValue(VALUE_ERROR);
//             jsonValueSetStart(val, current);
//             jsonValueSetEnd(val, current);
//             return val;
//         }

//         if jsonReaderGetCurrent(r) == jsonReaderGetEnd(r) {
//             jsonReaderSetError(r, "unexpected eof");
//             continue;
//         }

        

//         chr c = get_chr(jsonReaderGetCurrent(r));

//         // Skip whitespace and separators
//         if (c == ' ') | (c == '\n') | (c == '\r') | (c == '\t') | (c == ':') | (c == ',') {
//             jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//             continue;
//         }

//         ptr res = jsonNewValue(VALUE_ERROR);
//         jsonValueSetStart(res, jsonReaderGetCurrent(r));

//         // Numbers
//         if ((c >= '0') & (c <= '9')) | (c == '-') {
//             jsonValueSetType(res, VALUE_NUMBER);
//             while (jsonReaderGetCurrent(r) != jsonReaderGetEnd(r)) & (jsonIsNumberCont(get_chr(jsonReaderGetCurrent(r)))) {
//                 jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//             }
//             jsonValueSetEnd(res, jsonReaderGetCurrent(r));
//             return res;
//         }

//         // Strings
//         if c == '"' {
//             jsonValueSetType(res, VALUE_STRING);
//             jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//             jsonValueSetStart(res, jsonReaderGetCurrent(r));

//             loop {
//                 if jsonReaderGetCurrent(r) == jsonReaderGetEnd(r) {
//                     jsonReaderSetError(r, "unclosed string");
//                     jsonDestroyValue(res);
//                     break;
//                 }
//                 chr ch = get_chr(jsonReaderGetCurrent(r));
//                 if ch == '"' {
//                     jsonValueSetEnd(res, jsonReaderGetCurrent(r));
//                     jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//                     return res;
//                 }
//                 if ch == '\\' {
//                     jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//                 }
//                 jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//             }
//             continue;
//         }

//         // // Objects / arrays
//         if (c == '{') | (c == '[') {
//             if c == '{' {
//                 jsonValueSetType(res, VALUE_OBJECT);
//             } else {
//                 jsonValueSetType(res, VALUE_ARRAY);
//             }
//             jsonValueSetDepth(res, jsonReaderGetDepth(r) + 1);
//             jsonValueSetStart(res, jsonReaderGetCurrent(r));
//             jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//             jsonValueSetEnd(res, jsonReaderGetCurrent(r));            
//             jsonReaderSetDepth(r, jsonValueGetDepth(res));
//             return res;
//         }

//         // End of objects / arrays
//         if (c == '}') | (c == ']') {
//             jsonValueSetType(res, VALUE_END);
//             jsonValueSetStart(res, jsonReaderGetCurrent(r));
//             jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 1);
//             jsonValueSetEnd(res, jsonReaderGetCurrent(r));            
//             jsonReaderSetDepth(r, jsonReaderGetDepth(r) - 1);
//             return res;
//         }

//         // Literals: null, true, false
//         if (c == 'n') | (c == 't') | (c == 'f') {
//             if jsonIsString(jsonReaderGetCurrent(r), jsonReaderGetEnd(r), "null") {
//                 jsonValueSetType(res, VALUE_NULL);
//                 jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 4);
//                 jsonValueSetEnd(res, jsonReaderGetCurrent(r));
//                 return res;
//             }
//             if jsonIsString(jsonReaderGetCurrent(r), jsonReaderGetEnd(r), "true") {
//                 jsonValueSetType(res, VALUE_BOOL);
//                 jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 4);
//                 set_ptr(offset(res, value_end), jsonReaderGetCurrent(r));
//                 return res;
//             }
//             if jsonIsString(jsonReaderGetCurrent(r), jsonReaderGetEnd(r), "false") {
//                 jsonValueSetType(res, VALUE_BOOL);
//                 jsonReaderSetCurrent(r, jsonReaderGetCurrent(r) + 5);
//                 set_ptr(offset(res, value_end), jsonReaderGetCurrent(r));
//                 return res;
//             }
//             jsonReaderSetError(r, "unknown token");
//             jsonDestroyValue(res);
//             continue;
//         }

//         jsonReaderSetError(r, "unknown token");
//         jsonDestroyValue(res);
//     }
// }

json_value_t* json_read(json_reader_t* r) {
    while (1) {
        // Check for error
        if (r->error != NULL) {
            json_value_t* val = json_new_value(VALUE_ERROR);
            val->start = r->current;
            val->end = r->current;
            return val;
        }

        // Check for end of input
        if (r->current == r->end) {
            r->error = "unexpected eof";
            continue;
        }

        char c = *r->current;

        // Skip whitespace and separators
        if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t') || (c == ':') || (c == ',')) {
            r->current++;
            continue;
        }

        json_value_t* res = json_new_value(VALUE_ERROR);
        res->start = r->current;

        // Numbers
        if (((c >= '0') && (c <= '9')) || (c == '-')) {
            res->type = VALUE_NUMBER;
            while ((r->current != r->end) && json_is_number_cont(*r->current)) {
                r->current++;
            }
            res->end = r->current;
            return res;
        }

        // Strings
        if (c == '"') {
            res->type = VALUE_STRING;
            r->current++;
            res->start = r->current;

            while (1) {
                if (r->current == r->end) {
                    r->error = "unclosed string";
                    json_destroy_value(res);
                    break;
                }
                char ch = *r->current;
                if (ch == '"') {
                    res->end = r->current;
                    r->current++;
                    return res;
                }
                if (ch == '\\') {
                    r->current++;
                }
                r->current++;
            }
            continue;
        }

        // Objects / arrays
        if ((c == '{') || (c == '[')) {
            if (c == '{') {
                res->type = VALUE_OBJECT;
            } else {
                res->type = VALUE_ARRAY;
            }
            res->depth = r->depth + 1;
            res->start = r->current;
            r->current++;
            res->end = r->current;
            r->depth = res->depth;
            return res;
        }

        // End of objects / arrays
        if ((c == '}') || (c == ']')) {
            res->type = VALUE_END;
            res->start = r->current;
            r->current++;
            res->end = r->current;
            r->depth--;
            return res;
        }

        // Literals: null, true, false
        if ((c == 'n') || (c == 't') || (c == 'f')) {
            if (json_is_string(r->current, r->end, "null")) {
                res->type = VALUE_NULL;
                r->current += 4;
                res->end = r->current;
                return res;
            }
            if (json_is_string(r->current, r->end, "true")) {
                res->type = VALUE_BOOL;
                r->current += 4;
                res->end = r->current;
                return res;
            }
            if (json_is_string(r->current, r->end, "false")) {
                res->type = VALUE_BOOL;
                r->current += 5;
                res->end = r->current;
                return res;
            }
            r->error = "unknown token";
            json_destroy_value(res);
            continue;
        }

        r->error = "unknown token";
        json_destroy_value(res);
    }
}
