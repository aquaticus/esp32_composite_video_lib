idf_component_register(
    SRCS "demo.c" "rhino.c" "pm5544.c"
    INCLUDE_DIRS "."
    REQUIRES esp32_composite_video_lib lvgl driver nvs_flash esp_wifi efuse esp_app_format
)
