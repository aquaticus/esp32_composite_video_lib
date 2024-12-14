#include "video.h"
#include "esp_log.h"
#include <esp_err.h>

static const char *TAG = "test_pattern";

void app_main(void)
{
    ESP_LOGI(TAG, "Philips PM5544 test pattern");

    ESP_LOGI(TAG, "CPU Speed %d MHz", CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ);
    assert(240 == CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ);

#if CONFIG_VIDEO_MODE_NTSC
    ESP_LOGI(TAG, "NTSC mode");
    video_test_ntsc(VIDEO_TEST_PM5544);
#else
    ESP_LOGI(TAG, "PAL mode");
    video_test_pal(VIDEO_TEST_PM5544);
#endif
}
