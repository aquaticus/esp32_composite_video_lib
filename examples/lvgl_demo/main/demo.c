#include "video.h"
#include <stdio.h>
#include "esp_log.h"
#include <esp_err.h>
#include "nvs_flash.h"
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <driver/uart.h>
#include "esp_system.h"
#include "driver/rtc_io.h"
#include "esp_efuse.h"
#include "esp32/rom/lldesc.h"
#include <soc/rtc.h>
#include "freertos/queue.h"
#include "esp_flash.h"
#include "esp_chip_info.h"

#include "lvgl_driver_video.h"

static const char *TAG = "DEMO";

#ifndef CONFIG_VIDEO_ENABLE_LVGL_SUPPORT
#pragma GCC error "Nothing to show. Enable LVGL support."
#endif

void demo_pm5544(lv_obj_t *scr)
{
    char mode[14];
    video_get_mode_description(mode, sizeof(mode));

    LV_IMG_DECLARE(pm5544);
    lv_obj_t *img1 = lv_img_create(scr);
    lv_img_set_src(img1, &pm5544);
    lv_obj_set_align(img1, LV_ALIGN_CENTER);
    lv_obj_set_size(img1, pm5544.header.w, pm5544.header.h);
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *top_label = lv_label_create(scr);
    lv_obj_set_width(top_label, 55);
    lv_obj_set_style_text_color(top_label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(top_label, LV_ALIGN_CENTER, 0, -83);

    lv_obj_t *mode_text = lv_label_create(scr);
    lv_obj_set_width(mode_text, 75);
    lv_obj_set_style_text_color(mode_text, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_align(mode_text, LV_ALIGN_CENTER, 0, 66);
    lv_obj_set_style_text_align(mode_text, LV_TEXT_ALIGN_CENTER, 0);

    char *t = strtok(mode, " ");
    lv_label_set_text(top_label, t);
    t = strtok(NULL, " ");
    lv_label_set_text(mode_text, t);
}

void demo_rhino(lv_obj_t *scr)
{
    LV_IMG_DECLARE(rhino);
    lv_obj_t *img1 = lv_img_create(scr);
    lv_img_set_src(img1, &rhino);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(img1, rhino.header.w, rhino.header.h);
}

static lv_obj_t *meter;
static void meter_set_value(void *indic, int32_t v)
{
    lv_meter_set_indicator_value(meter, indic, v);
}

void demo_meter(lv_obj_t *scr)
{
    meter = lv_meter_create(scr);
    lv_obj_center(meter);
    lv_obj_set_size(meter, 200, 200);

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 8, 4, 15, lv_color_black(), 10);

    lv_meter_indicator_t *indic;

    /*Add a blue arc to the start*/
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter, indic, 0);
    lv_meter_set_indicator_end_value(meter, indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 0);
    lv_meter_set_indicator_end_value(meter, indic, 20);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);

    /*Add a needle line indicator*/
    indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, meter_set_value);
    lv_anim_set_var(&a, indic);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

void create_label(lv_style_t *style, lv_obj_t *cont, const char *text, uint8_t row, uint8_t col)
{
    lv_obj_t *obj = lv_label_create(cont);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1,
                         LV_GRID_ALIGN_STRETCH, row, 1);

    lv_label_set_text(obj, text);
    lv_obj_add_style(obj, style, 0);
}

const char *get_chip_name(esp_chip_model_t model)
{
    switch (model)
    {
    case CHIP_ESP32:
        return "ESP32";
        break;

    case CHIP_ESP32S2:
        return "ESP32-S2";
        break;

    case CHIP_ESP32S3:
        return "ESP32-S3";
        break;

    case CHIP_ESP32C3:
        return "ESP32-C3";
        break;

    case CHIP_ESP32H2:
        return "ESP32-H2";
        break;

    default:
        return "UNKNOWN";
        break;
    }
}

void demo_system_info(lv_obj_t *scr)
{
    lv_obj_t *label1 = lv_label_create(scr);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text(label1, "System Information");

    static lv_coord_t col_dsc[] = {160, 80, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {20, 20, 20, 20, 20, LV_GRID_TEMPLATE_LAST};

    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 300, 170);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 5);
    lv_style_set_border_width(&style, 1);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_BLUE, 1));
    lv_style_set_border_color(&style, lv_color_black());
    lv_style_set_pad_left(&style, 5);
    lv_style_set_border_side(&style, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    char value[32];
    const char *const labels[] = {"Chip", "Chip cores", "Bluetooth", "Silicon rev", "FLASH"};
    uint32_t size_flash_chip;

    for (int row = 0; row < 5; row++)
    {
        create_label(&style, cont, labels[row], row, 0);

        switch (row)
        {
        case 0:
            snprintf(value, sizeof(value), "%s", get_chip_name(chip_info.model));
            break;

        case 1:
            snprintf(value, sizeof(value), "%d", chip_info.cores);
            break;

        case 2:
            snprintf(value, sizeof(value), "%s %s", (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "", (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "");
            break;

        case 3:
            snprintf(value, sizeof(value), "%d", chip_info.revision);
            break;

        case 4:
            esp_flash_get_size(NULL, &size_flash_chip);
            snprintf(value, sizeof(value), "%lu MiB", size_flash_chip / (1024 * 1024));
            break;
        }

        create_label(&style, cont, value, row, 1);
    }
}

#define MAX_DEMO_COUNT 4
lv_obj_t *screens[MAX_DEMO_COUNT];
static int screen_index = 0;

void swap_screen_timer_callback(lv_timer_t *timer)
{
    lv_scr_load_anim(screens[screen_index], LV_SCR_LOAD_ANIM_MOVE_LEFT, 1000, 1000, false);
    screen_index++;
    if (screen_index >= MAX_DEMO_COUNT)
    {
        screen_index = 0;
    }
}

void slides_demo(void)
{
    for (size_t i = 0; i < MAX_DEMO_COUNT; i++)
    {
        screens[i] = lv_obj_create(NULL);
    }

    size_t index = 0;
    demo_pm5544(screens[index++]);
    demo_system_info(screens[index++]);
    demo_meter(screens[index++]);
    demo_rhino(screens[index++]);

    assert(index <= MAX_DEMO_COUNT);

    lv_disp_load_scr(screens[screen_index++]);
    const uint32_t timeout_ms = 10000;
    lv_timer_create(swap_screen_timer_callback, timeout_ms, NULL);
}

void run_demo_slides(bool ntsc_mode_on)
{
#if LV_COLOR_DEPTH < 16
    const size_t pixel_count = 320 * 200;
    lv_color_t *lvgl_pixel_buffer = heap_caps_malloc(sizeof(lv_color_t) * pixel_count, MALLOC_CAP_8BIT);
    assert(lvgl_pixel_buffer);

    lv_video_disp_init_buf(ntsc_mode_on ? NTSC_320x200 : PAL_320x200, lvgl_pixel_buffer, pixel_count);
#else
    // no memory for buffers
    ESP_LOGI(TAG, "Using direct framebuffer access. Expect tearing effect for animations.");
    lv_video_disp_init(ntsc_mode_on ? NTSC_320x200 : PAL_320x200);
#endif
    ESP_LOGI(TAG, "Slides Demo");

    slides_demo();

#if CONFIG_VIDEO_DIAG_ENABLE_INTERRUPT_STATS
    int n = 0;
#endif
    while (1)
    {
        lv_task_handler();

        vTaskDelay(20 / portTICK_PERIOD_MS);

#if CONFIG_VIDEO_DIAG_ENABLE_INTERRUPT_STATS
        if (n++ > 50)
        {
            video_show_stats();
            n = 0;
        }
#endif
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "LVGL Demo");

    ESP_LOGI(TAG, "CPU Speed %d MHz", CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ);
    assert(240 == CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ);

#if VIDEO_DIAG_ENABLE_INTERRUPT_STATS
    ESP_LOGI(TAG, "Interrupt timing stats enabled");
#endif

#if CONFIG_VIDEO_MODE_NTSC
    ESP_LOGI(TAG, "NTSC mode");
    run_demo_slides(true);
#else
    ESP_LOGI(TAG, "PAL mode");
    run_demo_slides(false);
#endif
}