#ifndef MAIN_H
#define MAIN_H
    
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

// fatal error check with cleanup macro
// libusb return 0 on sucess so we just need to check if its true or not
#define die(X) { \
    perror(X); \
    if(errcode) fprintf(stderr, "libusb_strerror: %s", libusb_strerror(errcode)); \
    if(device) libusb_close(device); \
    if(context) libusb_exit(context); \
    return 1; \
}

int transfer(unsigned char *packets) ;

#endif