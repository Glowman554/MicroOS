#include <config.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// #define CFG_DEBUG

config_node_t load_line(char* line) {
    if (*line == '#') {
    #ifdef CFG_DEBUG
        printf("Skipping comment!\n");
    #endif
        goto end;
    }

	int len = strlen(line);

	for (int i = 0; i < len; i++) {
		if (line[i] == '=') {
            const char* key = strndup(line, i);
            const char* value = strndup(&line[i + 1], len - i);
		#ifdef CFG_DEBUG
			printf("got key: %s, value: %s\n", key, value);
		#endif
			return (config_node_t) {
                .key = key,
                .value = value
            };
		}
	}

end:
    return (config_node_t) {
        0, 0
    };
}

config_node_t* load_cfg(const char* cfg) {
    const char* line = cfg;
    size_t config_size = 0;
    config_node_t* config_array = NULL;

    while (line && *line) {
        char* lineEnd = strchr(line, '\n');
        char* lineCpy = strndup(line,  (uint32_t) lineEnd - (uint32_t) line);
        
        config_node_t node = load_line(lineCpy);
        free(lineCpy);

        if (node.key && node.value) {
            config_size++;
            config_array = (config_node_t*) realloc(config_array, config_size * sizeof(config_node_t));
            config_array[config_size - 1] = node;
        }

        line = lineEnd;
        if (line) {
            line++;
        }
    }
    
    config_size++;
    config_array = (config_node_t*) realloc(config_array, config_size * sizeof(config_node_t));
    config_array[config_size - 1].key = NULL;
    config_array[config_size - 1].value = NULL;

    return config_array;
}

const char* get_cfg_value(config_node_t* cfg, const char* key) {
    for (int i = 0; cfg[i].key; i++) {
        if (strcmp(cfg[i].key, key) == 0) {
            return cfg[i].value;
        }
    }
    return NULL;
}