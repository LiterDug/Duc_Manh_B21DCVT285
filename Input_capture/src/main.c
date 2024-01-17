/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/

void rcc_configuration (void);
void gpio_configuration (void);
void nvic_configuration (void);
void tim2_configuration (void);
void tim3_configuration (void);
void usart_configuration (void);
int FPUTC (int ch,file *f)
{
    Usart_senddata (USART1, (U8) ch);
    while (Usart_getflagstatus (USART1,USART_FLAG_TXE) ==reset);
    return ch;
}

int main (void)
{

    Rcc_configuration ();
    Configuring the Clock
    Gpio_configuration ();//io Port configuration
    Tim3_configuration ();
    Tim2_configuration ();
    Nvic_configuration ();
    Usart_configuration ();

    while (1)
    {
        if (flag==1)
        {
            flag=0;
            printf ("The Duty is%d/r/n", duty);
            printf ("The frequency is%.2fkhz/r/n", freq);
        }
    }
}

void Rcc_configuration (void)
{
    Systeminit ();
    Rcc_apb2periphclockcmd (rcc_apb2periph_gpioa| rcc_apb2periph_usart1,enable);
    Rcc_apb1periphclockcmd (rcc_apb1periph_tim2| rcc_apb1periph_tim3,enable);
    Rcc_apb2periphclockcmd (rcc_apb2periph_afio,enable);
}
void Gpio_configuration (void)
{
    Gpio_inittypedef gpio_initstructure;
    Led
    Gpio_initstructure.gpio_pin=gpio_pin_9;//tx
    Gpio_initstructure.gpio_speed=gpio_speed_50mhz;
    GPIO_INITSTRUCTURE.GPIO_MODE=GPIO_MODE_AF_PP;
    Gpio_init (gpioa,&gpio_initstructure);

    Gpio_initstructure.gpio_pin=gpio_pin_10;//rx
    gpio_initstructure.gpio_mode=gpio_mode_in_floating;
    Gpio_init (gpioa,&gpio_initstructure);


    Gpio_initstructure.gpio_pin=gpio_pin_6;//pwm,tim3_ch1
    Gpio_initstructure.gpio_speed=gpio_speed_50mhz;
    GPIO_INITSTRUCTURE.GPIO_MODE=GPIO_MODE_AF_PP;
    Gpio_init (gpioa,&gpio_initstructure);

    Gpio_initstructure.gpio_pin=gpio_pin_1;//tim2_ch2
    gpio_initstructure.gpio_mode=gpio_mode_in_floating;
    Gpio_init (gpioa,&gpio_initstructure);
}

void Tim2_configuration (void)
{
    Tim_timebaseinittypedef tim_timebasestruct;
    Tim_icinittypedef tim_icinitstructure;

    tim_icinitstructure.tim_channel=tim_channel_2;//Select TIM2_CH2, select Input IC2 map to TI2
    tim_icinitstructure.tim_ icpolarity=tim_icpolarity_rising;//Rising edge capture
    tim_icinitstructure.tim_icselection=tim_icselection_directti;// Map to TI2 on
    tim_icinitstructure.tim_icprescaler=tim_icpsc_div1;//to perform a capture each time an edge is detected on the capture input
    tim_ icinitstructure.tim_icfilter=0;//filter setting, through a few periodic jump to determine the waveform stability. (Sampling high, only the continuous acquisition of n levels is high level is considered valid, otherwise less than n is considered invalid, n take 0x0-0xf)
    Tim_pwmiconfig (tim2,&tim_icinitstructure);// The above is the input capture configuration
    Tim_selectinputtrigger (TIM2,TIM_TS_TI2FP2);//Select the filtered TI2FP2 input as the trigger source, triggering the reset of the following program
    Tim_selectslavemode (tim2,tim_slavemode_reset)///From the mode controller is set to reset mode-the selected trigger signal rising edge reinitialize counter and produces an update signal (rising edge one to, tim2->cnt is cleared, each rising edge comes, CNT will be cleared 0)
    Tim_selectmasterslavemode (tim2,tim_masterslavemode_enable);//start timer passive trigger
    Tim_itconfig (tim2,tim_ it_cc2,enable);//capture Interrupt Open
    Tim_clearflag (TIM2,TIM_IT_CC2);//clear Flag bit
    Tim_cmd (tim2,enable);//Enable Timer 2
}
void Tim3_configuration (void)
{
    Tim_timebaseinittypedef tim_timebasestruct;
    Tim_ocinittypedef tim_ocinitstructure;
    tim_timebasestruct.tim_period=72000;//Count Initial value
    tim_timebasestruct.tim_prescaler=0;//divide factor
    tim_timebasestruct.tim_clockdivision=0;//Clock Segmentation
    tim_timebasestruct.tim_countermode=tim_countermode_up;//Up counting mode
    Tim_timebaseinit (tim3,&tim_timebasestruct);
    Tim3_ch1 as PWM output
    TIM_OCINITSTRUCTURE.TIM_OCMODE=TIM_OCMODE_PWM1;
    tim_ocinitstructure.tim_outputstate=tim_outputstate_enable;
    tim_ocinitstructure.tim_pulse=36000;
    Tim_ocinitstructure.tim_ocpolarity=tim_ocpolarity_high;
    Tim_oc1init (tim3,&tim_ocinitstructure);

    Tim_oc1preloadconfig (tim3,tim_ocpreload_enable);
    Tim_arrpreloadconfig (tim3,enable);
    Tim_cmd (tim3,enable);
}

void Nvic_configuration (void)
{
    Nvic_inittypedef nvic_initstructure;

    Nvic_prioritygroupconfig (nvic_prioritygroup_1);

    Nvic_initstructure.nvic_irqchannel = TIM2_IRQN;
    nvic_initstructure.nvic_irqchannelpreemptionpriority = 0;
    nvic_initstructure.nvic_irqchannelsubpriority = 1;
    Nvic_initstructure.nvic_irqchannelcmd = ENABLE;
    Nvic_init (&nvic_initstructure);

}

void Usart_configuration (void)
{
    Usart_inittypedef usart_initstructure;

    usart_initstructure.usart_baudrate=115200;
    usart_initstructure.usart_wordlength=usart_wordlength_8b;
    Usart_initstructure.usart_stopbits=usart_stopbits_1;
    Usart_initstructure.usart_parity=usart_parity_no;
    Usart_initstructure.usart_hardwareflowcontrol=usart_hardwareflowcontrol_none;
    Usart_initstructure.usart_mode=usart_mode_rx| Usart_mode_tx;

    Usart_init (usart1,&usart_initstructure);
    Usart_cmd (usart1,enable);
    Usart_clearflag (USART1,USART_FLAG_TC);
}


Interrupt Program
void Tim2_irqhandler (void)
{
    if (Tim_getitstatus (TIM2,TIM_IT_CC2) ==bit_set)
    {
        float ic2value=0;
        float dutycycle=0;


        float frequency=0;
        float pulse=0;


        Ic2value=tim_getcapture2 (TIM2);//Get the value of CCR2
        Pulse=tim_getcapture1 (TIM2);//Get the value of CCR1
        Dutycycle=pulse/ic2value;
        Frequency=72000000/ic2value;
        Duty= (U32) (dutycycle*100);
        Freq= (frequency/1000);

        flag=1;

        Tim_clearitpendingbit (TIM2,TIM_IT_CC2);
    }
}

Duty and freq are defined global variables
extern u32 duty;
extern U16 Freq;
U32 duty=0;
U16 Freq=0;
