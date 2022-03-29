#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
// #include "lvgl.h"
// #include "espressif/esp_dsp.h"
#include "bit_bang.h"
#include "cosmo.h"

static const char *TAG = "mac_se_video_3_24_2022_main";

#define DISP_PIXEL_CLOCK_HZ (16 * 1000 * 1000)
#define DISP_H_RES (512)
#define DISP_V_RES (342)
#define DISP_H_PW (720)
#define DISP_H_BP (0)
#define DISP_H_FP (0)
#define DISP_V_PW (2)
#define DISP_V_BP (0)
#define DISP_V_FP (0)
#define DISP_HSYNC_IDLE_LOW (0)
#define DISP_VSYNC_IDLE_LOW (0)

#define DISP_PIN_NUM_HSYNC 47
#define DISP_PIN_NUM_VSYNC 48
#define DISP_PIN_NUM_DE -1
#define DISP_PIN_NUM_PCLK 21
#define DISP_PIN_NUM_DATA0 3   // B0
#define DISP_PIN_NUM_DATA1 4   // B1
#define DISP_PIN_NUM_DATA2 5   // B2
#define DISP_PIN_NUM_DATA3 6   // B3
#define DISP_PIN_NUM_DATA4 7   // B4
#define DISP_PIN_NUM_DATA5 8   // G0
#define DISP_PIN_NUM_DATA6 9   // G1
#define DISP_PIN_NUM_DATA7 10  // G2
#define DISP_PIN_NUM_DATA8 11  // G3
#define DISP_PIN_NUM_DATA9 12  // G4
#define DISP_PIN_NUM_DATA10 13 // G5
#define DISP_PIN_NUM_DATA11 14 // R0
#define DISP_PIN_NUM_DATA12 15 // R1
#define DISP_PIN_NUM_DATA13 16 // R2
#define DISP_PIN_NUM_DATA14 17 // R3
#define DISP_PIN_NUM_DATA15 18 // R4
#define DISP_PIN_NUM_DISP_EN -1

void frame_done_cb(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    return;
}

const esp_lcd_rgb_timing_t panel_timing =
{
    .pclk_hz = DISP_PIXEL_CLOCK_HZ,
    .h_res = DISP_H_RES,
    .v_res = DISP_V_RES,
    .hsync_pulse_width = DISP_H_PW,
    .hsync_back_porch = DISP_H_BP,
    .hsync_front_porch = DISP_H_FP,
    .vsync_pulse_width = DISP_V_PW,
    .vsync_back_porch = DISP_V_BP,
    .vsync_front_porch = DISP_V_FP,
    .flags = {
        .hsync_idle_low = DISP_HSYNC_IDLE_LOW,
        .vsync_idle_low = DISP_VSYNC_IDLE_LOW,
        .pclk_active_neg = 0,
    }
};

esp_lcd_rgb_panel_config_t panel_cfg =
    {
        .clk_src = LCD_CLK_SRC_PLL160M,
        .timings = panel_timing,
        .data_width = 16,
        .hsync_gpio_num = DISP_PIN_NUM_HSYNC,
        .vsync_gpio_num = DISP_PIN_NUM_VSYNC,
        .de_gpio_num = DISP_PIN_NUM_DE,
        .pclk_gpio_num = DISP_PIN_NUM_PCLK,
        .data_gpio_nums = {
            DISP_PIN_NUM_DATA0,
            DISP_PIN_NUM_DATA1,
            DISP_PIN_NUM_DATA2,
            DISP_PIN_NUM_DATA3,
            DISP_PIN_NUM_DATA4,
            DISP_PIN_NUM_DATA5,
            DISP_PIN_NUM_DATA6,
            DISP_PIN_NUM_DATA7,
            DISP_PIN_NUM_DATA8,
            DISP_PIN_NUM_DATA9,
            DISP_PIN_NUM_DATA10,
            DISP_PIN_NUM_DATA11,
            DISP_PIN_NUM_DATA12,
            DISP_PIN_NUM_DATA13,
            DISP_PIN_NUM_DATA14,
            DISP_PIN_NUM_DATA15,
        },
        .disp_gpio_num = DISP_PIN_NUM_DISP_EN,
        // .on_frame_trans_done = frame_done_cb,
        .user_ctx = NULL,
        .flags = {.relax_on_idle = 0, .fb_in_psram = 1}};

esp_lcd_panel_handle_t panel_handle = NULL;

void app_main(void)
{
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_cfg, &panel_handle));

    // Reset the display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ESP_LOGI(TAG, "sizeof(unsigned int) = %d", sizeof(unsigned int));
    // vTaskDelay(pdMS_TO_TICKS(1000));

    // size_t fb_size = DISP_H_RES * DISP_V_RES * panel_cfg.data_width / 8;
    // uint16_t(*s_lines)[DISP_H_RES] = heap_caps_calloc(1, fb_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // if (!s_lines)
    // {
    //     ESP_LOGE(TAG, "no mem for framebuffer");
    //     while(1)
    //         ;
    // }

    uint8_t *framebuffer = heap_caps_calloc(1, (DISP_H_RES * DISP_V_RES), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!framebuffer)
    {
        ESP_LOGE(TAG, "no mem for framebuffer");
        while(1)
            ;
    }
    unpack_framebuffer(cosmo, framebuffer);

    // for (int row = 0; row < DISP_V_RES; row++)
    // {
    //     for (int col = 0; col < DISP_H_RES; col++)
    //     {
    //         if (col % 2 == 0)
    //             s_lines[row][col] = 1;
    //         else
    //             s_lines[row][col] = 0;
    //     }
    // }

    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);

    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_5, 0);
    int heartbeat_level = 0;

    // spin forever
    while (1)
    {
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        int64_t time_us_start = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

        // esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, DISP_H_RES, DISP_V_RES, s_lines);

        write_frame(framebuffer, GPIO_NUM_19);

        gpio_set_level(GPIO_NUM_5, heartbeat_level);
        heartbeat_level ^= 1;
        ESP_LOGI(TAG, "frame");

        gettimeofday(&tv_now, NULL);
        int64_t time_us_end = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

        ESP_LOGI(TAG, "elapsed %llu ms", (time_us_end - time_us_start)/1000);
        // vTaskDelay(pdMS_TO_TICKS(10));
    }
}
