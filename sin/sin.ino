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

/* Frequencies of notes when clocked 8MHz */
#define YMNOTE_C_LOW		614 // just so as to map to midi better
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

/*
 * MIDI stuff:
 * http://www.midi.org/techspecs/midimessages.php
 * http://arduino.cc/en/Tutorial/Midi
 * http://www.midi.org/techspecs/electrispec.php
 */
#define MIDI_BAUD		31250
#define MIDI_STAT_MASK		0xf0
#define MIDI_PARAM_MASK		0x0f

/* MIDI status bytes */
#define MIDI_STAT_NOTE_ON	0x9
#define MIDI_STAT_NOTE_OFF	0x8

/*
 * Note indices when mapping MIDI notes modulo 12 to ym freqs
 */
unsigned int		note_freqs[] = {
	YMNOTE_C_LOW, YMNOTE_CSH, YMNOTE_D, YMNOTE_DSH, YMNOTE_E, YMNOTE_F,
	YMNOTE_FSH, YMNOTE_G, YMNOTE_GSH, YMNOTE_A, YMNOTE_ASH, YMNOTE_B
};

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

/*
 * Instrument Parameters
 * In MiOPMdrv sound bank Paramer Ver2002.04.22 format
 * (Whatever that is...)
 * But seriously, its a VSTi plugin.
 */

struct ym_chan_params {
	uint8_t		pan;
	uint8_t		fl;
	uint8_t		con;
	uint8_t		ams;
	uint8_t		pms;
	uint8_t		slot;	// ???
	uint8_t		ne;	// ???
};

struct ym_oper_params {
	uint8_t		ar;
	uint8_t		d1r;
	uint8_t		d2r;
	uint8_t		rr;
	uint8_t		d1l;
	uint8_t		tl;
	uint8_t		ks;
	uint8_t		mul;
	uint8_t		dt1;
	uint8_t		dt2; // unused on ym2612?
	uint8_t		ams_en;
};

struct ym_lfo_params { 
	uint8_t		lfrq; 
	uint8_t		amd; 
	uint8_t		pmd; 
	uint8_t		wf; 
	uint8_t		nfrq; 
}; 

#define MAX_INSTR_NAME          64
#define MAX_INSTR_FILENAME      64
struct ym_instr {
	char			name[MAX_INSTR_NAME];
	char			from_filename[MAX_INSTR_FILENAME];
	struct ym_lfo_params	lfo_params;
	struct ym_chan_params	chan_params;
	struct ym_oper_params	opers_params[4];
};

#include <instrs.c> // yuck, can arduino do proper externs?
char		*loaded_instr_names[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

/* ------------------------------------------------------------------
 * Protos
 */

/* low level register read/write */
void		ym_write(struct ym_2612 *);
void		ym_write_reg(uint8_t, uint8_t, uint8_t);

/* parameters */
void		ym_set_chan_octave_and_freq(uint8_t, uint8_t, uint16_t);
void		ym_set_key(uint8_t, uint8_t);

/* midi */
void		midi_parse_key(uint8_t, uint8_t *, uint8_t *);
void		parse_midi_packet(uint8_t);

/* ------------------------------------------------------------------
 * Misc
 */

void
print_loaded_instr(uint8_t x)
{
	if (loaded_instr_names[x-1] == NULL) {
		Serial.println("No Instrument loaded in that channel!");
		return;
	}

	Serial.print("Switch channel: ");
	Serial.println(loaded_instr_names[x - 1]);
}

void
parse_serial_debugging_input(unsigned char c)
{
	static uint8_t		oct = 3;
	static uint8_t		key_chan = 1;

	switch (c) {
	/* Note presses */
	case 'q': /* C# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_CSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'w': /* D# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_DSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'r': /* F# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_FSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 't': /* G# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_GSH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'y': /* A# */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_ASH);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'a': /* D */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_D);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 's': /* E */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_E);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'd': /* F */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_F);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'f': /* G */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_G);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'g': /* A */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_A);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'h': /* B */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_B);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	case 'j': /* C */
		ym_set_chan_octave_and_freq(key_chan, oct, YMNOTE_C);
		ym_set_key(key_chan, 0);
		ym_set_key(key_chan, 1);
		break;
	/* Mute playing notes (all channels) */
	case ' ':
		int i;
		for (i = 1; i < 7; i++)
			ym_set_key(i, 0);
		break;
	/* Switch Channels */
	case '1':
		print_loaded_instr(1);
		key_chan = 1;
		break;
	case '2':
		print_loaded_instr(2);
		key_chan = 2;
		break;
	case '3':
		print_loaded_instr(3);
		key_chan = 3;
		break;
	case '4':
		print_loaded_instr(4);
		key_chan = 4;
		break;
	case '5':
		print_loaded_instr(5);
		key_chan = 5;
		break;
	case '6':
		print_loaded_instr(6);
		key_chan = 6;
		break;
	/* Octave switch */
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

/* ------------------------------------------------------------------
 * Top level modes
 */

void
midi_input_mode()
{
	uint8_t		ch;

	Serial.println("MIDI input mode");

	for (;;) {
		while (!Serial1.available());
		ch = Serial1.read();
		
		parse_midi_packet(ch);
	}
}

void
serial_debugging_mode()
{
	char			char_in;

	Serial.println("Serial debugging mode");

	while (1) {
		while (!Serial.available());
		char_in = Serial.read();

		parse_serial_debugging_input(char_in);

	}
	return;
}

/* ------------------------------------------------------------------
 * YM2612 Routines
 */

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


/*
 * XXX at some point make it so that no junk
 * comes out the serial line if YM_DEBUG is not defined
 */
void
setup(void) {
	int			i;

	/* Turn on MIDI UART */
	Serial1.begin(MIDI_BAUD);

	/* turn on serial debugging */
	Serial.begin(9600);

	/* Fun l33t hax0r banner */
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
	Serial.println("Sin 16-bit Instrument - https://github.com/vext01/sin");
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

/* depress/release a key */
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

void
load_instr(struct ym_instr *i, uint8_t chan)
{
	uint8_t			oper;
	struct ym_oper_params	*op;

	Serial.print("Loading instrument '");
	Serial.print(i->name);
	Serial.print("' into chan '");
	Serial.print(chan);
	Serial.println("'");

	/* XXX figure out what SLOT and NE are */
		
	for (oper = 1; oper < 5; oper++) {
		op = &(i->opers_params[oper-1]);

		ym_set_dt1_mul(chan, oper, op->dt1, op->mul);
		ym_set_tl(chan, oper, op->tl);
		ym_set_rs_ar(chan, oper, op->ks, op->ar);
		ym_set_am_d1r(chan, oper, op->ams_en, op->d1r);
		ym_set_d2r(chan, oper, op->d2r);
		ym_set_d1l_rr(chan, oper, op->d1l, op->rr);
	}

	ym_set_feedback_and_algo(chan,
	    i->chan_params.fl, i->chan_params.con);

	ym_set_lr_ams_fms(chan,
	    1, 1, i->chan_params.ams, i->chan_params.pms);

	loaded_instr_names[chan - 1] = i->name;
}

void
loop(void) {
	struct ym_2612		ym;
	unsigned char		char_in;
	uint8_t			chan, instr;

	Serial.write("Waiting for ym2612 to wake up...");
	Serial.flush();
	ym_wait_until_ready();
	Serial.println("OK!");

	ym_set_ch3_mode_and_timers(0, 0, 0, 0, 0, 0, 0);
	ym_set_dac(0, 0);		// no dac thanks
	ym_set_lfo(0, 0);		// LFO off for now. support later? XXX

	//ym_set_feedback_and_algo(chan, 0x07, 3);
	//ym_set_lr_ams_fms(chan, 1, 1, 0, 0);

	/*
	 * For now we just load the first few instrs from the array
	 * defined in instrs.c
	 */
	for (instr = 0; instr < 4; instr++)
		load_instr(&(ym_instrs[instr]), instr+1);

	/* Ask which mode */
	Serial.println("MIDI or [S]erial debugging mode? [m/s]:");
	while (char_in = Serial.read()) {
		if ((char_in != 'm') && (char_in != 's'))
			continue;

		break;
	}

	if (char_in == 's')
		serial_debugging_mode();
	else
		midi_input_mode();
}

void
midi_cmd_note(uint8_t chan, uint8_t onoff)
{
	uint8_t		oct, note, key, vel;

	while (!Serial1.available());
	key = Serial1.read();
	while (!Serial1.available());
	vel = Serial1.read();

	if (onoff) {
		midi_parse_key(key, &oct, &note);
		//Serial.print("I WILL PLAY FREQ: ");
		//Serial.println(note_freqs[note]);
		ym_set_chan_octave_and_freq(chan + 1, oct, note_freqs[note]);
		ym_set_key(chan + 1, 1);
	} else {
		ym_set_key(chan + 1, 0);
	}
}

/* http://www.phys.unsw.edu.au/jw/notes.html */
void
midi_parse_key(uint8_t kv, uint8_t *octave, uint8_t *note)
{
	kv = (kv & 0x7f);

	/*
	 * 0 is a C-1.
	 * 12 semitones in an octave.
	 * So C0 is 0 + 12
	 */
	*octave = (kv / 12) - 1;
	*note = kv % 12;
}


void
parse_midi_packet(uint8_t ch)
{
	/* split status byte into counterparts */
	uint8_t		cmd = (ch & MIDI_STAT_MASK) >> 4;
	uint8_t		param = (ch & MIDI_PARAM_MASK);

	switch (cmd) {
	case MIDI_STAT_NOTE_ON:
		midi_cmd_note(param, 1);
		break;
	case MIDI_STAT_NOTE_OFF:
		midi_cmd_note(param, 0);
		break;
	default:
		Serial.println("Unknown MIDI status byte");
		Serial.println(ch);
		break;
	};
}

#ifdef __cplusplus
}
#endif
