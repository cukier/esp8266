/*
 * esp.c
 *
 *  Created on: 13 de fev de 2017
 *      Author: cuki
 */

#include <18F25K22.h>

#fuses INTRC, PLLEN, PRIMARY, NOIESO, NOPUT, NOBROWNOUT
#fuses NOWDT, NOPBADEN, NOHFOFST, MCLR, NOSTVREN, NOLVP
#fuses NOXINST, NODEBUG, NOPROTECT, NOCPB, NOCPD, NOWRTC
#fuses NOWRTB, NOWRTD, NOEBTR, NOEBTRB

#use delay(internal=64MHz)
#use rs232(uart1, baud=115200)

#include <stdint.h>
#include <stdbool.h>

#define BUFF_S	512

uint8_t s[BUFF_S];
uint16_t b_index;
bool ok = false;

#INT_RDA
void isr_rda() {
	setup_timer_0(T0_INTERNAL | T0_DIV_256);
	set_timer0(0);

	s[b_index] = getc();
	b_index++;

	if (b_index >= BUFF_S)
		b_index = 0;

	clear_interrupt(INT_RDA);
}

#INT_TIMER0
void isr_timer() {
	setup_timer_0(T0_OFF);
	ok = true;
}

int main(void) {
	uint16_t cont;

	b_index = 0;

	setup_timer_0(T0_INTERNAL | T0_DIV_256);
	set_timer0(0);

	enable_interrupts(INT_RDA);
	enable_interrupts(INT_TIMER0);
	enable_interrupts(GLOBAL);

	ok = false;
	printf("AT+RST\r\n");

	while (!ok)
		;

	return 0;
}
