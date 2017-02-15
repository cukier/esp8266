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

#define BUFF_S	1024

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

uint8_t webPage[] =
		"<head><meta http-equiv=\" \"refresh\" \" content=\" \"3\" \">\"</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
bool esp_recived;

#INT_TIMER0
void isr_timer0() {
	set_timer0(0);
	setup_timer_0(T0_OFF);
	esp_recived = true;

	return;
}

uint8_t esp_find(uint8_t *str) {
	uint8_t ipd[] = "+IPD,";
	uint8_t ret = 0xFF;

	ret = strstr(str, ipd);

	return ret;
}

void esp_webPage(bool input) {
	printf("%s", webPage);
	printf("%u", input);
	printf("</h2>");

	return;
}

void esp_chipSend(uint8_t conId, uint16_t length) {
	printf("AT+CIPSEND=");
	printf("%u", conId);
	printf(", ");
	printf("%lu\n\r", length);

	return;
}

void esp_init(void) {
	printf("AT+RST\r\n");
	delay_ms(2000);
	printf("AT+CWJAP=\"dlinkesp\",\"\"\r\n");
	delay_ms(8000);
	printf("AT+CWMODE=1\r\n");
	delay_ms(1000);
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
//	clear_interrupt(INT_RDA);
	clear_interrupt(INT_TIMER0);
//	enable_interrupts(INT_RDA);
	enable_interrupts(INT_TIMER0);
	enable_interrupts(GLOBAL);

	return;
}

int main(void) {
	uint8_t ch_id;
	uint8_t esp_buff[512];
	uint16_t esp_index;

	ch_id = 0xFF;
	esp_index = 0;
	esp_init();
	delay_ms(1000);
	mcu_init();

	while (TRUE) {

		if (kbhit()) {
			esp_buff[esp_index++] = getc();
			set_timer0(0);
			setup_timer_0(T0_INTERNAL | T0_DIV_8);

			if (esp_index >= 512)
				esp_index = 0;
		}

		if (esp_recived) {
			esp_recived = false;
			ch_id = esp_find(esp_buff);

			if (ch_id != 0xFF) {
				esp_chipSend(ch_id, sizeof(webPage) + 6);
				esp_webPage(input(PIN_B0));
			}
		}
	}

	return 0;
}
