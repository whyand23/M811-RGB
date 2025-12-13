#include "main.h"
#include "dispatcher.h"
#include "dpi.h"
#include "led.h"
#include "side_btn.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"
// my autoformatter kept swapping include order
#include "../vendor/clay/renderers/raylib/clay_renderer_raylib.c"

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

Clay_Color ColorButtonHover = {255, 255, 255, 255};
Clay_Color ColorButton = {0, 0, 0, 255};

static libusb_device_handle *device =
    0; // init to 0 Struct representing a handle on a USB device.
static libusb_context *context =
    0;                  // init to 0 Struct representing a libusb session.
static int errcode = 0; // libusb will return 0 on sucess

Font fonts[2];
const int fontCount = 2;

// Stuct that will be used for measure text
typedef struct {
  Font *fonts;
  int fontCount;
} ClayRaylibFonts;

// Clay requirement, measure text
Clay_Dimensions RaylibMeasureText(Clay_StringSlice slice,
                                  Clay_TextElementConfig *config,
                                  void *userData) {
  ClayRaylibFonts *ctx = (ClayRaylibFonts *)userData;

  int fontId = config->fontId;
  float fontSize = config->fontSize;
  float spacing = config->letterSpacing;

  // Safety fallback font
  if (fontId < 0 || fontId >= ctx->fontCount) {
    fontId = 0;
  }

  Font font = ctx->fonts[fontId];

  // Convert Clay_StringSlice to NULL terminated string
  int len = slice.length;
  char buffer[4096];
  if (len >= 4095)
    len = 4095;
  memcpy(buffer, slice.chars, len);
  buffer[len] = '\0';

  // Raylib measurement
  Vector2 m = MeasureTextEx(font, buffer, fontSize, spacing);

  Clay_Dimensions dim;
  dim.width = m.x;
  dim.height = m.y;

  return dim;
}

// App resolution (fixed)
const float ScreenWidth = 896.0f;
const float ScreenHeight = 504.0f;

typedef enum { TEST, PAGE_LED, PAGE_DPI, PAGE_SBTN } Page;

Page currentPage = PAGE_DPI;

void test_btn(void) { currentPage = TEST; }
void led_btn(void) { currentPage = PAGE_LED; }
void dpi_btn(void) { currentPage = PAGE_DPI; }
void sbtn_btn(void) { currentPage = PAGE_SBTN; }

void draw_led_screen(void) {
  CLAY(
      {.layout = {.sizing = {.width = CLAY_SIZING_GROW(),
                             .height = CLAY_SIZING_GROW()},
                  .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}},
       .backgroundColor = Clay_Hovered() ? ColorButtonHover : ColorButton}) {

    CLAY_TEXT(
        CLAY_STRING("LED"),
        CLAY_TEXT_CONFIG(
            {.fontId = 1, .fontSize = 16, .textColor = {255, 255, 255, 255}}));
  }
}

void draw_dpi_screen(void) {
  CLAY(
      {.layout = {.sizing = {.width = CLAY_SIZING_GROW(),
                             .height = CLAY_SIZING_GROW()},
                  .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}},
       .backgroundColor = Clay_Hovered() ? ColorButtonHover : ColorButton}) {

    CLAY_TEXT(
        CLAY_STRING("DPI"),
        CLAY_TEXT_CONFIG(
            {.fontId = 1, .fontSize = 16, .textColor = {255, 255, 255, 255}}));
  }
}

void draw_sbtn_screen(void) {
  CLAY(
      {.layout = {.sizing = {.width = CLAY_SIZING_GROW(),
                             .height = CLAY_SIZING_GROW()},
                  .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}},
       .backgroundColor = Clay_Hovered() ? ColorButtonHover : ColorButton}) {

    CLAY_TEXT(
        CLAY_STRING("SIDE BUTTONS"),
        CLAY_TEXT_CONFIG(
            {.fontId = 1, .fontSize = 16, .textColor = {255, 255, 255, 255}}));
  }
}

void draw_test_screen(void) {
  CLAY(
      {.layout = {.sizing = {.width = CLAY_SIZING_GROW(),
                             .height = CLAY_SIZING_GROW()},
                  .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}},
       .backgroundColor = Clay_Hovered() ? ColorButtonHover : ColorButton}) {

    CLAY_TEXT(
        CLAY_STRING("TEST"),
        CLAY_TEXT_CONFIG(
            {.fontId = 1, .fontSize = 16, .textColor = {255, 255, 255, 255}}));
  }
}

typedef void (*ButtonCallback)(void);
void handle_button_interaction(Clay_ElementId id, Clay_PointerData pointerInfo,
                               intptr_t userData) {
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    ButtonCallback callback = (ButtonCallback)userData;
    if (callback) {
      callback();
    }
  }
}

typedef enum { MODE_1, MODE_2, MODE_3, MODE_4, MODE_5 } MouseMode;

MouseMode currentMode = MODE_1;

Clay_String MouseModeText(MouseMode input) {
  switch (input) {
  case MODE_1:
    return CLAY_STRING("MODE 1");
    break;
  case MODE_2:
    return CLAY_STRING("MODE 2");
    break;
  case MODE_3:
    return CLAY_STRING("MODE 3");
    break;
  case MODE_4:
    return CLAY_STRING("MODE 4");
    break;
  case MODE_5:
    return CLAY_STRING("MODE 5");
    break;
  }
  return CLAY_STRING("UNKNOWN");
}

void mode1_menu_func(void) { currentMode = MODE_1; }

void mode2_menu_func(void) { currentMode = MODE_2; }

void RenderDropdownMenuItem(Clay_String text, ButtonCallback callback) {
  CLAY({.layout = {.padding = CLAY_PADDING_ALL(16)}}) {
    Clay_OnHover(handle_button_interaction, (intptr_t)callback);
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = 1,
                                      .fontSize = 16,
                                      .textColor = {255, 255, 255, 255}}));
  }
}

void RenderFooterBtn(Clay_String text, ButtonCallback callback) {
  CLAY(
      {.layout = {.sizing = {.width = CLAY_SIZING_GROW(),
                             .height = CLAY_SIZING_GROW()},
                  .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}},
       .backgroundColor = Clay_Hovered() ? ColorButtonHover : ColorButton}) {
    Clay_OnHover(handle_button_interaction, (intptr_t)callback);
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = 1,
                                      .fontSize = 16,
                                      .textColor = {255, 255, 255, 255}}));
  }
}

// transfer from 1d array
// return 1 on success, 0 on fail
int transfer(unsigned char *packets) {
  int sent = libusb_control_transfer(device, bmRequestType, bRequest, wValue,
                                     wIndex, packets, wLength, 1000);

  if (sent != wLength) {
    errcode = sent;
    die("libusb_control_transfer failed") return 0;
  }

  return 1;
}

// Handling all clay error
void errorHandlerFunc(Clay_ErrorData errorText) {
  printf("An Error Occured%s", errorText.errorText.chars);
}

Clay_RenderCommandArray build_layout() {
  Clay_BeginLayout();

  // Image
  Texture2D header = LoadTexture("resources/img/header.png");

  // ROOT
  CLAY(.wrapped = {
           .id = CLAY_ID("Root"),
           .layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
                      .padding = CLAY_PADDING_ALL(10),
                      .childGap = 5,
                      .layoutDirection = CLAY_TOP_TO_BOTTOM},
           .backgroundColor = {17, 17, 17, 255}}) {

    // HEADER
    CLAY(.wrapped = {
             .id = CLAY_ID("Header"),
             .image = {.imageData = &header},
             .aspectRatio = {(float)header.width / (float)header.height},
             .backgroundColor = {255, 255, 255, 0},
             .layout = {
                 .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(20)},
                 .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER},
                 .childGap = 16,
                 .padding = CLAY_PADDING_ALL(16)}}) {

      CLAY_TEXT(CLAY_STRING("M811-RGB"),
                CLAY_TEXT_CONFIG({
                    .fontId = 1,
                    .fontSize = 24,
                    .textColor = {255, 255, 255, 255},
                    .textAlignment = CLAY_TEXT_ALIGN_LEFT,
                }));
    }

    // MIDDLE
    CLAY(.wrapped = {
             .id = CLAY_ID("Middle"),
             .layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 16,
                        .layoutDirection = CLAY_LEFT_TO_RIGHT},
             .backgroundColor = {200, 0, 0, 0},
             .border = {.color = {85, 85, 85, 255},
                        .width = {0, 0, 2, 2, 1}}}) {

      // LEFT CONTAINER
      CLAY(.wrapped = {.id = CLAY_ID("LeftContainer"),
                       .layout = {.sizing = {CLAY_SIZING_FIXED(300),
                                             CLAY_SIZING_GROW()},
                                  .layoutDirection = CLAY_TOP_TO_BOTTOM},
                       .backgroundColor = {190, 0, 0, 255}}) {
        CLAY({.id = CLAY_ID("ModeSelect"),
              .layout = {.childAlignment = {CLAY_ALIGN_X_CENTER,
                                            CLAY_ALIGN_Y_CENTER},
                         .sizing = {.width = CLAY_SIZING_GROW(),
                                    .height = CLAY_SIZING_FIXED(20)}},
              .backgroundColor = {255, 255, 255, 0}}) {
          // ModeSelect child

          CLAY_TEXT(MouseModeText(currentMode),
                    CLAY_TEXT_CONFIG({.fontId = 1,
                                      .fontSize = 20,
                                      .textColor = {255, 255, 255, 255}}));

          bool modeMenuVisible =
              Clay_PointerOver(Clay_GetElementId(CLAY_STRING("ModeSelect"))) ||
              Clay_PointerOver(Clay_GetElementId(CLAY_STRING("ModeMenu")));

          if (modeMenuVisible) {
            CLAY({.id = CLAY_ID("ModeMenu"),
                  .floating = {.attachTo = CLAY_ATTACH_TO_PARENT,
                               .attachPoints =
                                   {.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM}},
                  .layout = {.padding = {0, 0, 0, 8}}}) {
              CLAY({.layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                               .sizing = {.width = CLAY_SIZING_FIXED(200)},
                               .childAlignment = {CLAY_ALIGN_X_CENTER,
                                                  CLAY_ALIGN_Y_CENTER}},
                    .backgroundColor = {40, 40, 40, 255}}) {
                // drop down item for mode is here
                RenderDropdownMenuItem(CLAY_STRING("MODE 1"), mode1_menu_func);
                RenderDropdownMenuItem(CLAY_STRING("MODE 2"), mode2_menu_func);
              }
            }
          }
        }
      }

      // RIGHT CONTAINER
      CLAY(.wrapped = {
               .id = CLAY_ID("RightContainer"),
               .layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
                          .layoutDirection = CLAY_TOP_TO_BOTTOM},
               .backgroundColor = {190, 0, 0, 255}}) {
        switch (currentPage) {
        case TEST:
          draw_test_screen();
          break;
        case PAGE_DPI:
          draw_dpi_screen();
          break;
        case PAGE_LED:
          draw_led_screen();
          break;
        case PAGE_SBTN:
          draw_sbtn_screen();
          break;
        }
      }
    }

    // FOOTER
    CLAY(.wrapped = {
             .id = CLAY_ID("Footer"),
             .layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(30)},
                        .childAlignment = {CLAY_ALIGN_X_CENTER,
                                           CLAY_ALIGN_Y_CENTER},
                        .childGap = 10,
                        .padding = {10, 10, 3, 3},
                        .layoutDirection = CLAY_LEFT_TO_RIGHT},
             .backgroundColor = {190, 0, 0, 255}}) {

      // children go here

      RenderFooterBtn(CLAY_STRING("TEST"), test_btn);
      RenderFooterBtn(CLAY_STRING("DPI MODES"), dpi_btn);
      RenderFooterBtn(CLAY_STRING("LED MODES"), led_btn);
      RenderFooterBtn(CLAY_STRING("SIDE BUTTONS"), sbtn_btn);
    }

  } // end of Root

  return Clay_EndLayout();
}

int main(int argc, char **argv) {
  // if(argc <= 3) {
  //     printf("");
  //     return 0;
  // }

  // int currentMode = atoi(argv[2]);

  // if(strcmp(argv[1], "mode") != 0 || currentMode < 0 || currentMode > 5)
  // {
  //     return fprintf(stderr, "Need a valid mouse mode/%s", argv[2]);
  // }

  // // Before anything lib usb need to be initialized
  // if((errcode = libusb_init(&context))) {
  //     die("libusb_init failed")
  // }

  // // Open device
  // if(!(device = libusb_open_device_with_vid_pid(context, idVendor,
  // idProduct))) {
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
  //     {"side_btn", NULL, "Set Side Button binds", side_btn_sub,
  //     ARRAY_SIZE(side_btn_sub)}
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
  // dispatch(commands, ARRAY_SIZE(commands), argc - 3, &argv[3],
  // currentMode);

  // printf("led mod1 on mode 1: %d\n", led_packets[1][11]);
  // printf("led mod1 on mode 1: %d\n", led_packets[1][13]);

  // printf("rgb packets r: %d\n", led_packets[currentMode][8]);
  // printf("rgb packets g: %d\n", led_packets[currentMode][9]);
  // printf("rgb packets b: %d\n", led_packets[currentMode][10]);
  // printf("led mod1: %d\n", led_packets[currentMode][11]);
  // printf("led mod2: %d\n", led_packets[currentMode][13]);

  // const mouse_config *foo = get_mode_config(currentMode - 1);
  // mouse_config *all = all_mode_configs();

  // printf("led mod1: %d\n", foo->mod1);
  // printf("led mod2: %d\n", foo->mod2);

  // printf("all led mode 1 mod1: %d\n", all[0].mod1);
  // printf("all led mode 1 mod2: %d\n", all[0].mod2);

  // printf("all led mod1: %d\n", all[currentMode - 1].mod1);
  // printf("all led mod2: %d\n", all[currentMode - 1].mod2);

  // if((errcode = libusb_release_interface(device, outInterfaceNumber))) {
  //     die("libusb_release_interface failed")
  // }

  // libusb_close(device);
  // libusb_exit(context);

  // Init Raylib
  Clay_Raylib_Initialize(ScreenWidth, ScreenHeight, "M811-RGB Mouse",
                         FLAG_BORDERLESS_WINDOWED_MODE);

  // Init Clay
  uint64_t ClayMemorySize = Clay_MinMemorySize();
  Clay_Arena memoryArena = {
      .memory = malloc(ClayMemorySize),
      .capacity = ClayMemorySize,

  };
  Clay_Dimensions dimensions = {.height = ScreenHeight, .width = ScreenWidth};
  Clay_Initialize(memoryArena, dimensions,
                  (Clay_ErrorHandler){errorHandlerFunc});

  // init font(s)
  fonts[1] = LoadFont("resources/promptfont.ttf");
  fonts[0] = GetFontDefault();

  // Init ClayRaylibFonts with pointing it to
  // font array and int count
  ClayRaylibFonts clayFontsCtx = {.fonts = fonts, .fontCount = fontCount};

  // Setting up Clay_SetMeasureTextFunction
  Clay_SetMeasureTextFunction(&RaylibMeasureText, &clayFontsCtx);

  // Main UI loop
  while (!WindowShouldClose()) {
    // Get Delta time
    float delta_time = GetFrameTime();

    // Update clay state according raylib data
    Clay_SetPointerState(
        (Clay_Vector2){GetMousePosition().x, GetMousePosition().y},
        IsMouseButtonDown(MOUSE_LEFT_BUTTON));
    Clay_UpdateScrollContainers(
        false, (Clay_Vector2){GetMouseWheelMoveV().x, GetMouseWheelMoveV().y},
        delta_time);

    // Build clay layout
    Clay_RenderCommandArray renderCommands = build_layout();

    //    printf("Working directory: %s\n", GetWorkingDirectory());
    //   printf("Exists? %d\n", FileExists("resources/img/header.png"));

    // Render Stuff
    BeginDrawing();
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
  }

  return 0;
}
