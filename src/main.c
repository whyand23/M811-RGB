#include "led.h"
#include "dispatcher.h"
#include "dpi.h"
#include "side_btn.h"
#include "main.h"

#define CLAY_IMPLEMENTATION
#include "clay.h"     // vendor/clay/clay.h
#include "raylib.h"


#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

// device info
#define idVendor 0x04d9
#define idProduct 0xfc6d
#define wLength 0x10
#define outInterfaceNumber 2
#define outEndPoint 0x0
#define bmRequestType 0x21
#define bRequest 0x09
#define wValue 0x0302
#define wIndex 2

static libusb_device_handle *device = 0; // init to 0 Struct representing a handle on a USB device.
static libusb_context *context = 0; // init to 0 Struct representing a libusb session.
static int errcode = 0; // libusb will return 0 on sucess

// transfer from 1d array
// return 1 on success, 0 on fail
int transfer(unsigned char *packets) 
{
    int sent = libusb_control_transfer( device
                                    , bmRequestType
                                    , bRequest
                                    , wValue
                                    , wIndex
                                    , packets
                                    , wLength
                                    , 1000);
                                        
    if(sent != wLength) {
        errcode = sent;
        die("libusb_control_transfer failed")
        return 0;
    }

    return 1;
}


int main(int argc, char **argv) 
{
    // if(argc <= 3) {
    //     printf("bla bla bla");
    //     return 0;
    // }
  
    // int input_mode = atoi(argv[2]);

    // if(strcmp(argv[1], "mode") != 0 || input_mode < 0 || input_mode > 5)
    // {
    //     return fprintf(stderr, "Need a valid mouse mode/%s", argv[2]);
    // }


    // // Before anything lib usb need to be initialized
    // if((errcode = libusb_init(&context))) { 
    //     die("libusb_init failed")
    // }

    // // Open device
    // if(!(device = libusb_open_device_with_vid_pid(context, idVendor, idProduct))) {
    //     die("libusb_open_device_with_vid_pid failed")
    // }

    // // Tell libusb to automatically detach kernel driver
    // // when usb claimed (device var), and reattach when released
    // if((errcode = libusb_set_auto_detach_kernel_driver(device, true))) {
    //     die("libusb_set_auto_detach_kernel_driver")
    // }
    
    // // Claim interface
    // if((errcode = libusb_claim_interface(device, outInterfaceNumber))) {
    //     die("libusb_claim_interface failed")
    // }

    // command_t commands[] = {
    //     {"led", NULL, "Set LED", led_sub, ARRAY_SIZE(led_sub)},
    //     {"dpi", dpi_main, "Set DPI", NULL, 0},
    //     {"side_btn", NULL, "Set Side Button binds", side_btn_sub, ARRAY_SIZE(side_btn_sub)}
    // };

    // // reading config file
    // const char *cfgpath = "config.ini";
    // mouse_config *temp = all_mode_configs();
    // if(!load_config(cfgpath, temp)) {
    //     printf("Config not found saving config to %s\n", cfgpath);
    //     save_config(cfgpath, temp);
    // } else {
    //     printf("Config loaded from %s\n", cfgpath);
    // }

    // // calling dispatcher of commands
    // dispatch(commands, ARRAY_SIZE(commands), argc - 3, &argv[3], input_mode);

    // printf("led mod1 on mode 1: %d\n", led_packets[1][11]);
    // printf("led mod1 on mode 1: %d\n", led_packets[1][13]);
    
    // printf("rgb packets r: %d\n", led_packets[input_mode][8]);
    // printf("rgb packets g: %d\n", led_packets[input_mode][9]);
    // printf("rgb packets b: %d\n", led_packets[input_mode][10]);
    // printf("led mod1: %d\n", led_packets[input_mode][11]);
    // printf("led mod2: %d\n", led_packets[input_mode][13]);

    // const mouse_config *foo = get_mode_config(input_mode - 1);
    // mouse_config *all = all_mode_configs();

    // printf("led mod1: %d\n", foo->mod1);
    // printf("led mod2: %d\n", foo->mod2);

    // printf("all led mode 1 mod1: %d\n", all[0].mod1);
    // printf("all led mode 1 mod2: %d\n", all[0].mod2);

    // printf("all led mod1: %d\n", all[input_mode - 1].mod1);
    // printf("all led mod2: %d\n", all[input_mode - 1].mod2);


    // if((errcode = libusb_release_interface(device, outInterfaceNumber))) {
    //     die("libusb_release_interface failed")
    // }

    // libusb_close(device);
    // libusb_exit(context);

    InitWindow(800, 600, "Center Text Test");

    const char *message = "Raylib is working!";
    
    while (!WindowShouldClose()) {

        // Measure how large the rendered text will be
        int textWidth  = MeasureText(message, 20);
        int textHeight = 20;  // Raylib’s default height for font size 20

        // Compute centered coordinates
        int x = (GetScreenWidth()  - textWidth)  / 2;
        int y = (GetScreenHeight() - textHeight) / 2;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(message, x, y, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;

}

