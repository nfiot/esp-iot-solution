/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ZERO_DETECTION_H_
#define _ZERO_DETECTION_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "driver/gpio.h"
#include "driver/mcpwm_cap.h"
#include "hal/gpio_ll.h"
#if defined(CONFIG_USE_GPTIMER)
#include "driver/gptimer.h"
#else
#include "esp_timer.h"
#endif

#include "soc/gpio_struct.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_private/esp_clk.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "zero_detection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZERO_DETECTION_INIT_CONFIG_DEFAULT() { \
    .valid_time = 6, \
    .invalid_time = 5, \
    .freq_range_min_hz = 45,\
    .freq_range_max_hz = 65,\
    .capture_pin = 2,\
    .event_callback = NULL,\
    .zero_signal_type = SQUARE_WAVE,\
}

/**
 * @brief Zero detection events
 */
typedef enum {
    SIGNAL_FREQ_OUT_OF_RANGE = 0,  //Event when the current signal frequency exceeds the set range
    SIGNAL_VALID,   //Event when the frequency of the current signal is within the range, and the number of occurrences surpasses the user-set limit
    SIGNAL_INVALID, //Event when the frequency of the current signal exceeds the range, and the number of occurrences surpasses the user-set limit
    SIGNAL_LOST,    //Event occurring when no signal is received within 100ms
    SIGNAL_RISING_EDGE,            //Event on the rising edge of the signal
    SIGNAL_FALLING_EDGE,           //Event on the falling edge of the signal
} zero_detect_event_t;

/**
 * @brief Zero detection wave type
 */
typedef enum {
    SQUARE_WAVE = 0,
    PULSE_WAVE,
} zero_signal_type_t;

/**
 * @brief Event callback parameters union
 */
typedef union {
    /**
     * @brief Signal exceeds frequency range data return type
     */
    struct {
        mcpwm_capture_edge_t cap_edge;
        uint32_t full_cycle_ticks; 
    } signal_freq_event_data_t;

    /**
     * @brief Signal valid data return type
     */
    struct {
        mcpwm_capture_edge_t cap_edge;
        uint32_t full_cycle_ticks; 
        uint16_t valid_count;
    } signal_valid_event_data_t;

    /**
     * @brief Signal invalid data return type
     */
    struct {
        mcpwm_capture_edge_t cap_edge;
        uint32_t full_cycle_ticks; 
        uint16_t invalid_count;
    } signal_invalid_event_data_t;

    /**
     * @brief Signal rising edge data return type
     */
    struct {
        uint16_t valid_count;
        uint16_t invalid_count;
        uint32_t full_cycle_ticks; 
    } signal_rising_edge_event_data_t;

    /**
     * @brief Signal falling edge data return type
     */
    struct {
        uint16_t valid_count;
        uint16_t invalid_count;
        uint32_t full_cycle_ticks; 
    } signal_falling_edge_event_data_t;
} zero_detect_cb_param_t;

/**
 * @brief Callback format
 */
typedef int (*esp_zero_detect_cb_t)(zero_detect_event_t zero_detect_event, zero_detect_cb_param_t *param);

/**
 * @brief User config data type
 */
typedef struct {
    uint32_t capture_pin;
    uint16_t valid_time;            //Count value when the signal is valid
    uint16_t invalid_time;          //Count value when thea signl exceeds the frequency range
    zero_signal_type_t zero_signal_type;          //Zero Crossing Signal Type
    double freq_range_max_hz;       //Maximum value of the frequency range when determining a valid signal
    double freq_range_min_hz;       //Minimum value of the frequency range when determining a valid signal
    esp_zero_detect_cb_t event_callback;
} zero_detect_config_t;

typedef void *zero_detect_handle_t;

/**
 * @brief Create a zero detect target
 *
 * @param config A zero detect object to config
 *
 * @return
 *      - zero_detect_handle_t  A zero cross detection object
 */
zero_detect_handle_t zero_detect_create(zero_detect_config_t *config);

/**
 * @brief Show zero detect test data
 *
 * @param zcd_handle A zero detect handle
 *
 */
void zero_show_data(zero_detect_handle_t zcd_handle);

/**
 * @brief Delete a zero detect device
 *
 * @param zcd_handle A zero detect handle
 *
 * @return
 *      - ESP_OK  Success
 *      - ESP_FAIL Failure
 */
esp_err_t zero_detect_delete(zero_detect_handle_t zcd_handle);

/**
 * @brief Get relay power status
 *
 * @param zcd_handle A zero detect handle
 *
 * @return
 *      - true  Power on
 *      - false Power off
 */
bool zero_detect_get_power_status(zero_detect_handle_t zcd_handle);

/**
 * @brief Get singal invaild status
 *
 * @param zcd_handle A zero detect handle
 * 
 * @return
 *      - true  Signal is invaild
 *      - false Signal is vaild
 */
bool zero_detect_singal_invaild_status(zero_detect_handle_t zcd_handle);

#endif

#ifdef __cplusplus
}
#endif