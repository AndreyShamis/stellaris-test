#include "includes.h"


// Basically here I'm checking that everything works fine.
volatile unsigned long count;
long var_init = 2;

// An interrupt function.
void Timer1A_ISR(void);

// main function.
int main(void) {
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
	ROM_TimerControlStall(TIMER1_BASE, TIMER_A, true);
	ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, 2111);
	TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1A_ISR);	//	Register Interupt handler
	ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	count=3;
	ROM_TimerEnable(TIMER1_BASE, TIMER_A);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//	PORT F

	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);	//	PORT F LED

	// Unlock PF0 so we can change it to a GPIO input
    // Once we have enabled (unlocked) the commit register then re-lock it
    // to prevent further changes.  PF0 is muxed with NMI thus a special case.
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    // set user switches as inputs
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, BUTTON_1|BUTTON_2);			//	PORT F BUTTONS
    // turn weak pull-ups on
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, BUTTON_1|BUTTON_2, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);						// PORT A
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, TX_PIN);						// PORT A RF_TX_PIN
	while(1)
    {	
		int i = 0;
		for (i=0;i<16;i++) {
			var_init++;
			i+=1;
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, i);
			ROM_SysCtlDelay(1000);
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
			ROM_SysCtlDelay(1000000);
			long val = ROM_GPIOPinRead(GPIO_PORTF_BASE,BUTTON_2);

			if(!val)
				SendChip();

			if(i>=15)
			{
				i=0;
			}
		}
    }
}

void SendChip()
{
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x80);
	ROM_SysCtlDelay(10000000);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x00);
}

// The interrupt function definition.
void Timer1A_ISR(void)
{
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	count++;
}
