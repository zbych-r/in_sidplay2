#ifndef INCLUDED_FROM_NETLIST_SIM_C
#  define state_t void
#endif

extern state_t *initAndResetChip();
extern void step(state_t *state);
extern void chipStatus(state_t *state);

extern void setCs(state_t *state, unsigned int);
extern void setRw(state_t *state, unsigned int);

extern void writeAddress(state_t *state, unsigned char);
extern void writeData(state_t *state, unsigned char);
extern unsigned char readData(state_t *state);

extern unsigned int cycle;
extern unsigned int transistors;

//TODO remove
extern unsigned short readCtl(void *state);
extern unsigned short readWav3(void *state);
