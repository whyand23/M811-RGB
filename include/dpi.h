#ifndef DPI_H
#define DPI_H

#include "dispatcher.h"

#define PACKET_SIZE 16
#define DPI_LEVELS 5
#define MODES_COUNT 5

typedef unsigned char packet[PACKET_SIZE];
typedef struct {
    packet dpi[DPI_LEVELS];
} mode_packets;

extern mode_packets dpi_packets[MODES_COUNT];

void dpi_main(int argc, char **argv, int mode);

#endif