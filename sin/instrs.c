/* Global Instrument Defs, to be read from SD card one day */
struct ym_instr ym_instrs[] = 
{
	{
		"sonic2_mystic_cave_0",
		"internal",
		{0, 0, 0, 0, 0},
		{64, 6, 0, 0, 0, 120, 0},
		{
			{24, 1, 1, 15, 15, 52, 3, 5, 0, 0, 0},
			{24, 27, 4, 15, 2, 41, 1, 5, 0, 0, 0},
			{22, 3, 1, 15, 3, 16, 2, 1, 0, 0, 0},
			{20, 8, 1, 15, 3, 22, 2, 1, 6, 0, 0}
		}
	},
	{
		"sonic2_mystic_cave_1",
		"internal",
		{0, 0, 0, 0, 0},
		{64, 7, 2, 0, 0, 120, 0},
		{
			{31, 3, 2, 15, 10, 30, 0, 2, 6, 0, 0},
			{31, 3, 2, 15, 10, 40, 2, 2, 0, 0, 0},
			{31, 10, 2, 15, 7, 0, 0, 1, 4, 0, 0},
			{31, 10, 2, 15, 7, 8, 0, 1, 4, 0, 0},
		}
	},
	{
		"sonic2_mystic_cave2",
		"internal",
		{0, 0, 0, 0, 0},
		{64, 7, 5, 0, 0, 120, 0}, 
		{
			{16, 7, 1, 0, 2, 25, 0, 1, 3, 0, 0},
			{16, 8, 0, 10, 1, 16, 1, 1, 3, 0, 0},
			{16, 8, 0, 10, 1, 16, 1, 1, 3, 0, 0},
			{16, 8, 0, 10, 1, 16, 1, 1, 3, 0, 0}
		}
	},
	{
		"sonic_and_knuckles_flying_battery_bass",
		"internal",
		{0, 0 ,0 ,0, 0},
		{64, 7, 3, 0, 0, 0, 0},
		{
			{0x1f, 0xf, 0xe, 0xf, 0xe, 0x24, 0x2, 7, 4, 0, 0},
			{0x1f, 0x9, 0xe, 0xf, 0xe, 0x15, 0x0, 1, 4, 0, 0},
			{0x1f, 0xe, 0xe, 0xf, 0xe, 0x12, 0x0, 0, 4, 0, 0},
			{0x1f, 0x9, 0xf, 0xf, 0xe, 0x0b, 0x0, 0, 4, 0, 0},
		}
	}
};


#if 0
	/* grand piano :: The demo on the data sheet */

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
