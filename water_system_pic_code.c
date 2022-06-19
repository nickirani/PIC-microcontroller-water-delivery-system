#include <pic.h>
#include <htc.h>
/*
 RC3:RC0-LCD I/O D7:D4
 RC4 LCD E CLOCKING PIN
 RC5 LCD R/S PIN
 NICHOLAS IRANI 14/6/2012
 THIS PROGRAMME ASKS THE USER FOR DESIRED AMOUNT OF WATER
 THEN TURNS ON A RELAY FOR A CORRESPONDING AMOUNT OF TIME.
 ALL DATA IS DISPLAYED USING AN LCD IN 4BIT MODE.
 */
__CONFIG(FOSC_INTOSCIO &WDTE_OFF &PWRTE_ON &MCLRE_OFF &BOREN_OFF &
             CPD_OFF &CP_OFF &IESO_OFF &FCMEN_OFF);
//////////////////////////////////////////////////////////////////////////////////////////////////////
// gloabl variables //
int RESET, i, j, k, n, x, btntap, valvedelay, valvetimer;
long s, r, v, z, y;
//////////////////////////////////////////////////////////////////////////////////////////////////////
// LCD State machine variable //
int LCDstate;
int volumestate;
const int init = 1;     // intilisation of LCD
const int start = 2;    // Displays both NUIM screen and Prompt for input
const int volume = 3;   // Incremented or decrememnted depending on inputs
const int confirm = 4;  // Confirmation of volume required
const int dispense = 5; // Dispense water then return to start
//////////////////////////////////////////////////////////////////////////////////////////////////////
// LCD Screen messages //
const char TopMessage[] = "N.U.I.M. BIOLOGY";       // top line of lcd
const char BotMessage[] = "WATER SYSTEMS";          // bottom line of lcd
const char FirstTopMessage[] = "HOW MANY LITRES?";  // Welcom screen when a button is pressed
const char FirstBotMessage[] = "USE +/- BUTTONS";   // Prompt for user input
const char SecondTopMessage[] = "LITRES:  ";        // Number of litres selected
const char SecondBotMessage[] = "ENTER TO CONFIRM"; // prompt for conifrmation
const char ThirdTopMessage[] = "***DISPENSING***";  // Number of litres selected
const char ThirdBotMessage[] = "*****VOLUME*****";  // prompt for conifrmation
const char FourthTopMessage[] = "DO YOU NEED MORE"; // Additional water may be required
const char FourthBotMessage[] = "AUTO OR MANUAL";   // Auto goes back to start, Manual opens valve while BTN3 pressed.
const char FifthTopMessage[] = "HOLD ENTER TO";     // top line of lcd
const char FifthBotMessage[] = "DISPENSE WATER";    // bottom line of lcd
//////////////////////////////////////////////////////////////////////////////////////////////////////
// LCD Command constants //
const int LCDClear = 0b00000001;
const int LCDHome = 0b00000010;
const int LCDEntryMode = 0b00000110;
const int LCDDispOn = 0b00001100;
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Pin defines //
#define E RC4    // Define LCD control pins here. This is enable
#define RS RC5   // Register select
#define BTN1 RA0 // Button inputs
#define BTN2 RA1
#define BTN3 RA2
#define Relay RA4
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Constant delay value declarations //
const int Valvedealy = 1000;
const int HalfSecond = 40000000; // 0.5s delay
const int Twentyms = 1250;       // 20ms delay
const int Fivems = 300;          // 5ms delay
const int TwoHundredus = 30;
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Sub rountines //
LCDWrite(int LCDData, int RSValue) // Writing to LCD subroutine
{
  PORTC = (LCDData >> 4) & 0x0F; // Get High 4 Bits for Output
  RS = RSValue;
  E = 1;
  E = 0;                  // Toggle the High 4 Bits out
  PORTC = LCDData & 0x0F; // Get High 4 Bits for Output
  RS = RSValue;
  E = 1;
  E = 0; // Toggle the High 4 Bits out
  if ((0 == (LCDData & 0xFC)) && (0 == RSValue))
    n = Fivems;
  else
    n = TwoHundredus;
  for (k = 0; k < n; k++)
    ; // Delay for Character
} // End LCDWrite
Delay(int delayvalue)
{
  int x;
  for (x = 0; x < delayvalue; x++)
    ;
} // End Delay
Message(char *FirstLine, char *SecondLine)
{
  int x;
  for (x = 0; FirstLine[x] != 0; x++)
    LCDWrite(FirstLine[x], 1);
  LCDWrite(0b11000000, 0);
  for (x = 0; SecondLine[x] != 0; x++)
    LCDWrite(SecondLine[x], 1);
} // End Message
//////////////////////////////////////////////////////////////////////////////////////////////////////
main()
{
  PORTC = 0; // clear port C
  PORTA = 0; // clear port A
  CMCON0 = 7;
  ANSEL = 0;
  TRISC = 0;
  TRISA5 = 0;
  TRISA4 = 0;
  TRISA2 = 1;
  TRISA0 = 1; // RA<0:3> inputs RA4/5 output
  RA5 = 1;
  RESET = 0;
  Delay(Twentyms); // Initialise LCD screen
  PORTC = 3;       // Set 4Bit I/F mode for LCD
  E = 1;
  E = 0; // Reset
  Delay(Fivems);
  E = 1;
  E = 0; // Reset
  Delay(Fivems);
  E = 1;
  E = 0; // Reset
  Delay(Fivems);
  Delay(TwoHundredus);
  LCDWrite(0b00101000, 0);   // LCD in 4Bit IF mode
  LCDWrite(LCDClear, 0);     // Clear LCD
  LCDWrite(LCDEntryMode, 0); // Move cursor after each character
  LCDWrite(LCDDispOn, 0);    // Turn on LCD and enable blinking cursor
  while (1 == 1)             // while.1
  {
  begin:
    RESET = RESET + 1;
    if (RESET == 20)
    {
      RA5 = 0;
    }
    LCDWrite(LCDClear, 0);
    LCDWrite(LCDClear, 0);
    Message(&TopMessage, &BotMessage); // Prints out welcome message
    for (i = 0; i < 4; i++)            // Delay 2 Seconds
      Delay(HalfSecond);
    btntap = BTN1 && BTN2 && BTN3;
    if (btntap != 1) // If start
    {
      goto start;
    } // end If start
    LCDWrite(LCDClear, 0);
    LCDWrite(LCDClear, 0);                       // Clear LCD
    Message(&FirstTopMessage, &FirstBotMessage); // Prints out prompt for user input
    for (i = 0; i < 4; i++)
      Delay(HalfSecond); // ClearLCD
    LCDWrite(LCDClear, 0);
    LCDWrite(LCDClear, 0);
    btntap = BTN1 && BTN2 && BTN3;
  start:
    if (btntap != 1) // IF.1 this loop detects a press by the user
    {
      Delay(HalfSecond); // DELAY TO ENSURE INPUT
      if (btntap != 1)   // IF.2
      {
      automatic:
        s = 0;
        i = 0;
        RESET = 0;
        LCDWrite(LCDClear, 0);
        LCDWrite(LCDClear, 0);
        Message(&SecondTopMessage, &SecondBotMessage); // Display user prompt for desired amount of water
        LCDWrite(0b10000111, 0);
        LCDWrite(i + 48, 1);
        Delay(HalfSecond);
        while (s < 150000) // while.2
        {
          if (BTN1 == 0) // IF.3
          {
            Delay(Twentyms); // Debounce switch
            if (BTN1 == 0)   // IF.4 Begin incrementing litres
            {
              i = i + 1;
              s = 0;
              if (i > 20) // if above 20
              {
                i = 20;
              }    // end if above 20
              else // else above 20.
              {
                if (i < 10) // if 9
                {
                  LCDWrite(0b10000111, 0);
                  Delay(HalfSecond);
                  LCDWrite(i + 48, 1);
                  Delay(HalfSecond);
                }                // if 9
                else if (i < 20) // else if 19
                {
                  LCDWrite(0b10000111, 0);
                  Delay(HalfSecond);
                  LCDWrite(1 + 48, 1); // writes the character "1" to this location
                  LCDWrite(0b10001000, 0);
                  LCDWrite((i - 10) + 48, 1);
                  Delay(HalfSecond);
                }                // end else if 19
                else if (i = 20) // else if = 20
                {
                  LCDWrite(0b10000111, 0);
                  Delay(HalfSecond);
                  LCDWrite(2 + 48, 1); // writes the character "1" to this location
                  LCDWrite(0b10001000, 0);
                  LCDWrite((i - 20) + 48, 1);
                  Delay(HalfSecond);
                }             // end else if = 20
              }               // end above 20
            }                 // End IF.4
          }                   // End IF.3
          else if (BTN2 == 0) // else if.3
          {
            Delay(Twentyms); // Debounce switch
            if (BTN2 == 0)   // IF.5 Begin decrementing litres
            {
              i = i - 1;
              s = 0;
              if (i < 0) // if below zero
              {
                i = 0;
              }    // End if below zero
              else // else below 0
              {
                if (i < 10) // if 9
                {
                  LCDWrite(0b10000111, 0);
                  Delay(HalfSecond);
                  LCDWrite(i + 48, 1);
                  LCDWrite(0b10001000, 0);
                  LCDWrite(0b00100000, 1); // writes a blank character to this location
                  Delay(HalfSecond);
                }                // if 9
                else if (i < 20) // else 20
                {
                  LCDWrite(0b10000111, 0);
                  Delay(HalfSecond);
                  LCDWrite(1 + 48, 1);
                  LCDWrite(0b10001000, 0);
                  LCDWrite((i - 10) + 48, 1);
                  Delay(HalfSecond);
                }             // End else if 20
              }               // End else below 0
            }                 // End IF.5
          }                   // End else if.3
          else if (BTN3 == 0) // else if.3.1
          {
            Delay(Twentyms); // Debounce switch
            if (BTN3 == 0)   // IF.5 Begin dispensing litres
            {
              i = i;
              s = 0;
              if (i < 10) // if 9
              {
                LCDWrite(0b10000111, 0);
                Delay(HalfSecond);
                LCDWrite(i + 48, 1);
                LCDWrite(0b10001000, 0);
                LCDWrite(0b00100000, 1);
                Delay(HalfSecond);
              }                // if 9
              else if (i < 20) // else 20
              {
                LCDWrite(0b10000111, 0);
                Delay(HalfSecond);
                LCDWrite(1 + 48, 1);
                LCDWrite(0b10001000, 0);
                LCDWrite((i - 10) + 48, 1);
                Delay(HalfSecond);
              } // End else if 20
              LCDWrite(LCDClear, 0);
              LCDWrite(LCDClear, 0);
              Message(&ThirdTopMessage, &ThirdBotMessage);
              valvedelay = i;
              valvetimer = 0;
              Relay = 1;
              v = 0;
              while (valvetimer < valvedelay) // While.3
              {
                v = 0;
                valvetimer = valvetimer + 1;
                while (v < 420000) // While.4
                {
                  if (BTN1 && BTN2 == 1) // IF interupt
                  {
                    v = v + 1;
                  } // End IF interupt
                  else
                  {
                    Relay = 0;
                    goto automatic;
                  } // end else interupt
                }   // End while.4
              }     // End while.3
              Relay = 0;
              LCDWrite(LCDClear, 0);
              LCDWrite(LCDClear, 0);
              Message(&FourthTopMessage, &FourthBotMessage);
              z = 0;
              while (z < 50000) // While.4loop for waiting for user input
              {
                if (BTN1 == 0) // If.6
                {
                  Delay(Twentyms);
                  if (BTN1 == 0) // End if.7
                  {
                    goto automatic; // Starts automatic delivery of water again
                  }                 // End if.7
                }                   // End If.6
                else if (BTN2 == 0) // else if.6
                {
                  Delay(Twentyms);
                  if (BTN2 == 0) // If.8
                  {
                    Delay(HalfSecond);
                    LCDWrite(LCDClear, 0);
                    LCDWrite(LCDClear, 0);
                    Message(&FifthTopMessage, &FifthBotMessage); // Tells user to hold button 3 to dispense water
                    y = 0;
                    while (y < 50000) // while.6user input delay
                    {
                      y = y + 1;
                      while (BTN3 == 0) // while.5
                      {
                        Relay = 1;
                        y = 0;
                      } // End while.5
                      Relay = 0;
                    } // End While.6
                    Delay(HalfSecond);
                    goto begin;
                  }  // End If.8
                }    // End esle if.6
                else // esle .6
                {
                  z = z + 1;
                } // end else.6
              }   // End While.4
              goto begin;
            }                                                   // End IF.5
          }                                                     // end else if.3.1
          else if ((BTN1 && BTN2 && BTN3 == 1) && (Relay == 0)) // else if.3
          {
            s = s + 1;
          } // End else if.3
        }   // End while.2
      }     // End IF.2
    }       // End IF.1
    else    // else.1 this loop will go back to the start if nothing has been pressed for 5 seconds after inital press.
    {
      goto begin;
    } // End else.1
  }   // End While.1
} // end main
  //////////////////////////////////////////////////////////////////////////////////////////////////////
