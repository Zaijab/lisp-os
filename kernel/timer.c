
#include "system.h"

/* This will keep track of how many ticks that the system
 *  has been running for */
int timer_ticks = 0;

/* set the clock frequency */
void timer_phase(int hz)
{
	int divisor = 1193180 / hz;       /* Calculate our divisor */
	outportb(0x43, 0x36);             /* Set our command byte 0x36 */
	outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* Handles the timer.*/
void timer_handler(struct regs *r)
{
	/* Increment our 'tick count' */
	timer_ticks++;

	/* Every 18 clocks (approximately 1 second), we will
	 *  display a message on the screen */
	if (timer_ticks % 18 == 0) {
		/* puts("One second has passed\n"); */
	}
}

/* Sets up the system clock by installing the timer handler
 *  into IRQ0 */
void timer_install()
{
	/* Installs 'timer_handler' to IRQ0 */
	irq_install_handler(0, timer_handler);
}

/* This will continuously loop until the given time has
 *  been reached */
void timer_wait(int ticks)
{
	unsigned long eticks;

	eticks = timer_ticks + ticks;
	while(timer_ticks < eticks);
}
