/* Controlling TV using IR NEC protocol */

//- Header Includes -----------------------------------------------------------
#include "tv_sound_ctrl.h"

//- Global Variables  ---------------------------------------------------------
uint32 IncreaseVolume_u32 = 0xE51ABF40; // Send the msg, LSB first
uint32 DecreaseVolume_u32 = 0xE11EBF40;

//- Global Functions  ---------------------------------------------------------

/*- ---------------------------------------------------------------------------
* Function: StartComm
* -------------------
* Starts the IR communication by initiating the following -
*   - a nine-millisecond leading pulse burst.
*   - a 4.5 ms low period.
*
* Parameters: None
* Returns: None
*--------------------------------------------------------------------------- */
static inline void StartComm(void)
{
  tone(IR_LED, CARRIER_FREQ);
  delay(LEAD_BURST_DUR_MS);
  noTone(IR_LED);
  digitalWrite(IR_LED, LOW);
  delayMicroseconds(LEAD_LOW_PULSE_US);
}

/*- ---------------------------------------------------------------------------
* Function: StopComm
* -------------------
* Stops the communication and marks the end of message by -
*   - sending a final 563uS (562.5) of burst.
* At the end of this function IR_LED is set to 'HIGH'.
*
* Parameters: None
* Returns: None
*--------------------------------------------------------------------------- */
static inline void StopComm(void)
{
  tone(IR_LED, CARRIER_FREQ);
  delayMicroseconds(260); // Simply reusing the macro for 563uS
  noTone(IR_LED);
  digitalWrite(IR_LED, HIGH);
}

/*- ---------------------------------------------------------------------------
* Function: TransmitData
* ----------------------
* Transmits the data to Tv.
*
* Parameters:
*   Cmd (Message_Type*): Ptr to the message to be transmitted.
*
* Returns: None
*--------------------------------------------------------------------------- */
void TransmitData(Message_Type* Cmd)
{
  StartComm();
  for(uint8 bit_pos = 0u; bit_pos < 32; bit_pos++)
  {
      tone(IR_LED, CARRIER_FREQ);
      delayMicroseconds(270u); // edit here 562.2uS---------------------------
      noTone(IR_LED);
      digitalWrite(IR_LED, LOW);    
    
    if(bitRead(*(Message_Type*)Cmd, bit_pos))
    {
      //- Transmit '1'
      delayMicroseconds(LOG_1_TX_LOW_US);
    }
    else
    {
      //- Transmit '0'
      delayMicroseconds(LOG_0_TX_LOW_US);
    }
  }
  StopComm();

  for(uint8 idx = 0; idx < 4u; idx++)  /*Prepare Repeat signal: 4 times*/
  {
    delay(REPEAT_SIG_DELAY_MS);
    
    tone(IR_LED, CARRIER_FREQ);
    delay(LEAD_BURST_DUR_MS);
    noTone(IR_LED);
    
    digitalWrite(IR_LED, LOW);
    delayMicroseconds(REPEAT_SIG_SPACE_US);

    tone(IR_LED, CARRIER_FREQ);
    delayMicroseconds(270u); // edit here 560uS-----------------------------
    noTone(IR_LED);
    digitalWrite(IR_LED, HIGH);
  }
}

/* ----------------------------------------------------------------------------
* Function: ControlTv
* -------------------
* Increments or Decrements the Tv sound.
*
* Parameters:
*   Action (Volume_Ctrl_Type): Increment (or) Decrement the TV volume.
*
* Returns: None
*
* Note:
* ----
* There could be a small delay before starting the transmission of message -
* because the Start, Transmit, Stop are handled in different function calls.
* However, this small delay should not introduce any notable problems.
*- --------------------------------------------------------------------------*/
void ControlTv(Volume_Ctrl_Type Action)
{
  Message_Type* Cmd;

  if(Action == DO_NOTHING)
  {
    return;
  }
  
  if(Action == INCREASE_VOLUME)
  {
    Cmd = &IncreaseVolume_u32;
  }
  else if(Action == DECREASE_VOLUME)
  {
    Cmd = &DecreaseVolume_u32;
  }
  else
  {
    return;
  }
  TransmitData(Cmd);
}


//- Init Function -------------------------------------------------------------
void setup() {
  pinMode(IR_LED, OUTPUT);
  digitalWrite(IR_LED, HIGH);
  
  #if CFG_TESTS_EN
    pinMode(PIN_INCREASE_VOL, INPUT);
    pinMode(PIN_DECREASE_VOL, INPUT);
    Serial.begin(9600);
  #endif
}

void loop() {
  uint16 Tv_Volume_raw    = 0u;

  Tv_Volume_raw = (uint16)analogRead(SOUND_IN_ADC);
  
#if CFG_TESTS_EN
  if(digitalRead(PIN_INCREASE_VOL) == HIGH)
  {
    Serial.println("Button Pressed");
    delay(3000);
#else
  if(Tv_Volume_raw >= VOLUME_THRESHOLD)
  {
#endif

    ControlTv(DECREASE_VOLUME);
    delay(VOL_DECR_WAIT_MS);
    ControlTv(INCREASE_VOLUME);
  }

}
