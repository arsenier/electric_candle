#include <Arduino.h>
/*  
* Stardweeny Source Code
* Author: Travis Geis
* Version: 1
* Date: January 2012
* URL: http://zenlogic.org/
*
* file: main.c
*
* The Stardweeny project runs five channels of software PWM to
* control the five individual LEDs in the points of a paper star.
*
* Inspired by and dedicated to Linda Geis.
*
* For this code to work correctly, lfuse = 0x7A. (Disabling CKDIV8)
* Run the command "avrdude -c usbtiny -p t13 -U lfuse:w:0x7A:m"
*/

#define F_CPU 9.6E6L /* CPU Freq. Must come before delay.h include. 9.6MHz */

#include <avr/io.h>             // For data types and I/O operations
#include <avr/interrupt.h>      // Register and port definitions
#include <util/delay.h>        // Busy-wait delay functions
// #include <avr/timer.h>         // Exposes timers, counters and ISR functions


#define CH1_PIN PB0 /* Bind output channels to specific PortB pins. This depends on schematic. */
#define CH2_PIN PB1
#define CH3_PIN PB2
#define CH4_PIN PB3
#define CH5_PIN PB4

#define TIME_OFFSET 50
#define DELAY_MS 1

#define PWM_MAX_VAL 16
#define OCR_VAL 64

uint8_t ch1; /* The one-byte PWM level of each channel */
uint8_t directions = 0xFF;
/*
This one-byte flag holds five channel direction bit flags
0x [nothing] [nothing] [nothing] [ch5] [ch4] [ch3] [ch2] [ch1]
*/

volatile uint8_t ISRcounter = 0; /* Count the number of times the ISR has run */
uint8_t timeCount;

int main(void);

/*
* int main(void):
*
* The main function runs automatically when the AVR powers up.
* It never returns, and so dispatches all other actions for the
* microcontroller.
*
* The goal is to make a sinusoidal light intensity with time.
*
*/

int main(void)
{
	/* Setup */
	ch1 = 0;

	DDRB = 0xFF; //Every PORTB pin is output
	PORTB = 0x00; //Start with every pin low

	TCCR0B |= (1 << CS00); // disable timer prescale (=clock rate)
	// TIMSK0 |= (1 << TOIE0); // enable timer overflow interrupt specifically

  TCCR0A |= (1 << WGM01);
  OCR0A = OCR_VAL;
  TIMSK0 |= (1 << OCIE0A);

  sei(); // enable interrupts in general
  /* Loop */
  while(1)
  {
    ch1 = random(PWM_MAX_VAL / 2) + PWM_MAX_VAL / 2;
    // ch1 %= PWM_MAX_VAL;
		_delay_ms(DELAY_MS);
	}
}
/*
* The ISR is responsible for toggling the states of the five output channels
* based on the current global variables for the desired brightness levels.
* Because the brightnesses (ch1 ... ch5) are single-byte values, the range
* of values is 0 to 255\. Thus the ISR needs to restart its counting cycle every
* 256th time it is called.
*
* To avoid undesired wiggle on the PWM, the ISR updates evey channel's pin
* every time it runs. It should take the same number of cycles every time.
* The duty period of the PWM is the first section. Thus the LED goes ON then OFF.
*/
ISR(TIM0_COMPA_vect)
{
	if(ISRcounter < ch1) PORTB = 0xFF; //Write the ch1 pin high
	else PORTB = 0; //Write the ch1 pin low

	ISRcounter++;
  ISRcounter %= PWM_MAX_VAL;
}