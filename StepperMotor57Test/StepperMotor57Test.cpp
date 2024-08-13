#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}
int64_t alarm_callback2(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    puts("500 000 us, testing");
    return 0;
}


int main()
{
    stdio_init_all();

    // Timer example code - This example fires off the callback after 2000ms
    //add_alarm_in_ms(2000, alarm_callback, NULL, false);
    add_alarm_in_us(500000,alarm_callback2,NULL,false);

    puts("Hello, world!");

    return 0;
}
