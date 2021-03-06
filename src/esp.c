/*
 * esp.c
 *
 *  Created on: 13 de fev de 2017
 *      Author: cuki
 */

#include <18F25K22.h>

#fuses INTRC, NOPLLEN, PRIMARY, NOIESO, NOPUT, NOBROWNOUT
#fuses NOWDT, NOPBADEN, NOHFOFST, MCLR, NOSTVREN, NOLVP
#fuses NOXINST, NODEBUG, NOPROTECT, NOCPB, NOCPD, NOWRTC
#fuses NOWRTB, NOWRTD, NOEBTR, NOEBTRB

#use delay(internal=16MHz)
#use rs232(uart1, baud=9600)

//#bit CREN = 0xFAB.4
//#bit OERR = 0xFAB.1

//#define INIT_ESP

#ifndef BUFF_S
#define BUFF_S	512
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

uint8_t webPage[] = "<h1><u>ESP8266 - Web Server</u></h1><h2>Porta ";
bool esp_recived;
uint8_t esp_buff[BUFF_S];
uint16_t esp_index;

#INT_RDA
void isr_rda() {

//	if (OERR) {
//		CREN = 0;
//	}

	esp_buff[esp_index++] = getc();
	set_timer0(0);
	setup_timer_0(T0_INTERNAL | T0_DIV_8);

	if (esp_index >= BUFF_S)
		esp_index = 0;
}

#INT_TIMER0
void isr_timer0() {
	set_timer0(0);
	setup_timer_0(T0_OFF);
	esp_recived = true;
	esp_index = 0;

	return;
}

void esp_flush() {
	uint16_t cont;

	esp_index = 0;

	for (cont = 0; cont < BUFF_S; ++cont)
		esp_buff[cont] = 0;

	return;
}

uint8_t esp_find() {
	uint8_t ipd[] = "+IPD,";
	uint8_t *ptr = null, ret = 0;

	ptr = strstr(esp_buff, ipd);

	if (*ptr == '\0')
		return '\0';

	ptr += 5;
	ret = *ptr - 48;

	return ret;
}

void esp_webPage(bool input) {
	printf("%s%u</h2>", webPage, input);

	return;
}

void esp_chipSend(uint8_t conId, uint16_t length) {
	printf("AT+CIPSEND=%u,%lu\r\n", conId, length);

	return;
}

void esp_close(uint8_t conId) {
	printf("AT+CIPCLOSE=%u\r\n", conId);

	return;
}

void esp_init(void) {
	printf("AT+RST\r\n");
	delay_ms(2000);
	printf("AT+CWMODE=1\r\n");
	delay_ms(1000);
	printf("AT+CWJAP=\"dlinkesp\",\"\"\r\n");
	delay_ms(8000);
	printf("AT+CIFSR\r\n");
	delay_ms(1000);
	printf("AT+CIPMUX=1\r\n");
	delay_ms(1000);
	printf("AT+CIPSERVER=1,80\r\n");
	delay_ms(1000);

	return;
}

void mcu_init(void) {
//	esp_index = 0;
	esp_recived = false;
	clear_interrupt(INT_RDA);
	clear_interrupt(INT_TIMER0);
	enable_interrupts(INT_RDA);
	enable_interrupts(INT_TIMER0);
	enable_interrupts(GLOBAL);

	return;
}

int main(void) {
	uint8_t ch_id;

	mcu_init();
	delay_ms(100);
#ifdef INIT_ESP
	esp_init();
	delay_ms(1000);
#endif
	esp_recived = false;
	esp_index = 0;
	ch_id = '\0';

	while (TRUE) {

		if (esp_recived) {
			esp_recived = false;
			ch_id = esp_find();

			if (ch_id != '\0') {
				delay_ms(500);
				esp_chipSend(ch_id, sizeof(webPage) + 5);
				delay_ms(500);
				esp_webPage(input(PIN_B0));
				delay_ms(500);
				esp_close(ch_id);
				delay_ms(1000);
				esp_flush();
				ch_id = '\0';
			}
		}
	}

	return 0;
}
