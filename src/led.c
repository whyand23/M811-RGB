#include "led.h"
#include "main.h"

unsigned char led_packets[8][16] = {
    {0x2, 0xf3, 0x46, 0x4, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0x2, 0xf3, 0x49, 0x4, 0x7, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x1, 0x4, 0x8, 0x2, 0x0},
    {0x2, 0xf3, 0x51, 0x4, 0x7, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x2, 0x4, 0x0, 0x2, 0x0},
    {0x2, 0xf3, 0x59, 0x4, 0x7, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x2, 0x4, 0x0, 0x2, 0x0},
    {0x2, 0xf3, 0x61, 0x4, 0x7, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x2, 0x4, 0x0, 0x2, 0x0},
    {0x2, 0xf3, 0x69, 0x4, 0x7, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x2, 0x4, 0x0, 0x2, 0x0},
    {0x2, 0xf1, 0x2, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0x2, 0xf3, 0x2c, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
};

// TODO: Make help
command_t led_sub[8] =  {
    {"breathing", set_light_breathing, "Set LED to Breathing", NULL, 0},
    {"lighted", set_light_lighted, "Set LED to Full Lighted", NULL, 0},
    {"rainbow", set_light_rainbow, "Set LED to Rainbow", NULL, 0},
    {"wave", set_light_wave, "Set LED to Wave", NULL, 0},
    {"trace", set_light_trace, "Set LED to Go Without Trace", NULL, 0},
    {"reactive", set_light_reactive, "Set LED to Reactive", NULL, 0},
    {"flash", set_light_flash, "Set LED to Flash", NULL, 0},
    {"off", set_light_off, "Set LED to Off", NULL, 0}
};

// parsing input string to hex
static void parse_rgb(mouse_config *opts, int argc, char **argv) 
{
    printf("Changing RGB\n");
    //TODO: valid hex checker
    if (argc == 0) {
        printf("need RGB values e.g 0xFF0000\n");
        return;
    }

    long int rgb = strtol(argv[0], NULL, 16);
    printf("Your color is: %ld\n", rgb);

    opts->r = (rgb >> 16) & 0xFF;
    printf("r:%d\n", opts->r);
    opts->g = (rgb >> 8) & 0xFF;
    printf("g:%d\n", opts->g);
    opts->b = rgb & 0xFF;
    printf("b:%d\n", opts->b);

    return;
           
}

static void parse_brightness(mouse_config *opts, int argc, char **argv) {
    printf("Changing LED Brightness\n");
    if (argc == 0) {
        printf("brightness needs value\n");
        return;
    }
    unsigned int brightness = strtol(argv[0], NULL, 0x0);
    
    if(brightness > 3 || brightness <= 0) {
        printf("Error Invalid Brightness");
        return;
    }
    
    opts->brightness = brightness;
    return;
}

static void parse_speed(mouse_config *opts, int argc, char **argv) 
{
    printf("Changing LED Speed\n");
    printf("Speed input: %s\n", argv[0]);
    if (argc == 0) {
        printf("Speed needs value\n");
        return;
    }
    unsigned int speed = strtol(argv[0], NULL, 0x0);

    if(speed > 8 || speed <= 0) {
        printf("Error Invalid Speed");
        return;
    }
    
    opts->speed = speed;
    return;
}

// assigning changes to led packets array
//that will later be transferred by libusb
// need to -1 mode
void led_packets_build(mouse_config *opts) 
{
    for(int i = 0; i < MODES_COUNT; i++) {
        led_packets[i + 1][8] = opts[i].r;
        led_packets[i + 1][9] = opts[i].g;
        led_packets[i + 1][10] = opts[i].b;
        led_packets[i + 1][11] = opts[i].mod1;
        led_packets[i + 1][12] = opts[i].speed;
        led_packets[i + 1][13] = opts[i].mod2;
        led_packets[i + 1][14] = opts[i].brightness;
    }
}

option breathing_option[] = {
    {"rgb", parse_rgb, 1},
    {"speed", parse_speed, 1},
    {"brightness", parse_brightness, 1}
};

void set_light_breathing(int argc, char **argv, int mode)
{
    printf("LED Breathing\n");
    if(argc <= 0) {
        printf("No further command is given, default values for options will not be changed\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // change to breathing hexs
    temp_cfg.mod1 = 0x1;
    temp_cfg.mod2 = 0x4;

    // calling breathing subcommands
    sub_dispatch(breathing_option, ARRAY_SIZE(breathing_option), &temp_cfg, argc, argv);

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    //saving changes to file
    mouse_config *all_mode = all_mode_configs();
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }

    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;
}

option full_lighted_option[] = {
    {"rgb", parse_rgb, 1},
    {"brightness", parse_brightness, 1}
};

void set_light_lighted(int argc, char **argv, int mode) 
{
    printf("Changing LED mode to Fully Lighted\n");
    if(argc <= 0) {
        printf("No further command is given, default values for options will not be changed\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // full lighted hexs
    temp_cfg.mod1 = 0x1;
    temp_cfg.mod2 = 0x2;
    temp_cfg.speed = 0x0;

    // calling breathing subcommands
    sub_dispatch(full_lighted_option, ARRAY_SIZE(full_lighted_option), &temp_cfg, argc, argv);

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    mouse_config *all_mode = all_mode_configs();
    //saving changes to file
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }
    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;
}

option rainbow_option[] = {
    {"speed", parse_speed, 1},
    {"brightness", parse_brightness, 1}
};

void set_light_rainbow(int argc, char **argv, int mode) 
{
    printf("Changing LED mode to Rainbow\n");
    if(argc <= 0) {
        printf("No further command is given, default values for options will not be changed\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // raibow hexs
    temp_cfg.mod1 = 0x1;
    temp_cfg.mod2 = 0x8;
    temp_cfg.r = 0xFF;
    temp_cfg.g = 0x0;
    temp_cfg.b = 0x0;

    // calling breathing subcommands
    sub_dispatch(rainbow_option, ARRAY_SIZE(rainbow_option), &temp_cfg, argc, argv);

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    mouse_config *all_mode = all_mode_configs();
    //saving changes to file
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }
    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;
}

option wave_option[] = {
    {"speed", parse_speed, 1},
    {"brightness", parse_brightness, 1}
};

void set_light_wave(int argc, char **argv, int mode) 
{
    printf("Changing LED mode to Wave\n");
    if(argc <= 0) {
        printf("No further command is given, default values for options will not be changed\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // wave hexs
    temp_cfg.mod1 = 0x2;
    temp_cfg.mod2 = 0x0;
    temp_cfg.r = 0xFF;
    temp_cfg.g = 0x0;
    temp_cfg.b = 0x0;

    // calling breathing subcommands
    sub_dispatch(wave_option, ARRAY_SIZE(wave_option), &temp_cfg, argc, argv);

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    mouse_config *all_mode = all_mode_configs();
    //saving changes to file
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }
    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;   
}

option trace_option[] = {
    {"speed", parse_speed, 1},
    {"brightness", parse_brightness, 1}
};

void set_light_trace(int argc, char **argv, int mode) 
{
    printf("Changing LED mode to Go Without Trace\n");
    if(argc <= 0) {
        printf("No further command is given, default values for options will not be changed\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // trace hexs
    temp_cfg.mod1 = 0x6;
    temp_cfg.mod2 = 0x0;
    temp_cfg.r = 0xFF;
    temp_cfg.g = 0x0;
    temp_cfg.b = 0x0;

    // calling breathing subcommands
    sub_dispatch(trace_option, ARRAY_SIZE(trace_option), &temp_cfg, argc, argv);

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    mouse_config *all_mode = all_mode_configs();
    //saving changes to file
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }
    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;   
}

option reactive_option[] = {
    {"speed", parse_speed, 1},
    {"brightness", parse_brightness, 1}
};

void set_light_reactive(int argc, char **argv, int mode) 
{
    printf("Changing LED mode to Reactive\n");
    if(argc <= 0) {
        printf("No further command is given, default values for options will not be changed\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // reactive hexs
    temp_cfg.mod1 = 0x7;
    temp_cfg.mod2 = 0x0;
    temp_cfg.r = 0xFF;
    temp_cfg.g = 0x0;
    temp_cfg.b = 0x0;

    // calling breathing subcommands
    sub_dispatch(reactive_option, ARRAY_SIZE(reactive_option), &temp_cfg, argc, argv);

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    mouse_config *all_mode = all_mode_configs();
    //saving changes to file
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }
    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;
}

option flash_option[] = {
    {"rgb", parse_rgb, 1},
    {"speed", parse_speed, 1},
    {"brightness", parse_brightness, 1}
};

void set_light_flash(int argc, char **argv, int mode)
{
    printf("Changing LED mode to Flash\n");
    if(argc <= 0) {
        printf("No further command is given, default values for options will not be changed\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // flash hexs
    temp_cfg.mod1 = 0x1;
    temp_cfg.mod2 = 0x10;

    // calling breathing subcommands
    sub_dispatch(reactive_option, ARRAY_SIZE(reactive_option), &temp_cfg, argc, argv);

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    mouse_config *all_mode = all_mode_configs();
    //saving changes to file
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }
    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;   
}

void set_light_off(int argc, char **argv, int mode)
{
    printf("Changing LED mode to OFF\n");
    if(argc <= 0 || argv[0]) {
        printf("No further command needed for OFF bruh\n");
    }

    // get current mode settings
    mouse_config temp_cfg = *get_mode_config(mode - 1);

    // flash hexs
    temp_cfg.mod1 = 0x0;
    temp_cfg.mod2 = 0x0;

    //copying data changed in temp_cfg to mouse_config modes array
    set_mode_config(mode - 1, &temp_cfg);

    mouse_config *all_mode = all_mode_configs();
    //saving changes to file
    const char *cfgpath = "config.ini";
    if(!save_config(cfgpath , all_mode)) {
        fprintf(stderr, "Failed to save to %s\n", cfgpath);
    }
    // building array packets with new setting & cfg file
    led_packets_build(all_mode);

    for(size_t i = 0; i < ARRAY_SIZE(led_packets); i++)
    {
        transfer(led_packets[i]);
    }

    //TODO: Call dispatch again
    return;
}

