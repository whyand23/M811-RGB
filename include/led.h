#ifndef LED_H
#define LED_H

#include "mouse_config.h"
#include "dispatcher.h"

// reverse engineering led packages sent to mouse
// 1-5 is the mode it self, the rest is unknown but needed to be sent
extern unsigned char led_packets[8][16];

void set_light_breathing(int argc, char **argv, int mode);
void set_light_lighted(int argc, char **argv, int mode); 
void set_light_rainbow(int argc, char **argv, int mode); 
void set_light_wave(int argc, char **argv, int mode); 
void set_light_trace(int argc, char **argv, int mode); 
void set_light_reactive(int argc, char **argv, int mode); 
void set_light_flash(int argc, char **argv, int mode); 
void set_light_off(int argc, char **argv, int mode); 

void led_main(int argc, char **argv, int mode);

extern command_t led_sub[8];

#endif