#include "BCPU_ramPrograms.hpp"

uint8_t *newProg = nullptr;

uint8_t prog_NOP[RAM_SIZE] = 
{
 NOP | 0x00,
 NOP | 0x01,
 NOP | 0x02,
 NOP | 0x03,
 NOP | 0x04,
 NOP | 0x05,
 NOP | 0x06, 
 NOP | 0x07, 
 NOP | 0x08, 
 NOP | 0x09, 
 NOP | 0x0a, 
 NOP | 0x0b, 
 NOP | 0x0c, 
 NOP | 0x0d, 
 NOP | 0x0e, 
 NOP | 0x0f
};

uint8_t prog_AddUntilOverflow[RAM_SIZE] = 
{
 ADD | 0x05,
 OUT | 0x00,
 JC  | 0x04,
 JMP | 0x00,
 HLT | 0x00,
 0x00 | 0x01,
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00, 
 NOP | 0x00
};

uint8_t prog_Multiply7By12[RAM_SIZE] = 
{
 LDA | 0x0D,
 ADI | 0x0C,
 STA | 0x0D,
 LDA | 0x0C,
 ADI | 0x01,
 STA | 0x0C,
 SUI | 0x07, 
 JZ  | 0x09, 
 JMP | 0x00, 
 LDA | 0x0D, 
 OUT | 0x00, 
 HLT | 0x00, 
 0x00 | 0x00, 
 0x00 | 0x00, 
 NOP | 0x00, 
 NOP | 0x00
};

uint8_t prog_FibonacciSeries[RAM_SIZE] = 
{
 LDI | 0x00,
 STA | 0x0E,
 LDI | 0x01,
 STA | 0x0F,
 LDA | 0x0F,
 OUR | 0x0F,
 ADD | 0x0E, 
 JC  | 0x00, 
 STA | 0x0E, 
 OUR | 0x0E, 
 ADD | 0x0F, 
 JC  | 0x00, 
 STA | 0x0F, 
 JMP | 0x05, 
 0x00 | 0x00, 
 0x00 | 0x00
};