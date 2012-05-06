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
#define YMPIN_CS		A0

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
#define YMREG_CHAN1_FREQ1	0xa0
#define YMREG_CHAN1_FREQ2	0xa4
#define YMREG_CHAN2_FREQ1	0xa1
#define YMREG_CHAN2_FREQ2	0xa5
#define YMREG_CHAN3_FREQ1	0xa2
#define YMREG_CHAN3_FREQ2	0xa6

#define YMREG_CHAN1_FBACK_ALGO	0xb0
#define YMREG_CHAN2_FBACK_ALGO	0xb1
#define YMREG_CHAN3_FBACK_ALGO	0xb2

#define YMREG_PROPRIETARY1	0x90
#define YMREG_PROPRIETARY2	0x94
#define YMREG_PROPRIETARY3	0x98
#define YMREG_PROPRIETARY4	0x9c

#define YMREG_KEY		0x28
#define YMREG_LFO		0x22
#define YMREG_CH3_TIMERS	0x27

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
	uint8_t		a0;
	uint8_t		a1;
};


void		ym_write(struct ym_2612 *ym);
void		ym_write_reg(uint8_t reg, uint8_t data, uint8_t part);

/*
 * Given a channel return the part (1 or 2) and the channel offset (1-3).
 * Eg.
 * Channel 5 is part 2 offset 1
 */
void
ym_get_chan_part_and_offset(uint8_t chan, uint8_t *part, uint8_t *offs)
{
	if ((chan >= 1) && (chan <= 3)) {
		*part = 1;
	} else if (chan <= 6) {
		*part = 2;
	} else {
		Serial.println("ym_get_chan_part_and_offset: bad channel");
		return;
	}

	*offs = (chan - 1) % 3;
}

/* We are not going to use the so called "proprietary" registers for now */
void
ym_zero_proprietary_regs()
{
	uint8_t			chan, op, part, offs;

	for (op = 1; op < 5; op++) {
		for (chan = 1; chan < 7; chan++) {
			ym_get_chan_part_and_offset(chan, &part, &offs);

			ym_write_reg(YMREG_PROPRIETARY1 + (4 * (op-1)) + offs, 0x00, part);
			ym_write_reg(YMREG_PROPRIETARY2 + (4 * (op-1)) + offs, 0x00, part);
			ym_write_reg(YMREG_PROPRIETARY3 + (4 * (op-1)) + offs, 0x00, part);
			ym_write_reg(YMREG_PROPRIETARY4 + (4 * (op-1)) + offs, 0x00, part);

		}
	}
}

void
setup(void) {
	int			i;

	/* turn on serial debugging */
	Serial.begin(9600);
	Serial.println("Starting up");

	ym_zero_proprietary_regs();

	return;
}

extern "C" void
__cxa_pure_virtual(void)
{
	while(1);
} 

void
ym_all_pins_output()
{
	int			i;

	/* XXX hardcoded */
	for (i = 2; i < 14; i++)
		pinMode(i, OUTPUT);
	pinMode(A0, OUTPUT);
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
	uint8_t		val;

	digitalWrite(YMPIN_CS, YMVAL_CS_OFF);

	//delay(1);

	ym_all_pins_output();
	pinMode(pin, INPUT); /* XXX must be data bus (check) */

	digitalWrite(YMPIN_A0, YMVAL_A0_ON);
	digitalWrite(YMPIN_A1, YMVAL_A1_OFF);
	digitalWrite(YMPIN_CS, YMVAL_CS_ON);
	digitalWrite(YMPIN_WR, YMVAL_WR_OFF);
	digitalWrite(YMPIN_RD, YMVAL_RD_ON);

	//delay(1);
	val = digitalRead(pin);

	digitalWrite(YMPIN_CS, YMVAL_CS_OFF);

	return (val);
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
		delay(1);
	}

	return;
}

/* output to ym2612 */
void
ym_write(struct ym_2612 *ym)
{
	int			i;

	ym_all_pins_output();

#ifdef YM_DEBUG
	ym_debug(ym);
#endif

	digitalWrite(YMPIN_CS, YMVAL_CS_OFF);

	/* data bus */
	for (i = 0; i < 8; i++)
		digitalWrite(YMPIN_D0 + i, ym->data & (1 << i) ? 1 : 0);

	/* other */
	digitalWrite(YMPIN_A0, ym->a0 & 0x1);
	digitalWrite(YMPIN_A1, ym->a1 & 0x1);

	digitalWrite(YMPIN_CS, YMVAL_CS_ON);
	//delay(1);
	digitalWrite(YMPIN_WR, ym->wr & 0x1);
	digitalWrite(YMPIN_RD, ym->rd & 0x1);

	//delay(1);

	digitalWrite(YMPIN_CS, YMVAL_CS_OFF);
	
#ifdef YM_DEBUG
	while (!Serial.available());
	Serial.read();
#endif

	ym_wait_until_ready();

	digitalWrite(YMPIN_A0, YMVAL_A0_OFF);
	digitalWrite(YMPIN_A1, YMVAL_A1_OFF);
	digitalWrite(YMPIN_RD, YMVAL_RD_OFF);
	digitalWrite(YMPIN_RD, YMVAL_WR_OFF);

	//delay(1);

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
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_OFF;
	ym.a1 = part - 1;

	ym_write(&ym);
	//delay(YM_DELAY);

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
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_ON;
	ym.a1 = part - 1;

	ym_write(&ym);
	//delay(YM_DELAY);

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

/* depress a key */
void
ym_set_key(uint8_t chan, uint8_t onoff)
{
	uint8_t			data = onoff ? 0xf0 : 0x00;

	data |= chan - 1;

	ym_write_reg(YMREG_KEY, data, 1);

}


/* set the frequency and octave of a chanel (1-6) */
void
ym_set_chan_octave_and_freq(uint8_t chan, uint8_t octave, uint16_t freq)
{
	uint8_t			data, part, offs;
	uint8_t			reg1, reg2;

	ym_get_chan_part_and_offset(chan, &part, &offs);

	reg1 = YMREG_CHAN1_FREQ1 + offs;
	reg2 = YMREG_CHAN1_FREQ2 + offs;

	/* two writes, MSB first essential */
	data = (octave & 0x7) << 3;
	data |= (freq & 0x0700) >> 8;
	ym_write_reg(reg2, data, part);

	/* LSB */
	data = freq & 0xff;
	ym_write_reg(reg1, data, part);
}

void
ym_set_feedback_and_algo(uint8_t feedback, uint8_t algo)
{
	uint8_t			data = (feedback & 0x7) << 3;

	data |= (algo & 0x7);
	ym_write_reg(0xb0, data, 1); /* XXX hardcoded 1 */

}

void
ym_set_lfo(uint8_t enable, uint8_t freq)
{
	uint8_t			data = (enable & 1) << 3;

	data |= (freq & 0x7);
	ym_write_reg(YMREG_LFO, data, 1);
}

void ym_set_ch3_mode_and_timers(uint8_t ch3, uint8_t reset_b, uint8_t reset_a,
    uint8_t enable_b, uint8_t enable_a, uint8_t load_b, uint8_t load_a)
{
	uint8_t			data = (ch3 & 0x2) << 0x6;

	data |= (reset_b & 0x1) << 5;
	data |= (reset_a & 0x1) << 4;
	data |= (enable_b & 0x1) << 3;
	data |= (enable_a & 0x1) << 2;
	data |= (load_b & 0x1) << 1;
	data |= (load_a & 0x1);

	ym_write_reg(YMREG_CH3_TIMERS, data, 1);
}

void
loop(void) {
	struct ym_2612		ym;
	int			i;

	/*
	 * example program from:
	 * http://www.smspower.org/maxim/Documents/YM2612
	 */

	Serial.write("Sin v");
	Serial.println(VERSION);
	Serial.println("---------------------------");

	Serial.write("Waiting for ym2612 to wake up...");
	Serial.flush();
	ym_wait_until_ready();
	Serial.println("OK!");

	ym_set_lfo(0, 0);		// LFO off
	ym_set_ch3_mode_and_timers(0, 0, 0, 0, 0, 0, 0);

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

	ym_set_feedback_and_algo(0, 1);

	ym_write_reg(0xb4, 0xc0, 1);	// Both channels on
	ym_set_key(1, 0);		// Key off


	int freq = 600;
	int algo = 0;
	while (1) {

		ym_set_chan_octave_and_freq(1, 4, freq);

		while (!Serial.available());
		Serial.read();
		ym_set_key(1, 1);

		freq = freq + 100;
	}

	return ;
}

#ifdef __cplusplus
}
#endif
