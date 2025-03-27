/*Header file for tv_sound_ctrl module*/

//- Configurations ------------------------------------------------------------
#define CFG_TESTS_EN        false
//- MACRO Definitions ---------------------------------------------------------
#define IR_LED              5u       // Pin 5 has PWM capabilities.
#define SOUND_IN_ADC        A0       // ADC channel assignment
#define CARRIER_FREQ        38000u  // 38khz carrier freq. NEC Protocol.
#define LEAD_BURST_DUR_MS   9u      // 9ms Lead pulse duration.
#define LEAD_LOW_PULSE_US   4500u   // 4500uS or 4.5mS duration for LOW time.
#define BURST_563uS         (563u - 293u)    // 563uS burst time to transmit '1' (562.2uS)
#define LOG_1_TX_LOW_US     (1687u - 47u)   // 1.687ms delay
#define LOG_0_TX_LOW_US     BURST_563uS // 563uS low period
#define VOLUME_THRESHOLD    256u  // <<<<<--------------------------------------------------ADJUST HERE 
#define VOL_DECR_WAIT_MS    9000u // wait time after decreasing volume.
#define REPEAT_SIG_SPACE_US 2200u // 2.25ms low signal.
#define REPEAT_SIG_DELAY_MS 110u  // 110ms delay for each REPEAT signal

//- Following Macros are for testing
#define PIN_INCREASE_VOL  7u

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

/*----------------------------End of file -----------------------------------*/
