#include <stdio.h>
#include "esp_log.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "mac_se_video_3_24_2022_main";

esp_lcd_rgb_panel_frame_trans_done_cb_t frame_done_cb(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    return;
}

const esp_lcd_rgb_timing_t panel_timing =
{
    .pclk_hz = 16000000,
    .h_res = 512,
    .v_res = 342,
    .hsync_pulse_width = 45,
    .hsync_back_porch = 20,
    .hsync_front_porch = 20,
    .vsync_pulse_width = 180,
    .vsync_back_porch = 20,
    .vsync_front_porch = 20,
    {
        .hsync_idle_low = 0,
        .vsync_idle_low = 0,
        .de_idle_high = 1,
        .pclk_idle_high = 1
    }
};

esp_lcd_rgb_panel_config_t panel_cfg =
{
    .clk_src = LCD_CLK_SRC_PLL160M,
    .timings = panel_timing,
    .data_width = 16,
    .hsync_gpio_num = 40,
    .vsync_gpio_num = 41,
    .de_gpio_num = -1,
    .pclk_gpio_num = 42,
    // .data_gpio_nums = {39},
    .disp_gpio_num = -1,
    // .on_frame_trans_done = frame_done_cb,
    .user_ctx = NULL,
    {
        .relax_on_idle = 0,
        .fb_in_psram = 0
    }
};

esp_lcd_panel_handle_t panel_handle;

void app_main(void)
{
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_cfg, &panel_handle));

    // Reset the display
    // ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

    // // Initialize LCD panel
    // ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // spin forever
    while (1)
    {
        ESP_LOGI(TAG, "Spinning...");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
