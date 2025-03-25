/* Controlling TV using IR NEC protocol */

//- MACRO Definitions ---------------------------------------------------------
#define IR_LED            5u       // Pin 5 has PWM capabilities.
#define CARRIER_FREQ      38000u  // 38khz carrier freq. NEC Protocol.
#define LEAD_BURST_DUR_MS 9u      // 9ms Lead pulse duration.
#define LEAD_LOW_PULSE_US 4500u   // 4500uS or 4.5mS duration for LOW time.
#define LOG_1_0_TX_US     (563u - 25u)    // 563uS burst time to transmit '1' (562.2uS)
#define LOG_1_TX_LOW_US   (1687u - 47u)   // 1.687ms delay
#define LOG_0_TX_LOW_US   LOG_1_0_TX_US // 563uS low period
// #define INIT_BURST_DELAY  (LEAD_BURST_DUR_MS - 3u)

//- Following Macros are for testing
#define PIN_INCREASE_VOL  7u
#define PIN_DECREASE_VOL  4u

//- Type Definitions  ---------------------------------------------------------
typedef long  uint32;
typedef long  Message_Type;
typedef int   uint16;
typedef byte  uint8;

typedef enum {
  INCREASE_VOLUME,
  DECREASE_VOLUME,
  DO_NOTHING,
}Volume_Ctrl_Type;

//- Global Variables  ---------------------------------------------------------
uint32 IncreaseVolume_u32 = 0xE51ABF40; // Send the msg, LSB first
uint32 DecreaseVolume_u32 = 0xE11EBF40;

//- Inline Functions  ---------------------------------------------------------

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
void StartComm(void)
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
      delayMicroseconds(270u); // edit here -----------------------------------
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
  pinMode(PIN_INCREASE_VOL, INPUT);
  pinMode(PIN_DECREASE_VOL, INPUT);
  digitalWrite(IR_LED, HIGH);
  Serial.begin(9600);
}

void loop() {
  Volume_Ctrl_Type Action = DO_NOTHING;

  if(digitalRead(PIN_INCREASE_VOL) == HIGH)
  {
    Serial.println("Button Pressed");
    delay(3000);
    Action = INCREASE_VOLUME;
  }

  if(digitalRead(PIN_DECREASE_VOL) == HIGH)
  {
    delay(3000);
    Action = DECREASE_VOLUME;
  }
  
  // put your main code here, to run repeatedly:
  ControlTv(Action);
  Action = DO_NOTHING; //- test code
}
