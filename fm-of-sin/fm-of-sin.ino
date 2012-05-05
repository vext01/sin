/*
 * Copyright (c) 2012, Edd Barrett <vext01@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION			"0.1"

/* Pin Mapping */
#define YMPIN_D0		2
#define YMPIN_D1		3
#define YMPIN_D2		4
#define YMPIN_D3		5
#define YMPIN_D4		6
#define YMPIN_D5		7
#define YMPIN_D6		8
#define YMPIN_D7		9

#define YMPIN_WR		10
#define YMPIN_RD		11
#define YMPIN_A0		12
#define YMPIN_A1		13

/* Some pins are active low, others high */
#define YMVAL_WR_ON		0
#define YMVAL_WR_OFF		1

#define YMVAL_RD_ON		0
#define YMVAL_RD_OFF		1

#define YMVAL_CS_ON		0
#define YMVAL_CS_OFF		1

#define YMVAL_A0_ON		1
#define YMVAL_A0_OFF		0

#define YMVAL_A1_ON		1
#define YMVAL_A1_OFF		0

/* ym2612 registers */
/* define them here? XXX */

/* Delay between raising A* high then low (ms) */
/* XXX this is a guess for now */
#define YM_DELAY		1

/* Serial debugging? */
//#define YM_DEBUG		1

/* no need to be mega efficient (yet) */
struct ym_2612 {
	uint8_t		data;
	uint8_t		wr;
	uint8_t		rd;
	uint8_t		cs;
	uint8_t		a0;
	uint8_t		a1;
};

void
setup(void) {
	int			i;

	/* turn on serial debugging */
	Serial.begin(9600);
	Serial.println("Starting up");

	return;
}

extern "C" void
__cxa_pure_virtual(void)
{
	while(1);
} 

#ifdef YM_DEBUG 
void
ym_debug(struct ym_2612 *ym)
{
	int			i;

	Serial.println("DEBUG -------------------");

	/* data bus */
	for (i = 0; i < 8; i++) {
		Serial.write("YMPIN_D");
		Serial.write(i);
		Serial.write(" = ");
		Serial.println((ym->data & (1 << i)) ? 1 : 0);
	}

	Serial.write("YMPIN_WR = ");
	Serial.println(ym->wr);

	Serial.write("YMPIN_RD = ");
	Serial.println(ym->rd);

	Serial.write("YMPIN_A0 = ");
	Serial.println(ym->a0);

	Serial.write("YMPIN_A1 = ");
	Serial.println(ym->a1);

	return;
}
#endif

uint8_t
ym_read_pin(uint8_t pin)
{
	pinMode(pin, INPUT);

	digitalWrite(YMPIN_RD, YMVAL_RD_ON);
	digitalWrite(YMPIN_WR, YMVAL_WR_OFF);
	digitalWrite(YMPIN_A0, YMVAL_A0_ON);
	digitalWrite(YMPIN_A1, YMVAL_A1_OFF);

	return digitalRead(pin);
}

uint8_t
ym_is_busy()
{
	return ym_read_pin(YMPIN_D7);
}

void
ym_wait_until_ready()
{
	while (ym_is_busy()) {
		Serial.write(".");
		Serial.flush();
		delay(10);
	}

	return;
}

/* output to ym2612 */
void
ym_write(struct ym_2612 *ym)
{
	int			i;

	/* al pins are output */
	for (i = 2; i < 14; i++)
		pinMode(i, OUTPUT);

#ifdef YM_DEBUG
	ym_debug(ym);
#endif

	/* data bus */
	for (i = 0; i < 8; i++)
		digitalWrite(YMPIN_D0 + i, ym->data & (1 << i) ? 1 : 0);

	/* other */
	digitalWrite(YMPIN_WR, ym->wr & 0x1);
	digitalWrite(YMPIN_RD, ym->rd & 0x1);
	digitalWrite(YMPIN_A0, ym->a0 & 0x1);
	digitalWrite(YMPIN_A1, ym->a1 & 0x1);

	delay(10);
	
#ifdef YM_DEBUG
	while (!Serial.available());
	Serial.read();
#endif

	ym_wait_until_ready();

	digitalWrite(YMPIN_A0, 1);
	digitalWrite(YMPIN_A1, 1);

	return;
}


/* select a register in the ym2612 */
void
ym_set_reg_addr(uint8_t addr, uint8_t part)
{
	struct ym_2612		ym;

#ifdef YM_DEBUG
	Serial.write("Set address to ");
	Serial.println((int) addr);
	Serial.write("On part ");
	Serial.println(part);
#endif

	ym.data = addr;
	ym.cs = YMVAL_CS_ON;
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_OFF;
	ym.a1 = part - 1;

	ym_write(&ym);
	delay(YM_DELAY);

	//ym.wr = YMVAL_WR_OFF;
	//ym_write(&ym);
	//delay(YM_DELAY);
}

void
ym_set_reg_data(uint8_t data, uint8_t part)
{
	struct ym_2612		ym;

#ifdef YM_DEBUG
	Serial.write("Set data to ");
	Serial.println((int) data);
	Serial.write("On part ");
	Serial.println(part);
#endif

	ym.data = data;
	ym.cs = YMVAL_CS_ON;
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_ON;
	ym.a1 = part - 1;

	ym_write(&ym);
	delay(YM_DELAY);

	//ym.wr = YMVAL_WR_OFF;
	//ym_write(&ym);
	//delay(YM_DELAY);
}

void
ym_write_reg(uint8_t reg, uint8_t data, uint8_t part)
{
	ym_set_reg_addr(reg, part);
	ym_set_reg_data(data, part);
}

void
loop(void) {
	struct ym_2612		ym;
	int			i;

	/*
	 * example program from:
	 * http://www.smspower.org/maxim/Documents/YM2612
	 */

	Serial.write("FM of Sin v");
	Serial.println(VERSION);
	Serial.println("---------------------------");

	Serial.write("Waiting for ym2612 to wake up...");
	Serial.flush();
	ym_wait_until_ready();
	Serial.println("OK!");

	ym_write_reg(0x22, 0, 1);	// LFO off
	ym_write_reg(0x27, 0, 1);	// Channel 3 mode normal

	for (i = 0; i < 7; i ++)	// All channels off
		ym_write_reg(0x28, i, 1);

	ym_write_reg(0x2b, 0, 1);	// DAC off

	ym_write_reg(0x30, 0x71, 1);	// DT1/MUL
	ym_write_reg(0x34, 0x0d, 1);	// "
	ym_write_reg(0x38, 0x33, 1);	// "
	ym_write_reg(0x3c, 0x01, 1);	// "

	ym_write_reg(0x40, 0x23, 1);	// Total level
	ym_write_reg(0x44, 0x2d, 1);	// "
	ym_write_reg(0x48, 0x26, 1);	// "
	ym_write_reg(0x4c, 0x00, 1);	// "

	ym_write_reg(0x50, 0x5f, 1);	// RS/AR
	ym_write_reg(0x54, 0x99, 1);	// "
	ym_write_reg(0x58, 0x5f, 1);	// "
	ym_write_reg(0x5c, 0x94, 1);	// "

	ym_write_reg(0x60, 0x05, 1);	// AM/D1R
	ym_write_reg(0x64, 0x05, 1);	// "
	ym_write_reg(0x68, 0x05, 1);	// "
	ym_write_reg(0x6c, 0x07, 1);	// "

	ym_write_reg(0x70, 0x02, 1);	// D2R
	ym_write_reg(0x74, 0x02, 1);	// "
	ym_write_reg(0x78, 0x02, 1);	// "
	ym_write_reg(0x7C, 0x02, 1);	// "

	ym_write_reg(0x80, 0x11, 1);	// D1L/RR
	ym_write_reg(0x84, 0x11, 1);	// "
	ym_write_reg(0x88, 0x11, 1);	// "
	ym_write_reg(0x8c, 0xa6, 1);	// "

	ym_write_reg(0x90, 0x00, 1);	// Proprietary shit
	ym_write_reg(0x94, 0x00, 1);	// "
	ym_write_reg(0x98, 0x00, 1);	// "
	ym_write_reg(0x9c, 0x00, 1);	// "

	ym_write_reg(0xb0, 0x32, 1);	// Feedback/Algo
	ym_write_reg(0xb4, 0xc0, 1);	// Both channels on
	ym_write_reg(0x28, 0x00, 1);	// Key off

	ym_write_reg(0xa4, 0x22, 1);	// Set frequency
	ym_write_reg(0xa0, 0x69, 1);	// "

	while (1) {
		ym_write_reg(0x28, 0xf0, 1);	// Key on
		Serial.println("Key on");
		delay(1000);
		ym_write_reg(0x28, 0x00, 1);	// Key off
		Serial.println("Key off");
		delay(1000);
	}

	return ;
}

#ifdef __cplusplus
}
#endif
