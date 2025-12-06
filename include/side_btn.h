#ifndef SIDE_BTN_H
#define SIDE_BTN_H

#include "mouse_config.h"
#include "dispatcher.h"

#define PACKET_SIZE 16
#define SIDE_BUTTON_COUNT 8
#define MODES_COUNT 5

typedef unsigned char packet[PACKET_SIZE];

typedef struct
{
    packet btn[SIDE_BUTTON_COUNT];
} side_mode_packets;

extern side_mode_packets side_btn_packets[MODES_COUNT];

void side_btn_main(int argc, char **argv, int mode);
void side_btn_key(int argc, char **argv, int mode);
extern command_t side_btn_sub[1];

#endif