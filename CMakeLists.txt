idf_component_register(SRCS "src/lvgl_driver_video.c" "src/video.c" "src/diag.c"
                    INCLUDE_DIRS "include" 
                    PRIV_REQUIRES driver lvgl
                    )
