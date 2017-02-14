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
#include <string.h>

uint8_t webPage[] =
		"<head><meta http-equiv=\" \"refresh\" \" content=\" \"3\" \">\"</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";

uint8_t esp_find(uint8_t *esp_buffer) {
	uint8_t ipd[] = "+IPD,";
	uint8_t ret = 0xFF;

	ret = strcmp(ipd, esp_buffer);

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
	printf(",");
	printf("%lu\n\r", length);

	return;
}

void esp_init(void) {
	printf("AT+RST\r\n");
	delay_ms(2000);
	printf("AT+CWJAP=\"dlink\",\"\"\r\n");
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

int main(void) {
	uint8_t ch_id;
	uint8_t str[] = "";
//	uint8_t timeout;

	ch_id = 0xFF;
	esp_init();
	delay_ms(300);

	while (TRUE) {
//		timeout = 0xFF;
		if (kbhit()) {
			gets(str);
			ch_id = esp_find(str);
			if (ch_id != 0xFF) {
				esp_chipSend(ch_id, sizeof(webPage) + 6);
				esp_webPage(input(PIN_B0));
			}
		}
	}

	return 0;
}
