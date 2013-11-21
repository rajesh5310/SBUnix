/* @name : timer.c
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */
#include <sys/timer.h>
#include <sys/isr.h>
#include <print.h>
#include <ports.h>
#include <process.h>

uint32_t tick = 0;

/*
* This method calculates the ticks and prints the time since boot on the screen
*/
void timer_callback()
{
   tick++;
   if((tick % 100) == 0)
   {
        uint64_t seconds = tick/100;
        uint64_t hours = seconds/(3600);
        uint64_t minutes = (seconds%(3600))/60;
        uint64_t secs = (seconds%(3600))%60;
        print_line(50, 24, "Time Passed : %d:%d:%d ", hours, minutes, secs);
        schedule();
  }

}

/*
* This method is used to initialize the timer for printinng time
*/
void init_timer()
{

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
