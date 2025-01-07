#ifndef BCPU_FUNCTIONS
#define BCPU_FUNCTIONS

#include <stdint.h>
#include "BCPU_ramPrograms.hpp"

/* ---------------- FUNCTION DECLARATION SECTION ---------------- */
void initPins();

void printErrorState();

void pinsToInput();
void pinsToOutput();

int checkProgrammable();

void lockProgramming();
void releaseProgramming();

void programByte(uint8_t adr, uint8_t byteData);
void programRAM(const uint8_t* ramData, int dataSize);

/* control functions */
void setHLT(uint8_t hltState);
void setMCLK(uint8_t sclkState);
void pulseMCLK();
void resetBCPU();

bool getClkState();

#endif /* BCPU_FUNCTIONS */