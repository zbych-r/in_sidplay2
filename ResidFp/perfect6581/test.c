#include "perfect6581.h"

#include <stdio.h>

void
writeReg(void *state, unsigned char addr, unsigned char data)
{
	setCs(state, 0);
	setRw(state, 0);
	writeAddress(state, addr);
	writeData(state, data);

	step(state);
	step(state);
	setCs(state, 1);
}

unsigned char
readReg(void *state, unsigned char addr)
{
	setCs(state, 0);
	setRw(state, 1);
	writeAddress(state, 0x1B);

	step(state);
	unsigned char data = readData(state);
	step(state);
	setCs(state, 1);
	return data;
}

int
main()
{
	//int clk = 0;

	void *state = initAndResetChip();

	chipStatus(state);

	// set pw3 hi
	writeReg(state, 0x11, 0xA5);

	// set pw3 lo
	writeReg(state, 0x10, 0x57);

	// set freq3 lo
	writeReg(state, 0x0E, 0x00);

	// set freq3 hi
	writeReg(state, 0x0F, 0x10);

	// Set gate on
	writeReg(state, 0x12, 0x11);

    for (int i=0; i<0xfff; i++)
	{
		step(state);
		step(state);
		printf("*** Wave 3: %03X\n", readWav3(state));
	}

	// read from OSC3
	printf("*** OSC3: %02X\n\n", readReg(state, 0x1B));

	step(state);
	step(state);

	step(state);
	step(state);

	// Set test on
	writeReg(state, 0x12, 0x08);

	step(state);
	step(state);

	chipStatus(state);

	// Set test off
	writeReg(state, 0x12, 0x00);

	step(state);
	step(state);

	chipStatus(state);

#if 0
	/* emulate the 6581! */
	for (;;) {
		step(state);
		clk = !clk;
//		if (clk)
//			handle_monitor(state);

		chipStatus(state);
		//if (!(cycle % 1000)) printf("%d\n", cycle);
	};
#endif
}
