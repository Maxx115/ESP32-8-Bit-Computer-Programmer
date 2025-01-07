#ifndef BCPU_RAMPROGRAMS
#define BCPU_RAMPROGRAMS

#include <stdint.h>

/* ---------------- ASSEMBLY ---------------- */
#define     NOP     0x00
#define     LDA     0x10
#define     STA     0x20
#define     ADD     0x30
#define     SUB     0x40
#define     LDI     0x50
#define     JMP     0x60
#define     JC      0x70
#define     JZ      0x80
#define     ADI     0x90
#define     SUI     0xA0
#define     DF      0xB0
#define     RF      0xC0
#define     OUR     0xD0
#define     OUT     0xE0
#define     HLT     0xF0

/* ---------------- RAM CONTENTS ---------------- */
#define RAM_SIZE 16

extern uint8_t *newProg;

extern uint8_t prog_NOP[RAM_SIZE];

extern uint8_t prog_AddUntilOverflow[RAM_SIZE];

extern uint8_t prog_Multiply7By12[RAM_SIZE];

extern uint8_t prog_FibonacciSeries[RAM_SIZE];

#endif /* BCPU_RAMPROGRAMS */