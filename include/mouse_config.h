#ifndef MOUSE_CONFIG_H
#define MOUSE_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MODES_COUNT 5

typedef struct side_btn
{
    unsigned char mode;
    unsigned char mod1;
    unsigned char mod2;
    unsigned char mod3;

    int has_mode;
    int has_mod1;
    int has_mod2;
    int has_mod3;
} side_btn;

typedef struct
{
    // led modifiers
    unsigned char mod1;
    unsigned char mod2;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char brightness;
    unsigned char speed;
    
    // dpi levels
    unsigned int dpi[MODES_COUNT];

    // side button modifiers
    side_btn buttons[8];
    
    // flags to know what's explicitly set
    int has_mod1;
    int has_mod2;
    int has_r;
    int has_g;
    int has_b;
    int has_brightness;
    int has_speed;
    int has_dpi[MODES_COUNT];
    int has_side_btn[8];

} mouse_config;

int load_config(const char *path, mouse_config *modes_out);
int save_config(const char *path, const mouse_config *modes);
const mouse_config *get_mode_config(int idx);
void set_mode_config(int idx, mouse_config *cfg);
mouse_config *all_mode_configs(void);

#endif