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

/* Pin Mapping */
#define YMPIN_D0		0
#define YMPIN_D1		1
#define YMPIN_D2		2
#define YMPIN_D3		3
#define YMPIN_D4		4
#define YMPIN_D5		5
#define YMPIN_D6		6
#define YMPIN_D7		7

#define YMPIN_WR		8
#define YMPIN_RD		9
#define YMPIN_CS		11
#define YMPIN_A0		10
#define YMPIN_A1		12

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
#define YM_DELAY		100

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

	for (i = 0; i < 12; i++)
		pinMode(i,  OUTPUT);

	return;
}

/* output to ym2612 */
void
ym_do(struct ym_2612 *ym)
{
	int			i;

	/* data bus */
	for (i = 0; i < 8; i++)
		digitalWrite(YMPIN_D0, ym->data & (1 << i));

	/* other */
	digitalWrite(YMPIN_WR, ym->wr & 0x1);
	digitalWrite(YMPIN_RD, ym->rd & 0x1);
	digitalWrite(YMPIN_CS, ym->cs & 0x1);
	digitalWrite(YMPIN_A0, ym->a0 & 0x1);
	digitalWrite(YMPIN_A1, ym->a1 & 0x1);

	return;
}

/* select a register in the ym2612 */
void
ym_set_reg_addr(uint8_t addr, uint8_t part)
{
	struct ym_2612		ym;

	ym.data = addr;
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_OFF;
	ym.a1 = part - 1;

	ym_do(&ym);
	delay(YM_DELAY);

	ym.wr = YMVAL_WR_OFF;
	ym_do(&ym);
	delay(YM_DELAY);
}

void
ym_set_reg_data(uint8_t data, uint8_t part)
{
	struct ym_2612		ym;

	ym.data = data;
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_ON;
	ym.a1 = part - 1;

	ym_do(&ym);
	delay(YM_DELAY);

	ym.wr = YMVAL_WR_OFF;
	ym_do(&ym);
	delay(YM_DELAY);
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

	ym_write_reg(0x28, 0xf0, 1);	// Key off
	delay(1000);
	ym_write_reg(0x28, 0x00, 1);	// Key off
	delay(1000);


	return ;
}

#ifdef __cplusplus
}
#endif
