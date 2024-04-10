#include "I2C.h"
#include "stdbool.h"

#define RTC_ADDR 0xD0

// Source: https://github.com/libdriver/ds1307/blob/master/src/driver_ds1307.c 
#define REG_SECOND               0x00        /**< second register */
#define REG_MINUTE               0x01        /**< minute register */
#define REG_HOUR                 0x02        /**< hour register */
#define REG_WEEK                 0x03        /**< week register */
#define REG_DATE                 0x04        /**< date register */
#define REG_MONTH                0x05        /**< month register */
#define REG_YEAR                 0x06        /**< year register */
#define REG_CONTROL              0x07        /**< control register */
#define REG_RAM                  0x08        /**< ram start register */

void RTC_Init() {
    uint8_t seconds = 0x00; // 0 s and enable oscillator bit
    write(0x00, seconds);

    uint8_t minutes = 0x30; // 30 m
    write(0x01, minutes);

    uint8_t hours = 0x01;   // 1 hour in 24 hour clock
    write(0x02, seconds);

    uint8_t day = 0x02; // Monday
    write(0x03, day);

    uint8_t date = 0x15; // The 15th
    write(0x04, date);

    uint8_t month = 0x04;   // April
    write(0x05, month);

    uint8_t year = 0x24;    // 24 (2024!)
    write(0x06, year);
}

uint8_t bcd_to_decimal(uint8_t input) {
    // Lower byte
    uint8_t lower = input & 0x0F;
    int lower_value;

    // Upper byte
    uint8_t upper = input & 0xF0;
    int upper_value;

    if (lower == 0x01) {
        lower_value = 1;
    } else if (lower == 0x02) {
        lower_value = 2;
    } else if (lower == 0x03) {
        lower_value = 3;
    } else if (lower == 0x04) {
        lower_value = 4;
    } else if (lower == 0x05) {
        lower_value = 5;
    } else if (lower == 0x06) {
        lower_value = 6;
    } else if (lower == 0x07) {
        lower_value = 7;
    } else if (lower == 0x08) {
        lower_value = 8;
    } else if (lower == 0x09) {
        lower_value = 9;
    }

    if (upper == 0x10) {
        upper_value = 10;
    } else if (upper == 0x20) {
        upper_value = 20;
    } else if (upper == 0x30) {
        upper_value = 30;
    } else if (upper == 0x40) {
        upper_value = 40;
    } else if (upper == 0x50) {
        upper_value = 50;
    } else if (upper == 0x60) {
        upper_value = 60;
    } else if (upper == 0x70) {
        upper_value = 70;
    } else if (upper == 0x80) {
        upper_value = 80;
    } else if (upper == 0x90) {
        upper_value = 90;
    }

    return (upper_value + lower_value);
}

uint8_t RTC_getTime(rtc* rtc) {
    uint8_t buffer;

    start_read(RTC_ADDR);

    // Read byte and send acknowledge
    for (int i = 0; i < 7; i++) {
        // Read byte
        buffer = read();
        if (i == 0) {
            rtc->seconds = bcd_to_decimal(buffer);
        } else if (i == 1) {
            rtc->minutes = bcd_to_decimal(buffer);
        } else if (i == 2) {
            rtc->hours = bcd_to_decimal(buffer);
        } else if (i == 3) {
            rtc->day = bcd_to_decimal(buffer);
        } else if (i == 4) {
            rtc->date = bcd_to_decimal(buffer);
        } else if (i == 5) {
            rtc->month = bcd_to_decimal(buffer);
        } else if (i == 6) {
            rtc->year = bcd_to_decimal(buffer);
        }
    }

    // Send NACK
    stop_read();

    return buffer;
}