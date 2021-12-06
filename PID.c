/* Standard Includes */
#include "PID.h"

extern volatile uint32_t iIndex;
extern bool runFlag;

/* Statics */
uint16_t notchesdetected, notchesdetected2, timer_count = 0;

bool syncflag = false, speedflag = false;
extern uint8_t state;

void Initalise_encoderTimer(void)
{
    /* Timer_A UpMode Configuration Parameter */
    const Timer_A_UpModeConfig encoderUpConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/3 = 1MHz
            TICKPERIOD2,                             // 1000 tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    int a = CS_getSMCLK();

    /* Configuring Timer_A0 for Up Mode */
    Timer_A_configureUpMode(TIMER_A2_BASE, &encoderUpConfig);

    /* Enabling interrupts and starting the timer */
    Interrupt_enableInterrupt(INT_TA2_0);
}

void WheelEncoderSetup(void)
{
    //wheel encoder 1
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);

    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN0);

    //wheel encoder 2
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN2);

    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN2);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN2);

    Interrupt_enableInterrupt(INT_PORT5);
}

//******************************************************************************
//
//This is the TIMER_A interrupt vector service routine.
//
//******************************************************************************

void TA2_0_IRQHandler(void)
{
    if(timer_count != 6) timer_count++;
    else
    {
        if(timer_count == 6)
        {
            if(notchesdetected != 20 || notchesdetected2 != 20) SetBaseSpeed(notchesdetected, notchesdetected2);
            if(syncflag == false && state == 'f' && speedflag == false)
            {
                syncflag = true;
                speedflag = true;
                if((notchesdetected<19 || notchesdetected>21) && (notchesdetected2<19 || notchesdetected2>21))
                    SetBaseSpeed(notchesdetected, notchesdetected2);
            }
            notchesdetected=0;
            notchesdetected2=0;
            iIndex++;
            runFlag = false;
            timer_count = 0;
        }
    }
    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

}

/* GPIO ISR */
void PORT5_IRQHandler(void)
{
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    if(status & GPIO_PIN0)
    {
        notchesdetected2++; //right
    }
    if(status & GPIO_PIN2)
    {
         notchesdetected++; //left
    }

    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
}
