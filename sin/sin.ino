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

#define VERSION		"0.1"
#define COPYRIGHT	"(c) Edd Barrett 2012 - Released under the ISC License"

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

/*
 * YM2612 Registers
 */

/* global ym2612 registers (YMREG_*) */
#define YMREG_LFO		0x22
#define YMREG_TIMER_A1		0x24
#define YMREG_TIMER_A2		0x25
#define YMREG_TIMER_B		0x26
#define YMREG_CH3_TIMERS	0x27
#define YMREG_KEY		0x28
#define YMREG_DAC		0x2a
#define YMREG_DAC_ENABLE	0x2b
#define YMREG_OP_SSG_EG		0x90

/* per channel ym2612 registers (YMREG_CHAN_*) */
#define YMREG_CHAN_FREQ1	0xa0
#define YMREG_CHAN_FREQ2	0xa4
#define YMREG_CHAN_CH3_FREQ1	0xa8
#define YMREG_CHAN_CH3_FREQ2	0xac
#define YMREG_CHAN_FBACK_ALGO	0xb0
#define YMREG_CHAN_LR_AMS_FMS	0xb4

/* per operator um2612 registers (YMREG_OP_*) */
#define YMREG_OP_DT1_MUL	0x30
#define YMREG_OP_TL		0x40
#define YMREG_OP_RS_AR		0x50
#define YMREG_OP_AM_D1R		0x60
#define YMREG_OP_D2R		0x70
#define YMREG_OP_D1L_RR		0x80
#define YMREG_OP_SSG_EG		0x90

/* Serial debugging? */
#define YM_DEBUG		1
uint8_t		debug_enable = 0;

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
 * Eg. Channel 5 is part 2 offset 1
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
		Serial.println(chan);
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
			ym_write_reg(YMREG_OP_SSG_EG + (4 * (op-1)) + offs, 0x00, part);
		}
	}
}

/* Per channel, per operator, set detune and frequency multiple */
void
ym_set_dt1_mul(uint8_t chan, uint8_t op, uint8_t dt1, uint8_t mul)
{
	uint8_t			data, part, offset, reg;

	ym_get_chan_part_and_offset(chan, &part, &offset);

	reg = YMREG_OP_DT1_MUL + ((op - 1) * 4) + offset;

	data = (dt1 & 0x7) << 4;
	data |= mul & 0xf;

	ym_write_reg(reg, data, part);
}

/* Per channel, per operator, AM and 1st decay rate */
void
ym_set_am_d1r(uint8_t chan, uint8_t op, uint8_t am, uint8_t d1r)
{
	uint8_t			data, part, offset, reg;

	ym_get_chan_part_and_offset(chan, &part, &offset);
	reg = YMREG_OP_AM_D1R + ((op - 1) * 4) + offset;

	data = (am & 0x1) << 7;
	data |= (d1r & 0x1f);

	ym_write_reg(reg, data, part);
}

/* Per channel, per operator, 2nd decay rate */
void
ym_set_d2r(uint8_t chan, uint8_t op, uint8_t d2r)
{
	uint8_t			data, part, offset, reg;

	ym_get_chan_part_and_offset(chan, &part, &offset);
	reg = YMREG_OP_D2R + ((op - 1) * 4) + offset;

	data = d2r & 0x1f;

	ym_write_reg(reg, data, part);
}

/* Per channel, per operator, secondary amplitude (D1L) and release rate (RR) */
void
ym_set_d1l_rr(uint8_t chan, uint8_t op, uint8_t d1l, uint8_t rr)
{
	uint8_t			data, part, offset, reg;

	ym_get_chan_part_and_offset(chan, &part, &offset);
	reg = YMREG_OP_D1L_RR + ((op - 1) * 4) + offset;

	data = (d1l & 0xf) << 4;
	data = (rr & 0xf);

	ym_write_reg(reg, data, part);
}

/* Per channel, per operator, rate scaling and attack rate */
void
ym_set_rs_ar(uint8_t chan, uint8_t op, uint8_t rs, uint8_t ar)
{
	uint8_t			data, part, offset, reg;

	ym_get_chan_part_and_offset(chan, &part, &offset);
	reg = YMREG_OP_RS_AR + ((op - 1) * 4) + offset;

	data = (rs & 0x3) << 6;
	data |= (ar & 0x1f);

	ym_write_reg(reg, data, part);
}

/* Per channel, per operator, set total level (TL) */
void
ym_set_tl(uint8_t chan, uint8_t op, uint8_t tl)
{
	uint8_t			data, part, offset, reg;

	ym_get_chan_part_and_offset(chan, &part, &offset);
	reg = YMREG_OP_TL + ((op - 1) * 4) + offset;

	data = tl & 0x7f;
	ym_write_reg(reg, data, part);
}


void
setup(void) {
	int			i;

	/* turn on serial debugging */
	Serial.begin(9600);

	Serial.println("");
	Serial.println("");
	Serial.println(" @@@@@@   @@@  @@@  @@@  ");
	Serial.println("@@@@@@@   @@@  @@@@ @@@  ");
	Serial.println("!@@       @@!  @@!@!@@@  ");
	Serial.println("!@!       !@!  !@!!@!@!  ");
	Serial.println("!!@@!!    !!@  @!@ !!@!  ");
	Serial.println(" !!@!!!   !!!  !@!  !!!  ");
	Serial.println("     !:!  !!:  !!:  !!!  ");
	Serial.println("    !:!   :!:  :!:  !:!  ");
	Serial.println(":::: ::    ::   ::   ::  ");
	Serial.println(":: : :    :    ::    :   ");
	Serial.println("");
	Serial.println("Sin 8-bit Instrument - https://github.com/vext01/sin");
	Serial.println(COPYRIGHT);
	Serial.write("Version: ");
	Serial.println(VERSION);

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

	/* data bus */
	pinMode(YMPIN_D0, OUTPUT);
	pinMode(YMPIN_D1, OUTPUT);
	pinMode(YMPIN_D2, OUTPUT);
	pinMode(YMPIN_D3, OUTPUT);
	pinMode(YMPIN_D4, OUTPUT);
	pinMode(YMPIN_D5, OUTPUT);
	pinMode(YMPIN_D6, OUTPUT);
	pinMode(YMPIN_D7, OUTPUT);

	/* other pins */
	pinMode(YMPIN_WR, OUTPUT);
	pinMode(YMPIN_RD, OUTPUT);
	pinMode(YMPIN_CS, OUTPUT);
	pinMode(YMPIN_A0, OUTPUT);
	pinMode(YMPIN_A1, OUTPUT);
}

#ifdef YM_DEBUG
void
ym_debug(struct ym_2612 *ym)
{
	int			i;

	if (debug_enable == 0)
		return;

	Serial.println("DEBUG -------------------");

	/* data bus */
	Serial.write("Data bus = ");
	Serial.println(ym->data);
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

	while (!Serial.available());
	Serial.read();

	return;
}
#endif

uint8_t
ym_read_pin(uint8_t pin)
{
	uint8_t		val;

	digitalWrite(YMPIN_CS, YMVAL_CS_OFF);

	ym_all_pins_output();
	pinMode(pin, INPUT); /* XXX must be data bus (check) */

	digitalWrite(YMPIN_A0, YMVAL_A0_ON);
	digitalWrite(YMPIN_A1, YMVAL_A1_OFF);
	digitalWrite(YMPIN_CS, YMVAL_CS_ON);
	digitalWrite(YMPIN_WR, YMVAL_WR_OFF);
	digitalWrite(YMPIN_RD, YMVAL_RD_ON);

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
	digitalWrite(YMPIN_WR, ym->wr & 0x1);
	digitalWrite(YMPIN_RD, ym->rd & 0x1);

	digitalWrite(YMPIN_CS, YMVAL_CS_OFF);

	ym_wait_until_ready();

	digitalWrite(YMPIN_A0, YMVAL_A0_OFF);
	digitalWrite(YMPIN_A1, YMVAL_A1_OFF);
	digitalWrite(YMPIN_RD, YMVAL_RD_OFF);
	digitalWrite(YMPIN_RD, YMVAL_WR_OFF);

	return;
}

/* select a register in the ym2612 */
void
ym_set_reg_addr(uint8_t addr, uint8_t part)
{
	struct ym_2612		ym;

#ifdef YM_DEBUG
	if (debug_enable) {
		Serial.write("Set address to ");
		Serial.println((int) addr);
		Serial.write("On part ");
		Serial.println(part);
	}
#endif

	ym.data = addr;
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_OFF;
	ym.a1 = part - 1;

	ym_write(&ym);
}

void
ym_set_reg_data(uint8_t data, uint8_t part)
{
	struct ym_2612		ym;

#ifdef YM_DEBUG
	if (debug_enable) {
		Serial.write("Set data to ");
		Serial.println((int) data);
		Serial.write("On part ");
		Serial.println(part);
	}
#endif

	ym.data = data;
	ym.rd = YMVAL_RD_OFF;
	ym.wr = YMVAL_WR_ON;
	ym.a0 = YMVAL_A0_ON;
	ym.a1 = part - 1;

	ym_write(&ym);
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

	if (chan <= 3)
		data |= (chan - 1);
	else
		data |= chan; /* weird isn't it */

	ym_write_reg(YMREG_KEY, data, 1);
}


/* set the frequency and octave of a chanel (1-6) */
void
ym_set_chan_octave_and_freq(uint8_t chan, uint8_t octave, uint16_t freq)
{
	uint8_t			data, part, offs;
	uint8_t			reg1, reg2;

	ym_get_chan_part_and_offset(chan, &part, &offs);

	reg1 = YMREG_CHAN_FREQ1 + offs;
	reg2 = YMREG_CHAN_FREQ2 + offs;

	/* two writes, MSB first essential */
	data = (octave & 0x7) << 3;
	data |= (freq & 0x0700) >> 8;
	ym_write_reg(reg2, data, part);

	/* LSB */
	data = freq & 0xff;
	ym_write_reg(reg1, data, part);
}

void
ym_set_feedback_and_algo(uint8_t chan, uint8_t feedback, uint8_t algo)
{
	uint8_t			data = (feedback & 0x7) << 3;
	uint8_t			part, offs;

	ym_get_chan_part_and_offset(chan, &part, &offs);

	data |= (algo & 0x7);
	ym_write_reg(YMREG_CHAN_FBACK_ALGO + offs, data, part);

}

void
ym_set_lfo(uint8_t enable, uint8_t freq)
{
	uint8_t			data = (enable & 1) << 3;

	data |= (freq & 0x7);
	ym_write_reg(YMREG_LFO, data, 1);
}

void
ym_set_ch3_mode_and_timers(uint8_t ch3, uint8_t reset_b, uint8_t reset_a,
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
ym_set_dac(uint8_t enable, uint8_t dac)
{
	ym_write_reg(YMREG_DAC, dac, 1);
	ym_write_reg(YMREG_DAC_ENABLE, (enable & 0x2) << 6, 1);
}

void
ym_set_lr_ams_fms(uint8_t ch, uint8_t l, uint8_t r, uint8_t ams, uint8_t fms)
{
	uint8_t			data, part, offs;

	ym_get_chan_part_and_offset(ch, &part, &offs);

	/* two writes, MSB first essential */
	data = (l & 0x1) << 7;
	data |= (r & 0x1) << 6;
	data |= (ams & 0x7) << 3;
	data |= fms & 0x3;

	ym_write_reg(YMREG_CHAN_LR_AMS_FMS + offs, data, part);
}

uint8_t
cycle_key_channel(uint8_t c)
{
	if (++c > 6) /* enable other 3 chans */
		c = 1;
	return (c);
}

/* Frequencies of notes when clocked 8MHz */
#define YMNOTE_CSH		654
#define YMNOTE_D		693
#define YMNOTE_DSH		734
#define YMNOTE_E		778
#define YMNOTE_F		824
#define YMNOTE_FSH		873
#define YMNOTE_G		925
#define YMNOTE_GSH		980
#define YMNOTE_A		1038
#define YMNOTE_ASH		1100
#define YMNOTE_B		1165
#define YMNOTE_C		1235
void
parse_serial_input(unsigned char c)
{
	static uint8_t		oct = 3;
	static uint8_t		key_chan = 1;

	switch (c) {
	/* Note presses */
	case 'q': /* C# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_CSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'w': /* D# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_DSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'r': /* F# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_FSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 't': /* G# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_GSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'y': /* A# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_ASH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'a': /* D */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_D);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 's': /* E */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_E);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'd': /* F */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_F);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'f': /* G */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_G);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'g': /* A */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_A);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'h': /* B */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_B);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	case 'j': /* C */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_C);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		key_chan = cycle_key_channel(key_chan);
		break;
	/* Mute playing note */
	case ' ':
		int i;
		for (i = 1; i < 7; i++)
			ym_set_key(i, 0);
		break;
	/* octave switch */
	case 'z':
		oct = 0;
		break;
	case 'x':
		oct = 1;
		break;
	case 'c':
		oct = 2;
		break;
	case 'v':
		oct = 3;
		break;
	case 'b':
		oct = 4;
		break;
	case 'n':
		oct = 5;
		break;
	case 'm':
		oct = 6;
		break;
	case ',':
		oct = 7;
		break;
	};
}

void
loop(void) {
	struct ym_2612		ym;
	unsigned char		char_in;
	uint8_t			chan;

	Serial.write("Waiting for ym2612 to wake up...");
	Serial.flush();
	ym_wait_until_ready();
	Serial.println("OK!");

	ym_set_ch3_mode_and_timers(0, 0, 0, 0, 0, 0, 0);

	ym_set_dac(0, 0);		// no dac thanks

	ym_set_lfo(0, 0);		// LFO off

#if 0
	/* grand piano */

	for (chan = 1; chan < 7; chan++) {
		/* set channel multiplier and detunes */
		ym_set_dt1_mul(chan, 1, 7, 1);
		ym_set_dt1_mul(chan, 2, 0, 13);
		ym_set_dt1_mul(chan, 3, 3, 3);
		ym_set_dt1_mul(chan, 4, 0, 1);

		ym_set_tl(chan, 1, 0x23);
		ym_set_tl(chan, 2, 0x2d);
		ym_set_tl(chan, 3, 0x26);
		ym_set_tl(chan, 4, 0x00);

		ym_set_rs_ar(chan, 1, 0x02, 0x1f);
		ym_set_rs_ar(chan, 2, 0x02, 0x19);
		ym_set_rs_ar(chan, 3, 0x02, 0x1f);
		ym_set_rs_ar(chan, 4, 0x02, 0x14);

		ym_set_am_d1r(chan, 1, 0, 5);
		ym_set_am_d1r(chan, 2, 0, 5);
		ym_set_am_d1r(chan, 3, 0, 5);
		ym_set_am_d1r(chan, 4, 0, 7);

		ym_set_d2r(chan, 1, 2);
		ym_set_d2r(chan, 2, 2);
		ym_set_d2r(chan, 3, 2);
		ym_set_d2r(chan, 4, 2);

		ym_set_d1l_rr(chan, 1, 1, 1);
		ym_set_d1l_rr(chan, 2, 1, 1);
		ym_set_d1l_rr(chan, 3, 1, 1);
		ym_set_d1l_rr(chan, 4, 0xa, 0x6);

		ym_set_feedback_and_algo(chan, 0, 1);
	}
#endif

#if 0
	/* Evil electric sound */
	for (chan = 1; chan < 7; chan++) {
		/* set channel multiplier and detunes */
		ym_set_dt1_mul(chan, 1, 6, 1);
		ym_set_dt1_mul(chan, 2, 0, 1);
		ym_set_dt1_mul(chan, 3, 0, 1);
		ym_set_dt1_mul(chan, 4, 6, 1);

		ym_set_tl(chan, 1, 0x19);
		ym_set_tl(chan, 2, 0x17);
		ym_set_tl(chan, 3, 0x12);
		ym_set_tl(chan, 4, 0x13);

		ym_set_rs_ar(chan, 1, 0x0, 0x10);
		ym_set_rs_ar(chan, 2, 0x0, 0x10);
		ym_set_rs_ar(chan, 3, 0x0, 0x11);
		ym_set_rs_ar(chan, 4, 0x2, 0x11);

		ym_set_am_d1r(chan, 1, 0, 0x06);
		ym_set_am_d1r(chan, 2, 0, 0x1);
		ym_set_am_d1r(chan, 3, 0, 0x01);
		ym_set_am_d1r(chan, 4, 1, 0x1);

		ym_set_d2r(chan, 1, 0x8);
		ym_set_d2r(chan, 2, 0x9);
		ym_set_d2r(chan, 3, 0x0);
		ym_set_d2r(chan, 4, 0x0);

		ym_set_d1l_rr(chan, 1, 0x08, 0x0f);
		ym_set_d1l_rr(chan, 2, 0x0f, 0x0f);
		ym_set_d1l_rr(chan, 3, 0x0f, 0x0f);
		ym_set_d1l_rr(chan, 4, 0x0f, 0x0f);

		ym_set_feedback_and_algo(chan, 0x06, 0);
	}
#endif

#if 0
	/* Flying battery high pitch noise */

	for (chan = 1; chan < 7; chan++) {
		/* set channel multiplier and detunes */
		ym_set_dt1_mul(chan, 1, 7, 3);
		ym_set_dt1_mul(chan, 2, 1, 2);
		ym_set_dt1_mul(chan, 3, 5, 5);
		ym_set_dt1_mul(chan, 4, 3, 1);

		ym_set_tl(chan, 1, 0x36);
		ym_set_tl(chan, 2, 0x21);
		ym_set_tl(chan, 3, 0x38);
		ym_set_tl(chan, 4, 0x0f);

		ym_set_rs_ar(chan, 1, 0x0, 0x16);
		ym_set_rs_ar(chan, 2, 0x0, 0x14);
		ym_set_rs_ar(chan, 3, 0x0, 0x15);
		ym_set_rs_ar(chan, 4, 0x0, 0x12);

		ym_set_am_d1r(chan, 1, 0, 0xe);
		ym_set_am_d1r(chan, 2, 0, 0x5);
		ym_set_am_d1r(chan, 3, 0, 0x1);
		ym_set_am_d1r(chan, 4, 0, 0x4);

		ym_set_d2r(chan, 1, 0xc);
		ym_set_d2r(chan, 2, 0xc);
		ym_set_d2r(chan, 3, 0x13);
		ym_set_d2r(chan, 4, 0xb);

		ym_set_d1l_rr(chan, 1, 0x0f, 0x0f);
		ym_set_d1l_rr(chan, 2, 0x0d, 0x0f);
		ym_set_d1l_rr(chan, 3, 0x0e, 0x0f);
		ym_set_d1l_rr(chan, 4, 0x08, 0x0f);

		ym_set_feedback_and_algo(chan, 0x07, 0);
	}
#endif

	for (chan = 1; chan < 7; chan++) {
		/* set channel multiplier and detunes */
		ym_set_dt1_mul(chan, 1, 4, 7);
		ym_set_dt1_mul(chan, 2, 4, 1);
		ym_set_dt1_mul(chan, 3, 4, 0);
		ym_set_dt1_mul(chan, 4, 4, 0);

		ym_set_tl(chan, 1, 0x24);
		ym_set_tl(chan, 2, 0x15);
		ym_set_tl(chan, 3, 0x12);
		ym_set_tl(chan, 4, 0x0b);

		ym_set_rs_ar(chan, 1, 0x2, 0x1f);
		ym_set_rs_ar(chan, 2, 0x0, 0x1f);
		ym_set_rs_ar(chan, 3, 0x0, 0x1f);
		ym_set_rs_ar(chan, 4, 0x0, 0x1f);

		ym_set_am_d1r(chan, 1, 0, 0xf);
		ym_set_am_d1r(chan, 2, 0, 0x9);
		ym_set_am_d1r(chan, 3, 0, 0xe);
		ym_set_am_d1r(chan, 4, 0, 0x9);

		ym_set_d2r(chan, 1, 0xe);
		ym_set_d2r(chan, 2, 0xe);
		ym_set_d2r(chan, 3, 0xe);
		ym_set_d2r(chan, 4, 0xf);


		ym_set_d1l_rr(chan, 1, 0x0e, 0x0f);
		ym_set_d1l_rr(chan, 2, 0x0e, 0x0f);
		ym_set_d1l_rr(chan, 3, 0x0e, 0x0f);
		ym_set_d1l_rr(chan, 4, 0x0e, 0x0f);

		ym_set_feedback_and_algo(chan, 0x07, 3);
		ym_set_lr_ams_fms(chan, 1, 1, 0, 0);
	}

	while (1) {

		while (!Serial.available());
		char_in = Serial.read();

		parse_serial_input(char_in);
		//ym_set_key(1, 1);

	}

	return ;
}

#ifdef __cplusplus
}
#endif
