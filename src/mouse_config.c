// Simple INI load/save for 5 mouse modes (MODE1..MODE5)
// Format (Option A - standard INI):
// [MODE1]
// MOD1=0x1
// MOD2=0x4
// R=0xff
// G=0xff
// B=0xff
// BR=0x3
// SP=0x4


#include "mouse_config.h"

static mouse_config modes[MODES_COUNT];

#define LINE_BUF 256

// trimming white spaces for the s
char *trim(char *s) 
{
    if(!s) return s;
    
    char *end = s + (strlen(s) - 1); // end is pointer to trim the end

    while(isspace((unsigned char)*s)) s++; //to trim the left(beginning)
    while(s > end && isspace((unsigned char)*end)) end--;
    end[1] = '\0'; // so when previous while loops end the next char is nullt

    return s;
}

// parse uint support hex and decimals
// returns 1 on success, out is assigned; 0 on fail
static int parse_uint(const char *token, unsigned int *out) 
{
    if(!token || !*token) return 0;
    const char *p = token;
    while(isspace((unsigned char)*p)) p++;

    if(p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        //hex
        char *end;
        unsigned long v = strtoul(p, &end, 16);
        if(end == p + 2) return 0; //when there's actually no num after 0x
        *out = (unsigned int)v;
        return 1;
    } else {
        // decimals and hex without 0x prefix
        char *end;
        unsigned long v = strtoul(p, &end, 10);
        if(end == p) return 0; 
        *out = (unsigned int)v;
        return 1;
    }
}

// parse single key=value line to mouse_config
static void apply_kv_to_mode(mouse_config *m, const char *key, const char *value) 
{
    unsigned int tmp;
    
    if(strncmp(key, "MOD1", 4) == 0) {
        if(parse_uint(value, &tmp)) {
            m->mod1 = (unsigned char)tmp;
            m->has_mod1 = 1;
        }
    } else if(strncmp(key, "MOD2", 4) == 0) {
        if(parse_uint(value, &tmp)) {
            m->mod2 = (unsigned char)tmp;
            m->has_mod2 = 1;
        } 
    } else if(strncmp(key, "R", 1) == 0 || strncmp(key, "RED", 3) == 0) {
        if(parse_uint(value, &tmp)) {
            m->r = (unsigned char)tmp;
            m->has_r = 1;
        }
    } else if(strncmp(key, "G", 1) == 0 || strncmp(key, "GREEN", 5) == 0) {
        if(parse_uint(value, &tmp)) {
            m->g = (unsigned char)tmp;
            m->has_g = 1;
        }
    } else if(strncmp(key, "B", 1) == 0 || strncmp(key, "BLUE", 4) == 0) {
        if(parse_uint(value, &tmp)) {
            m->b = (unsigned char)tmp;
            m->has_b = 1;
        }
    } else if(strncmp(key, "BR", 2) == 0 || strncmp(key, "BRIGHTNESS", 10) == 0) {
        if(parse_uint(value, &tmp)) 
        {
            m->brightness = (unsigned char)tmp;
            m->has_brightness = 1;
        }
    } else if(strncmp(key, "SP", 2) == 0 || strncmp(key, "SPEED", 5) == 0) 
    {
        if(parse_uint(value, &tmp)) {
            m->speed = (unsigned char)tmp;
            m->has_speed = 1;
        }
    } else if(strncmp(key, "DPI1", 4) == 0)
    {
        if(parse_uint(value, &tmp))
        {
            m->dpi[0] = tmp;
            m->has_dpi[0] = 1;
        }
    } else if(strncmp(key, "DPI2", 4) == 0)
    {
        if(parse_uint(value, &tmp))
        {
            m->dpi[1] = tmp;
            m->has_dpi[1] = 1;
        }
    } else if(strncmp(key, "DPI3", 4) == 0)
    {
        if(parse_uint(value, &tmp))
        {
            m->dpi[2] = tmp;
            m->has_dpi[2] = 1;
        }
    } else if(strncmp(key, "DPI4", 4) == 0)
    {
        if(parse_uint(value, &tmp))
        {
            m->dpi[3] = tmp;
            m->has_dpi[3] = 1;
        }
    } else if(strncmp(key, "DPI5", 4) == 0)
    {
        if(parse_uint(value, &tmp))
        {
            m->dpi[4] = tmp;
            m->has_dpi[4] = 1;
        }
    } else if(strncmp(key, "SIDEB", 5) == 0) 
    {
        // ex: SIDEB1_M0
        int side_button_idx = (atoi(&key[5])) - 1;
        int button_m = atoi(&key[8]);

        if(button_m < 0 || button_m > 3)
        {
            printf("Failed to parse side button index: %d", side_button_idx);
        }

        if(parse_uint(value, &tmp))
        {
            switch (button_m)
            {
            case 0:
                m->buttons[side_button_idx].mode = (unsigned char)tmp;
                m->buttons[side_button_idx].has_mode = 1;
                break;
            case 1:
                m->buttons[side_button_idx].mod1 = (unsigned char)tmp;
                m->buttons[side_button_idx].has_mod1 = 1;
                break;
            case 2:
                m->buttons[side_button_idx].mod2 = (unsigned char)tmp;
                m->buttons[side_button_idx].has_mod2 = 1;
                break;
            case 3:
                m->buttons[side_button_idx].mod3 = (unsigned char)tmp;
                m->buttons[side_button_idx].has_mod3 = 1;
                break;
            }
            
        }
    } else 
    {
        printf("Unknown key %s\n", key);
    }
}

// init struct mouse_config to defaults
static void init_default(mouse_config *modes) 
{
    for (int i = 0; i < MODES_COUNT; i++)
    {
        // default values to rainbow
        modes[i].mod1 = 0x1;
        modes[i].mod2 = 0x8;
        modes[i].r = 0xff;
        modes[i].g = 0x0;
        modes[i].b = 0x0;
        modes[i].brightness = 0x2;
        modes[i].speed = 0x4;
        // should i init dpis?
        modes[i].buttons->mode = 0x0;
        modes[i].buttons->mod1 = 0x0;
        modes[i].buttons->mod2 = 0x0;
        modes[i].buttons->mod3 = 0x0;

        modes[i].has_mod1 = modes[i].has_mod2 = 0;
        modes[i].has_r = modes[i].has_g = modes[i].has_b = 0;
        modes[i].has_brightness = modes[i].has_speed = 0;
    }
    
}

// load config from path
// return 1 for sucess 0 on error
int load_config(const char *path, mouse_config *modes_out) 
{
    init_default(modes_out);

    FILE *f = fopen(path, "r");

    if(!f) {
        // file missing kept default return 0
        return 0;
    }

    char line[LINE_BUF];
    int current_mode = -1;
    while(fgets(line, sizeof line, f)) {
        char *s = trim(line);
        if(s[0] == '\0') continue; //its empty
        if(s[0] == ';' || s[0] == '#') continue; //comments

        if(s[0] == '[') {
            // sections like [MODE1]

            char *end = strchr(s, ']');
            if(!end) continue; //malformed
            *end = '\0';

            char *sec = s + 1;
            char name[64];
            strncpy(name, sec, sizeof name - 1);
            name[sizeof name - 1] = '\0'; 
            
            char *t = trim(name);

            if(strncmp(t, "MODE", 4) == 0) {
                char *num = t + 4;
                while(num && (isspace(*num) || *num == '_' || *num == '-')) num++;
                int n = atoi(num);

                if(n >= 1 || n <= MODES_COUNT) {
                    current_mode = n - 1;
                } else {
                    current_mode = -1;
                }
            } else {
                current_mode = -1;
            }

            continue;
        }

        // key = value
        char *eq = strchr(s, '=');
        if(!eq) continue; //ignore malformed data
        *eq = '\0';
        char *k = trim(s);
        char *v = trim(eq + 1);
        
        if(current_mode >= 0 && current_mode <= MODES_COUNT) {
            apply_kv_to_mode(&modes_out[current_mode], k, v);
        } else {
            // not inside a MODE section: ignore for this simple parser
        }
    }
    fclose(f);
    return 1;
}

// writing (overwrite) config
// return 1 on sucess 0 on failure
int save_config(const char *path, const mouse_config *modes) 
{
    FILE *f = fopen(path, "w");

    if(!f) return 0;

    for(int i = 0; i < MODES_COUNT; i++) {
        fprintf(f, "[MODE%d]\n", i + 1);

        //writing values in hex
        fprintf(f, "MOD1=0x%X\n", modes[i].mod1);
        fprintf(f, "MOD2=0x%X\n", modes[i].mod2);
        fprintf(f, "R=0x%X\n", modes[i].r);
        fprintf(f, "G=0x%X\n", modes[i].g);
        fprintf(f, "B=0x%X\n", modes[i].b);
        fprintf(f, "BR=0x%X\n", modes[i].brightness);
        fprintf(f, "SP=0x%X\n", modes[i].speed);
        fprintf(f, "DPI1=%d\n", modes[i].dpi[0]);
        fprintf(f, "DPI2=%d\n", modes[i].dpi[1]);
        fprintf(f, "DPI3=%d\n", modes[i].dpi[2]);
        fprintf(f, "DPI4=%d\n", modes[i].dpi[3]);
        fprintf(f, "DPI5=%d\n", modes[i].dpi[4]);

        for (int j = 0; j < 8; j++)
        {
            fprintf(f, "SIDEB%d_M0=0x%X\n", j + 1, modes[i].buttons[j].mode);
            fprintf(f, "SIDEB%d_M1=0x%X\n", j + 1, modes[i].buttons[j].mod1);
            fprintf(f, "SIDEB%d_M2=0x%X\n", j + 1, modes[i].buttons[j].mod2);
            fprintf(f, "SIDEB%d_M3=0x%X\n", j + 1, modes[i].buttons[j].mod3);
            printf("modes[%d].buttons[%d].mode : %x\n", i, j, modes[i].buttons[j].mode);
        }

        fprintf(f, "\n");
    }

    fclose(f);

    return 1;
}

// this is for getting mouse_config modes on other file (read only)
// still questionable whether i will actually need it as modes array are already declared globally in header
// don't forget to -1 index
const mouse_config *get_mode_config(int idx) {
    if(idx < 0 || idx > 5) return NULL;
    return &modes[idx];
}

// writing to mouse_config modes array
// need to -1 index
void set_mode_config(int idx, mouse_config *cfg) {
    if(idx < 0 || idx > 5) return;
    modes[idx] = *cfg;
}

// return all modes array
// proceed with caution
mouse_config *all_mode_configs(void) {
    return modes;
}