#pragma once

typedef struct config_node {
    const char* key;
    const char* value;
} config_node_t;

config_node_t* load_cfg(const char* cfg);

const char* get_cfg_value(config_node_t* cfg, const char* key);