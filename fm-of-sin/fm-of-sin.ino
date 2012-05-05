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

/* no need to be mega efficient (yet) */
struct ym2612 {
	uint8_t		data[8];
	uint8_t		wr;
	uint8_t		rd;
	uint8_t		cs;
	uint8_t		a0;
	uint8_t		a1;
};

void setup(void) {
	int			i;

	for (i = 0; i < 12; i++)
		pinMode(i,  OUTPUT);

	return;
}

void loop(void) {
	/* more of your code here */
	return ;
}

#ifdef __cplusplus
}
#endif
