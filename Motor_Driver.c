#include "Motor_Driver.h"
uint8_t state='s';

extern bool syncflag;

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_64,
        30000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        3000
};


/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_64,
        30000,
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        3000
};

void MotorSetup(void)
{
    /*left motor*/
    /* Configuring P4.4 and P4.5 as Output. P2.4 as peripheral output for PWM and P1.1 for button interrupt */
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN5);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /*right motor*/
    /* Configuring P4.0 and P4.2 as Output. P2.5 as peripheral output for PWM and P1.4 for button interrupt */
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    //setDirection('f');
}

void SetRightDirection(void)
{
    /*left motor*/
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
    pwmConfig.dutyCycle = 9000;

    /*right motor*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    pwmConfig2.dutyCycle = 0;


}
void SetLeftDirection(void)
{
    /*left motor*/
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
    pwmConfig.dutyCycle = 0;

    /*right motor*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    pwmConfig2.dutyCycle = 9000;


}

void SetForwardDirection(void)
{
    /*left motor*/
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
    pwmConfig.dutyCycle = 9000;


    /*right motor*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    pwmConfig2.dutyCycle = 9000;


}

void SetStop(void)
{
    /*left motor*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5);
    pwmConfig.dutyCycle = 0;


    /*right motor*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    pwmConfig2.dutyCycle = 0;


}

void SetReverseDirection(void)
{
    /*left motor*/
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
    pwmConfig.dutyCycle = 27000;


    /*right motor*/
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7);
    pwmConfig2.dutyCycle = 27000;


}

void setDirection(char dir)
{
    state = dir;
    switch(dir)
    {
        case 'l':   SetLeftDirection();
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
                    break;

        case 'r':   SetRightDirection();
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
                    break;

        case 'f':   SetForwardDirection();
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
                    break;

        case 'b':   SetReverseDirection();
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
                    break;

        case 's':   SetStop();
                    syncflag = false;
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
                    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
                    break;
    }

}

void SetMotorSpeed(double Lspeed, double Rspeed)
{
    pwmConfig.dutyCycle = Lspeed;
    pwmConfig2.dutyCycle = Rspeed;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
}

void SetSpeeds(uint16_t lNotch, uint16_t rNotch)
{
    uint16_t leftCycle = 0, rightCycle = 0, diff = 0, pwmleft = 0, pwmright = 0;
    pwmleft = pwmConfig.dutyCycle /  lNotch;
    pwmright = pwmConfig2.dutyCycle /  rNotch;

    if(lNotch != rNotch)
    {
        if(lNotch > rNotch)
        {
            diff = (lNotch - rNotch) / 2;

            leftCycle = pwmConfig.dutyCycle - (diff * pwmleft);
            rightCycle = pwmConfig2.dutyCycle + (diff * pwmright);

            SetMotorSpeed(leftCycle, rightCycle);
        }
        else
        {
            diff = (rNotch - lNotch) / 2;

            leftCycle = pwmConfig.dutyCycle + (diff * pwmleft);
            rightCycle = pwmConfig2.dutyCycle - (diff * pwmright);

            SetMotorSpeed(leftCycle, rightCycle);
        }
    }
}

void SetBaseSpeed(uint16_t lNotch, uint16_t rNotch)
{
    uint16_t leftCycle = 0, rightCycle = 0, pwmleft = 0, pwmright = 0;
    pwmleft = pwmConfig.dutyCycle /  lNotch;
    pwmright = pwmConfig2.dutyCycle /  rNotch;
    leftCycle = 20 * pwmleft;
    rightCycle = 20 * pwmright;
    SetMotorSpeed(leftCycle, rightCycle);


}
