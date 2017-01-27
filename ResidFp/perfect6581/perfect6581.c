/*
 * Copyright (c) 2016 Leandro Nini
 * Copyright (c) 2010,2014 Michael Steil, Brian Silverman, Barry Silverman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include "types.h"
#include "netlist_sim.h"
/* nodes & transistors */
#include "netlist_6581.h"

//#define DEBUG

/************************************************************
 *
 * 6581-specific Interfacing
 *
 ************************************************************/

void setCs(state_t *state, unsigned int val)
{
    setNode(state, cs, val);
}

void setRw(state_t *state, unsigned int val)
{
    setNode(state, rw, val);
}

void
writeData(void *state, uint8_t d)
{
        writeNodes(state, 8, (nodenum_t[]){ D0, D1, D2, D3, D4, D5, D6, D7 }, d);
}

void
writeAddress(void *state, uint8_t d)
{
        writeNodes(state, 5, (nodenum_t[]){ A0, A1, A2, A3, A4 }, d);
}

uint8_t
readData(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ D0, D1, D2, D3, D4, D5, D6, D7 });
}

/************************************************************
 *
 * Internals
 *
 ************************************************************/

 uint8_t
readDataBus(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ db0, db1, db2, db3, db4, db5, db6, db7 });
}

uint8_t
readAddressBus(void *state)
{
        return readNodes(state, 5, (nodenum_t[]){ A0_high, A1_high, A2_high, A3_high, A4_high });
}

//BOOL
//readRW(void *state)
//{
//    return isNodeHigh(state, rw);
//}

uint8_t
readFreq3Low(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ freq3_bit0_fb, freq3_bit1_fb, freq3_bit2_fb, freq3_bit3_fb, freq3_bit4_fb, freq3_bit5_fb, freq3_bit6_fb, freq3_bit7_fb });
}

uint8_t
readFreq3High(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ freq3_bit8_fb, freq3_bit9_fb, freq3_bit10_fb, freq3_bit11_fb, freq3_bit12_fb, freq3_bit13_fb, freq3_bit14_fb, freq3_bit15_fb });
}

uint16_t
readFreq3(void *state)
{
    return (readFreq3High(state) << 8) | readFreq3Low(state);
}

uint8_t
readOsc3Low(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ osc3_bit0, osc3_bit1_1, osc3_bit2, osc3_bit3_1, osc3_bit4, osc3_bit5_1, osc3_bit6, osc3_bit7_1 });
}

uint8_t
readOsc3Mid(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ osc3_bit8, osc3_bit9_1, osc3_bit10, osc3_bit11_1, osc3_bit12, osc3_bit13_1, osc3_bit14, osc3_bit15_1 });
}

uint8_t
readOsc3High(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ osc3_bit16, osc3_bit17_1, osc3_bit18, osc3_bit19_1, osc3_bit20, osc3_bit21_1, osc3_bit22, osc3_bit23_1 });
}

uint32_t
readOsc3(void *state)
{
    return (readOsc3High(state) << 16) | (readOsc3Mid(state) << 8) | readOsc3Low(state);
}

uint8_t
readPw3Low(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ pw3_bit0_fb, pw3_bit1_fb, pw3_bit2_fb, pw3_bit3_fb, pw3_bit4_fb, pw3_bit5_fb, pw3_bit6_fb, pw3_bit7_fb });
}

uint8_t
readPw3High(void *state)
{
    return readNodes(state, 4, (nodenum_t[]){ pw3_bit8_fb, pw3_bit9_fb, pw3_bit10_fb, pw3_bit11_fb });
}

uint16_t
readPw3(void *state)
{
    return (readPw3High(state) << 8) | readPw3Low(state);
}

uint8_t
readTri3Low(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ 0, tri3_bit0_out, tri3_bit1_out, tri3_bit2_out, tri3_bit3_out, tri3_bit4_out, tri3_bit5_out, tri3_bit6_out  });
}

uint8_t
readTri3High(void *state)
{
    return readNodes(state, 4, (nodenum_t[]){ tri3_bit7_out, tri3_bit8_out, tri3_bit9_out, tri3_bit10_out });
}

uint16_t
readTri3(void *state)
{
    return (readTri3High(state) << 8) | readTri3Low(state);
}

uint8_t
readNoi3Low(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ noi3_bit0_out, noi3_bit1_out, noi3_bit2_out, noi3_bit3_out, noi3_bit4_out, noi3_bit5_out, noi3_bit6_out, noi3_bit7_out });
}

uint8_t
readNoi3Mid(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ noi3_bit8_out, noi3_bit9_out, noi3_bit10_out, noi3_bit11_out, noi3_bit12_out, noi3_bit13_out, noi3_bit14_out, noi3_bit15_out });
}

uint8_t
readNoi3High(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ noi3_bit16_out, noi3_bit17_out, noi3_bit18_out, noi3_bit19_out, noi3_bit20_out, noi3_bit21_out, noi3_bit22_out, noi3_bit23_out });
}

uint32_t
readNoi3(void *state)
{
    return (readNoi3High(state) << 16) | (readNoi3Mid(state) << 8) | readNoi3Low(state);
}

uint8_t
readVoi3Low(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ voi3_bit0, voi3_bit1, voi3_bit2, voi3_bit3, voi3_bit4, voi3_bit5, voi3_bit6, voi3_bit7 });
}

uint8_t
readVoi3High(void *state)
{
    return readNodes(state, 4, (nodenum_t[]){ voi3_bit8, voi3_bit9, voi3_bit10, voi3_bit11 });
}

uint16_t
readVoi3(void *state)
{
    return (readVoi3High(state) << 8) | readVoi3Low(state);
}

uint8_t
readCtl3(void *state)
{
    return readNodes(state, 8, (nodenum_t[]){ ctl3_bit0_fb, ctl3_bit1_fb, ctl3_bit2_fb, ctl3_bit3_fb, ctl3_bit4_fb, ctl3_bit5_fb, ctl3_bit6_fb, ctl3_bit7_fb });
}

uint16_t
readWav3(void *state)
{
    return readNodes(state, 12, (nodenum_t[]){ wav3_bit0_out, wav3_bit1_out, wav3_bit2_out, wav3_bit3_out, wav3_bit4_out, wav3_bit5_out, wav3_bit6_out, wav3_bit7_out, wav3_bit8_out, wav3_bit9_out, wav3_bit10_out, wav3_bit11_out });
}

/************************************************************
 *
 * Main Clock Loop
 *
 ************************************************************/

static unsigned int cycle;

void
step(void *state)
{
    BOOL clk = isNodeHigh(state, Phi2);

    /* invert clock */
    setNode(state, Phi2, !clk);
    recalcNodeList(state);

    /* handle memory reads and writes */
    //if (!clk)
    //    handleMemory(state);
#ifdef DEBUG
    //printf("Phi2: %s\n", isNodeHigh(state, Phi2) ? "high" : "low"); //DEBUG
    //printf("clk1: %s\n", isNodeHigh(state, sid_clk1) ? "high" : "low"); //DEBUG
    //printf("clk2: %s\n\n", isNodeHigh(state, sid_clk2) ? "high" : "low"); //DEBUG

    //printf("/res: %s\n", isNodeHigh(state, res) ? "high" : "low"); //DEBUG
    //printf("sid_rst: %s\n\n", isNodeHigh(state, sid_rst) ? "high" : "low"); //DEBUG

    //printf("/read: %s\n", isNodeHigh(state, read) ? "high" : "low"); //DEBUG
    //printf("/write: %s\n", isNodeHigh(state, write) ? "high" : "low"); //DEBUG
    //printf("osc3_bit0_add_s: %s\n\n", isNodeHigh(state, osc3_bit0_add_s) ? "high" : "low"); //DEBUG
    //printf("osc3: %06X\n", readOsc3(state)); //DEBUG
    //printf("pw3: %04X\n", readPw3(state)); //DEBUG
	//printf("pul_out: %s\n\n", isNodeHigh(state, pul_out) ? "high" : "low"); //DEBUG
    //printf("tri3: %04X\n", readTri3(state)); //DEBUG
    //printf("tri3 xor: %s\n\n", isNodeHigh(state, tri3_xor) ? "high" : "low"); //DEBUG
    //if (isNodeHigh(state, noi3_clk_lc))
    //{
    //    printf("noi3: %04X\n", readVoi3(state)); //DEBUG
    //}
	printf("Voice 3: %04X\n", readVoi3(state)); //DEBUG
    printf("\n");
#endif
    cycle++;
}

void *
initAndResetChip()
{
    /* set up data structures for efficient emulation */
    nodenum_t nodes = sizeof(netlist_6581_node_is_pullup)/sizeof(*netlist_6581_node_is_pullup);
    printf("nodes: %d\n", nodes); //DEBUG
    nodenum_t transistors = sizeof(netlist_6581_transdefs)/sizeof(*netlist_6581_transdefs);
    printf("transistors: %d\n", transistors); //DEBUG
    void *state = setupNodesAndTransistors(netlist_6581_transdefs,
                                                netlist_6581_node_is_pullup,
                                                nodes,
                                                transistors,
                                                GND,
                                                Vcc);

    setNode(state, cs, 1);
    setNode(state, res, 0);
    setNode(state, Phi2, 1);

    stabilizeChip(state);

    /* hold RESET for 10 cycles */
    for (int i = 0; i < 20; i++)
        step(state);

    printf("... RESET done ...\n"); //DEBUG
    /* release RESET */
    setNode(state, res, 1);
    recalcNodeList(state);

    cycle = 0;

    return state;
}

/************************************************************
 *
 * Tracing/Debugging
 *
 ************************************************************/

void
chipStatus(void *state)
{
    BOOL clk = isNodeHigh(state, sid_clk1);
    uint16_t a = readAddressBus(state);
    uint8_t d = readDataBus(state);
    BOOL r_w = isNodeHigh(state, rw);

    printf("halfcyc:%d phi0:%d AB:%02X D:%02X RnW:%d CTL3:%02X FREQ3:%04X PW3:%04X Osc3:%06X Noi3:%06X",
                cycle,
                clk,
                a,
                d,
                r_w,
                readCtl3(state),
                readFreq3(state),
                readPw3(state),
                readOsc3(state),
                readNoi3(state));
/*
    if (clk) {
        if (r_w)
        printf(" R$%04X=$%02X", a, memory[a]);
        else
        printf(" W$%04X=$%02X", a, d);
    }
*/
    printf("\n");
}
