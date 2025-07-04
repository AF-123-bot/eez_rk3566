cmake_minimum_required(VERSION 3.16)

project(eez_rk3566 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")

set(CMAKE_BUILD_TYPE Debug)

set(LV_BUILD_CONF_PATH "${PROJECT_SOURCE_DIR}/lv_conf.h")

add_subdirectory(lvgl)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GPIOD REQUIRED libgpiod)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(PNG REQUIRED libpng)

add_executable(${PROJECT_NAME}
  src/main.c
  src/gpio.c
  src/uart.c
  src/data_save_load.c

  lib/ui.c
  lib/screens.c
  lib/images.c
  lib/styles.c

  lib/ui_font_14.c
  lib/ui_font_16.c
  lib/ui_font_18.c
  lib/ui_font_20.c
  lib/ui_font_22.c
  lib/ui_font_24.c
  lib/ui_font_26.c
  lib/ui_font_28.c
  lib/ui_font_30.c
  lib/ui_font_32.c
  lib/ui_font_34.c
  lib/ui_font_36.c

  lib/ui_image_b5.c
  lib/ui_image_bar.c
  lib/ui_image_barrel_empty.c
  lib/ui_image_barrel_full.c
  lib/ui_image_calendar_symbol.c
  lib/ui_image_home.c
  lib/ui_image_net.c
  lib/ui_image_background.c
  lib/ui_image_background1.c
  lib/ui_image_background2.c
  lib/ui_image_setting1.c
  lib/ui_image_setting7.c
  lib/ui_image_warning_background.c
  lib/ui_image_warning_icon.c
  lib/ui_image_sr_back.c
  lib/ui_image_shutdown.c
  lib/ui_image_reboot.c
  lib/ui_image_power.c

  lib/actions.c
  lib/eez-flow.cpp
  lib/vars.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "main")

target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/lib
  ${PROJECT_SOURCE_DIR}/lvgl
  ${GPIOD_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
  /usr/include
)

target_link_libraries(${PROJECT_NAME}
  lvgl
  pthread
  ${GPIOD_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${PNG_LIBRARIES}
  cjson
)
