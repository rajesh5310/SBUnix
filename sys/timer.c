#include <sys/timer.h>
#include <sys/isr.h>
#include <sys/print.h>
#include <sys/ports.h>
#include <sys/process.h>
#include<defs.h>                                                                                                                               
#include <sys/gdt.h>
#include <sys/pmap.h>

uint32_t tick = 0;
volatile int sleep_time = 0;
void timer_callback()
{
    tick++;

    outb(0x20, 0x20);
    if((tick % 18) == 0)
    {
        uint32_t time_secs, time_mins=0, time_hours=0;
        time_secs = tick/100;

        time_hours = time_secs/3600;

        if(time_secs >= 3600) 
            time_mins = time_secs%3600; 
        else if(time_secs < 3600 && time_secs >= 60) 
            time_mins = time_secs/60;
        else
            time_mins = 0;

        if(time_secs==60) {
            time_secs=0;
        } 
        else if(time_secs > 60) {
            time_secs=time_secs%60;
        }

        print_line(40, 24, "%s" "%d : %d : %d", "Time since boot (hh:mm:ss): ", time_hours, time_mins, time_secs);

    }
    if(sleep_time != 0)
    {
        sleep_time--;
    }


}

void init_timer()
{
    // Firstly, register our timer callback.
    //irq_install_handler(0, &timer_callback);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.

    int frequency = 100;
    uint16_t divisor = 1193180/frequency;
    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)(divisor>>8);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}

void sleep_shell(int secs)
{
    sleep_time = secs*100;
    while(sleep_time != 0);

    //print("\n%d sec passed", secs); 
}
