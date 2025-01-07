#include "BCPU_functions.hpp"
#include "self_arduino.hpp"


/* ---------------- DEFINES / CONSTANTs ---------------- */

/* control lines */
#define RST 15 /* high-active */
#define HLT 2 /* high-active */
#define SEL 4 /* low-active */
#define SET 0 /* low-active */
#define SCLK 22 /* low-active */
#define MCLK 23 /* high-active */
#define senseCLK 35

/* adress lines - low-active */
#define A0 17
#define A1 16
#define A2 5
#define A3 18

/* data lines - low-active */
#define D0 13
#define D1 32
#define D2 12
#define D3 14
#define D4 33
#define D5 25
#define D6 26
#define D7 27

/* pin_lines as array */
#define num_pins 16
#define ADR_START 4
#define DATA_START 8
const int pin_lines[num_pins] = {RST, HLT, SEL, SET, A0, A1, A2, A3, D0, D1, D2, D3, D4, D5, D6, D7};
const int pin_setActive[num_pins] = {HIGH, HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

/* delay times */
#define PULSE_DELAY 100
#define ADR_DELAY 100
#define SIGNAL_DELAY 100

/* ---------------- GOLAB VARIABLES ---------------- */
uint8_t flag_programmable = 0;

/* ---------------- FUNCTION SECTION ---------------- */
void initPins()
{
  pinMode(senseCLK, INPUT);
}

void printErrorState()
{
    /* ToDo: report state through ethernet */
  Serial.println("ERROR: Not in programmable state!");
}

void pinsToInput()
{
  for(int i = 0; i < num_pins; i++)
  {
    pinMode(pin_lines[i], INPUT);
  }
}
void pinsToOutput()
{
  for(int i = 0; i < num_pins; i++)      
  {
    pinMode(pin_lines[i], OUTPUT);
    digitalWrite(pin_lines[i], !pin_setActive[i]);
  }
}

int checkProgrammable()
{
  pinsToInput();
  for(int i = 0; i < num_pins - (ADR_START + DATA_START); i++)
  {
    if(digitalRead(pin_lines[i]) == pin_setActive[i])
    {
      return 0;
    }
  }
  return 1;
}

void lockProgramming()
{
  if(checkProgrammable() == 1)
  {
    pinsToOutput();
    digitalWrite(HLT, HIGH);

    vTaskDelay(SIGNAL_DELAY);

    digitalWrite(RST, HIGH);
    vTaskDelay(PULSE_DELAY);
    digitalWrite(RST, LOW);

    vTaskDelay(SIGNAL_DELAY);

    digitalWrite(SEL, LOW);
    
    flag_programmable = 1;
  }
  else
  {
    flag_programmable = 0;
    printErrorState();
  }
}
void releaseProgramming()
{
  flag_programmable = 0;

  digitalWrite(SEL, HIGH);

  digitalWrite(RST, HIGH);
  vTaskDelay(PULSE_DELAY);
  digitalWrite(RST, LOW);
  
  for(int i = ADR_START; i < num_pins; i++) 
  {
    digitalWrite(pin_lines[i], !pin_setActive[i]);
  }

  pinsToInput();

  digitalWrite(HLT, LOW);
}

void programByte(uint8_t adr, uint8_t byteData)
{
  if(flag_programmable == 1)
  {
    for(int i = 0; i < 4; i++)
    {
      vTaskDelay(ADR_DELAY);
      digitalWrite(pin_lines[i+ADR_START], (adr >> i) & 0x01); /* set ADR pins */
    }
    for(int i = 0; i < 8; i++)
    {
      digitalWrite(pin_lines[i+DATA_START], (byteData >> i) & 0x01); /* set DATA pins */
    }
    
    digitalWrite(SET, LOW);
    vTaskDelay(PULSE_DELAY);
    digitalWrite(SET, HIGH);
  }
  else
  {
    printErrorState();
  }
}
void programRAM(const uint8_t* ramData, int dataSize)
{
  if(flag_programmable == 1 && ramData != nullptr && dataSize > 0 && dataSize < 17)
  {
    for(int i = 0; i < 16; i++)
    {
      if(i < dataSize)
      {
        programByte((uint8_t) i, ramData[i]);
      }
      else
      {
        programByte((uint8_t) i, (uint8_t) 0x0);
      }
    }
  }
  else
  {
    printErrorState();
  }
}

void setHLT(uint8_t hltState)
{
  if(hltState == HIGH)
  {
    pinMode(HLT, OUTPUT);
    digitalWrite(HLT, HIGH);
  }
  else if(hltState == LOW)
  {
    digitalWrite(HLT, LOW);
    pinMode(HLT, INPUT);
  }
}

void setMCLK(uint8_t sclkState)
{
  if(sclkState == HIGH)
  {
    pinMode(SCLK, OUTPUT);
    digitalWrite(SCLK, LOW);
  }
  else if(sclkState == LOW)
  {
    digitalWrite(SCLK, HIGH);
    pinMode(SCLK, INPUT);
  }
}

void pulseMCLK()
{
  digitalWrite(MCLK, LOW);
  digitalWrite(MCLK, HIGH);
  vTaskDelay(200);
  digitalWrite(MCLK, LOW);
}

void resetBCPU()
{
  pinMode(RST, OUTPUT);

  if(digitalRead(HLT) == HIGH)
  {
    digitalWrite(RST, HIGH);
    vTaskDelay(PULSE_DELAY);
    digitalWrite(RST, LOW);    
  }
  else
  {
    pinMode(HLT, OUTPUT);

    digitalWrite(HLT, HIGH);
    digitalWrite(RST, HIGH);
    vTaskDelay(PULSE_DELAY);
    digitalWrite(RST, LOW);
    digitalWrite(HLT, LOW);

    pinMode(HLT, INPUT);
  }

  pinMode(RST, INPUT);
}

bool getClkState(void)
{
  return digitalRead(senseCLK);
}