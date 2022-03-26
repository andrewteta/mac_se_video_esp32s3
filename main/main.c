#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"

static const char *TAG = "mac_se_video_3_24_2022_main";

#define MAC_PIXEL_CLOCK_HZ (16 * 1000 * 1000)
#define MAC_H_RES (512)
#define MAC_V_RES (342)
#define MAC_H_PW (40)
#define MAC_H_BP (20)
#define MAC_H_FP (20)
#define MAC_V_PW (180)
#define MAC_V_BP (20)
#define MAC_V_FP (20)
#define MAC_HSYNC_IDLE_LOW (0)
#define MAC_VSYNC_IDLE_LOW (0)

#define EXAMPLE_PIN_NUM_HSYNC 47
#define EXAMPLE_PIN_NUM_VSYNC 48
#define EXAMPLE_PIN_NUM_DE -1
#define EXAMPLE_PIN_NUM_PCLK 21
#define EXAMPLE_PIN_NUM_DATA0 3   // B0
#define EXAMPLE_PIN_NUM_DATA1 4   // B1
#define EXAMPLE_PIN_NUM_DATA2 5   // B2
#define EXAMPLE_PIN_NUM_DATA3 6   // B3
#define EXAMPLE_PIN_NUM_DATA4 7   // B4
#define EXAMPLE_PIN_NUM_DATA5 8   // G0
#define EXAMPLE_PIN_NUM_DATA6 9   // G1
#define EXAMPLE_PIN_NUM_DATA7 10  // G2
#define EXAMPLE_PIN_NUM_DATA8 11  // G3
#define EXAMPLE_PIN_NUM_DATA9 12  // G4
#define EXAMPLE_PIN_NUM_DATA10 13 // G5
#define EXAMPLE_PIN_NUM_DATA11 14 // R0
#define EXAMPLE_PIN_NUM_DATA12 15 // R1
#define EXAMPLE_PIN_NUM_DATA13 16 // R2
#define EXAMPLE_PIN_NUM_DATA14 17 // R3
#define EXAMPLE_PIN_NUM_DATA15 18 // R4
#define EXAMPLE_PIN_NUM_DISP_EN -1

void frame_done_cb(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    return;
}

const esp_lcd_rgb_timing_t panel_timing =
{
    .pclk_hz = MAC_PIXEL_CLOCK_HZ,
    .h_res = MAC_H_RES,
    .v_res = MAC_V_RES,
    .hsync_pulse_width = MAC_H_PW,
    .hsync_back_porch = MAC_H_BP,
    .hsync_front_porch = MAC_H_FP,
    .vsync_pulse_width = MAC_V_PW,
    .vsync_back_porch = MAC_V_BP,
    .vsync_front_porch = MAC_V_FP,
    {
        .hsync_idle_low = MAC_HSYNC_IDLE_LOW,
        .vsync_idle_low = MAC_VSYNC_IDLE_LOW,
    }
};

esp_lcd_rgb_panel_config_t panel_cfg =
    {
        .clk_src = LCD_CLK_SRC_PLL160M,
        .timings = panel_timing,
        .data_width = 16,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            EXAMPLE_PIN_NUM_DATA3,
            EXAMPLE_PIN_NUM_DATA4,
            EXAMPLE_PIN_NUM_DATA5,
            EXAMPLE_PIN_NUM_DATA6,
            EXAMPLE_PIN_NUM_DATA7,
            EXAMPLE_PIN_NUM_DATA8,
            EXAMPLE_PIN_NUM_DATA9,
            EXAMPLE_PIN_NUM_DATA10,
            EXAMPLE_PIN_NUM_DATA11,
            EXAMPLE_PIN_NUM_DATA12,
            EXAMPLE_PIN_NUM_DATA13,
            EXAMPLE_PIN_NUM_DATA14,
            EXAMPLE_PIN_NUM_DATA15,
        },
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
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

    size_t fb_size = MAC_H_RES * MAC_V_RES * panel_cfg.data_width / 8;
    uint16_t(*s_lines)[MAC_H_RES] = heap_caps_calloc(1, fb_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!s_lines)
    {
        ESP_LOGE(TAG, "no mem for framebuffer");
        while(1)
            ;
    }

    for (int row = 0; row < MAC_V_RES; row++)
    {
        for (int col = 0; col < MAC_H_RES; col++)
        {
            if (col % 2 == 0)
                s_lines[row][col] = 1;
            else
                s_lines[row][col] = 0;
        }
    }

    // spin forever
    while (1)
    {
        for (int y = 0; y < MAC_V_RES; y += 1)
        {
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, 0 + MAC_H_RES, y + 1, s_lines[y]);
        }
        // esp_lcd_panel_draw_bitmap(panel_handle, 0, 1, 0, 10, 10, {1, 2, 3});
        ESP_LOGI(TAG, "Spinning...");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
