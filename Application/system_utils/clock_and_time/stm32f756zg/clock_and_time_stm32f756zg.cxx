/* Be name khoda
******************************************************************************
File:     main.c
Info:     MMohtashamiRad

    This code is created in order to set the system clock and to have a
    relaible source of timing for delays and ...
******************************************************************************
*/
#ifdef STM32F756xx
#include "clock_and_time.hxx"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_tim.h"

#define MyTimer_Number TIM3
#define MyTimer_Number_IRQ TIM3_IRQn

TIM_HandleTypeDef MyTimer_handel;

void Error_Handler(void){} // in main.c
// void periodicCall_1ms();

uint64_t ClockAndTime::mili_seconds_ = 0;

/*
    micro_seconds_
    this variable holds the MicroSecons from system start up.
    each time getSystem_u is called micro_seconds_ will update .
    it's actually a combination of mili_seconds_ and 1ms timer periodic value.

*/
uint64_t ClockAndTime::micro_seconds_ = 0;

uint64_t ClockAndTime::getSystem_uS()
{
    micro_seconds_ = mili_seconds_ * 1000 + TIM3->CNT;
    return micro_seconds_;
}

inline uint64_t ClockAndTime::getSystem_mS() { return mili_seconds_; }

uint32_t HAL_GetTick(void) { return ClockAndTime::getSystem_mS(); }

void ClockAndTime::periodicTimerInit()
{
    MyTimer_handel.Instance = MyTimer_Number;

    __HAL_RCC_TIM3_CLK_ENABLE();

    /*##-2- Configure the NVIC for TIMx
     * ########################################*/
    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(MyTimer_Number_IRQ, 5, 5);

    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(MyTimer_Number_IRQ);
    MyTimer_handel.Init.Prescaler = 72;
    MyTimer_handel.Init.Period = 1000;
    MyTimer_handel.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    MyTimer_handel.Init.CounterMode = TIM_COUNTERMODE_UP;
    MyTimer_handel.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&MyTimer_handel) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    //*##-2- Start the TIM Base generation in interrupt mode
    //####################*/
    /* Start Channel1 */
    if (HAL_TIM_Base_Start_IT(&MyTimer_handel) != HAL_OK)
    {
        /* Starting Error */
        Error_Handler();
    }
}

void ClockAndTime::delay_ms(__uint32_t dly)
{
    uint64_t t0 = mili_seconds_;
    while (mili_seconds_ < (t0 + dly))
        ;
}

void ClockAndTime::delay_us(__uint32_t dly)
{
    uint64_t t0 = mili_seconds_ * 1000 + TIM3->CNT;

    while ((mili_seconds_ * 1000 + TIM3->CNT) < (t0 + dly))
        ;
}

void TIM3_IRQHandler()
{
    // Checks whether the TIM3 interrupt has occurred or not
    if (__HAL_TIM_GET_FLAG(&MyTimer_handel, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&MyTimer_handel, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_IT(&MyTimer_handel, TIM_IT_UPDATE);
            ClockAndTime::IncreaseMillies();
            // periodicCall_1ms();
        }
    }
}
inline void ClockAndTime::IncreaseMillies() { mili_seconds_++; }
/*
this delay is used mostly in interrupts.
*/
__attribute__((optimize(0))) void ClockAndTime::softDelay_ms(uint32_t dly)
{
    uint32_t x = 0;
    dly = dly * SystemCoreClock / 1000 / 26;
    for (uint32_t i = 0; i < dly; i++)
    {
        if (x > i) // dummy statement
            break;
    }
}

void HAL_Delay(uint32_t delay) { ClockAndTime::delay_ms(delay); }

#endif
