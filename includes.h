#include "inc/hw_ints.h" 	//	Macros that define the interrupt assignment on Stellaris devices (NVIC)
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"	// 	Macros defining the memory map of the Stellaris device. This includes
							// 	defines such as peripheral base address locations, e.g., GPIO_PORTF_BASE
#include "inc/hw_sysctl.h"	// 	Defines and macros for System Control API of driverLib. This includes
							//	API functions such as SysCtlClockSet and SysCtlClockGet.
#include "inc/hw_types.h"	// 	Defines common types and macros such as tBoolean and HWREG(x)
#include "driverlib/gpio.h"	//	Defines and macros for GPIO API of driverLib. This includes API functions
							//	such as GPIOPinTypePWM and GPIOPinWrite.
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"	// 	Defines and macros for NVIC Controller (Interrupt) API of DriverLib.
									// 	This includes API functions such as IntEnable and IntPrioritySet
#include "driverlib/timer.h"		//	Defines and macros for Timer API of driverLib. This includes API functions
									//	such as TimerConfigure and TimerLoadSet.
#include "driverlib/rom.h"


#include "define_pins.h"
#include "defines.h"

#include "driverlib/uart.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
