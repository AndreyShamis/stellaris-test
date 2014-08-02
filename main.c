
#include "includes.h"


volatile unsigned long count;

long var_init = 2;
int  uart_controll = 0;
// An interrupt function.
void Timer1A_ISR(void);
// An interrupt function.

void UARTIntHandler(void);
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
void SendTx(char *string,int size);
void DisableTx();
void EnbaleTx();
/**
 * main function
 */
int main(void)
{
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
	ROM_TimerControlStall(TIMER1_BASE, TIMER_A, true);
	ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, 2111);
	TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1A_ISR);	//	Register Interupt handler
	ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	count=3;
	ROM_TimerEnable(TIMER1_BASE, TIMER_A);

    // Enable the peripherals used by this example.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//	PORT F
    //
    // Enable processor interrupts.
    ROM_IntMasterEnable();

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

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

    // Configure the UART for 115,200, 8-N-1 operation.
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    // Enable the UART interrupt.
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	// PORT A
    ROM_GPIOPinTypeGPIOOutput(TX_PORT, TX_PIN);			// PORT A RF_TX_PIN
	ROM_GPIOPinTypeGPIOInput(RX_PORT, RX_PIN);

	while(1)
    {	
		int i = 0;
		for (i=0;i<16;i++) {
		    //
		    var_init++;
			i+=1;
			//ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 2);
			ROM_SysCtlDelay(200);
			//ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
			//ROM_SysCtlDelay(1000);
			long val 	= ROM_GPIOPinRead(GPIO_PORTF_BASE,BUTTON_2);

			long rx_val = ROM_GPIOPinRead(RX_PORT,RX_PIN);

			if(!val || uart_controll == 1){
				EnbaleTx();
				uart_controll = 0;
				ROM_SysCtlDelay(9000);
			}
			DisableTx();
			if(rx_val)
			{
				UARTSend((unsigned char *)"Rx:", sizeof("Rx:"));
				UARTSend((unsigned char *)rx_val, sizeof(rx_val)-1);
				UARTSend((unsigned char *)"\r\n", sizeof("\r\n"));
				ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 4);
				ROM_SysCtlDelay(90000);

			}
			if(i>=15){
				//SendChip();
				i=0;
			}
		}
    }
}


//*****************************************************************************
// The UART interrupt handler.
//*****************************************************************************
void UARTIntHandler(void)
{
    unsigned long ulStatus;
    // Get the interrrupt status.
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);
    // Clear the asserted interrupts.
    ROM_UARTIntClear(UART0_BASE, ulStatus);

    // Loop while there are characters in the receive FIFO.
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
    	unsigned char ucData = ROM_UARTCharGetNonBlocking(UART0_BASE);
    	//SendTx("Hello RussiaA)0JKL",sizeof("Hello RussiaA)0JKL"));
    	if(ucData == 'A'){
    		uart_controll = 1;
    	}
    	else if(ucData == 'Z'){
    		uart_controll = 0;
    	}
    		//char str[20];
    		//ucData = ucData + 30;
    		//sprintf(str, "Received char %c\n", ucData);
    		UARTSend((unsigned char *)"Received char:" , sizeof("Received char:"));
    		//UARTSend((unsigned char *)ucData, 2);
    		ROM_UARTCharPutNonBlocking(UART0_BASE, ucData);
    		UARTSend((unsigned char *)"\r\n" , sizeof("\r\n"));
    	//}
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
        SysCtlDelay(SysCtlClockGet() / (1000 * 3));
    }
}

//*****************************************************************************
// Send a string to the UART.
//*****************************************************************************
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    // Loop while there are more characters to send.
    while(ulCount--){
        // Write the next character to the UART.
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}

void SendChip()
{
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x80);
	ROM_SysCtlDelay(90000);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x00);
	ROM_SysCtlDelay(10);
}

void SendTx(char *string,int size)
{
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 2);
	int i=0;
	//for(i=0; i<size; i++){
	//	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , string[i]);
	//	ROM_SysCtlDelay(2000);
	//}
	for(i=0; i<255; i++){
			ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , i);
			ROM_SysCtlDelay(100);
		}

	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x00);
}

void EnbaleTx()
{
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x80);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 2);

}

void DisableTx()
{
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x00);
	ROM_SysCtlDelay(100);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, TX_PIN , 0x00);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
	ROM_SysCtlDelay(800);
}

// The interrupt function definition.
void Timer1A_ISR(void)
{
	ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	count++;
}
