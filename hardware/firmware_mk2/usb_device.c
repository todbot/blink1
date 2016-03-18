// todbot: adds RAM_BASED_SERIALNUMBER based one: http://www.microchip.com/forums/m669383.aspx
// todbot: might include changes from: https://www.microchip.com/forums/m643065-print.aspx
// 
/********************************************************************
  File Information:
    FileName:     	usb_device.c
    Dependencies:	See INCLUDES section
    Processor:		PIC18,PIC24, PIC32 and dsPIC33E USB Microcontrollers
    Hardware:		This code is natively intended to be used on Mirochip USB
                    demo boards.  See www.microchip.com/usb (Software & Tools 
                    section) for list of available platforms.  The firmware may 
                    be modified for use on other USB platforms by editing the
    				HardwareProfile.h and HardwareProfile - [platform].h files.
    Complier:  	    Microchip C18 (for PIC18),C30 (for PIC24 and dsPIC33E)
                    and C32 (for PIC32)
    Company:		Microchip Technology, Inc.
    
    Software License Agreement:
    
    The software supplied herewith by Microchip Technology Incorporated
    (the "Company") for its PIC(r) Microcontroller is intended and
    supplied to you, the Company's customer, for use solely and
    exclusively on Microchip PIC Microcontroller products. The
    software is owned by the Company and/or its supplier, and is
    protected under applicable copyright laws. All rights are reserved.
    Any use in violation of the foregoing restrictions may subject the
    user to criminal sanctions under applicable laws, as well as to
    civil liability for the breach of the terms and conditions of this
    license.
    
    THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
    WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
    TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
    IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
    CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

  Summary:
    This file contains functions, macros, definitions, variables,
    datatypes, etc. that are required for usage with the MCHPFSUSB device
    stack. This file should be included in projects that use the device stack. 
    
    This file is located in the "\<Install Directory\>\\Microchip\\USB"
    directory.

  Description:
    USB Device Stack File
    
    This file contains functions, macros, definitions, variables,
    datatypes, etc. that are required for usage with the MCHPFSUSB device
    stack. This file should be included in projects that use the device stack.
    
    This file is located in the "\<Install Directory\>\\Microchip\\USB"
    directory.
    
    When including this file in a new project, this file can either be
    referenced from the directory in which it was installed or copied
    directly into the user application folder. If the first method is
    chosen to keep the file located in the folder in which it is installed
    then include paths need to be added so that the library and the
    application both know where to reference each others files. If the
    application folder is located in the same folder as the Microchip
    folder (like the current demo folders), then the following include
    paths need to be added to the application's project:
    
    .
    ..\\..\\MicrochipInclude
    
    If a different directory structure is used, modify the paths as
    required. An example using absolute paths instead of relative paths
    would be the following:
    
    C:\\Microchip Solutions\\Microchip\\Include
    
    C:\\Microchip Solutions\\My Demo Application 

********************************************************************
 File Description:

 Change History:
  Rev    Description
  ----   -----------
  2.6    Added USBCancelIO() function.  Moved and some stack
         defintions to be more consistant with the host stack.

  2.6a   Fixed issue where a SET_CONFIGURATION received could cause
         inability to transmit on an endpoint if using ping-pong
         and an odd number of packets had been sent on that endpoint 

  2.7    Fixed error where the USB error interrupt flag was not getting
         cleared properly for PIC32 resulting in lots of extra error interrupts.
         http://www.microchip.com/forums/tm.aspx?m=479085

         Fixed issue with dual role mode when device run in polling
         mode.  Interrupts were remaining enabled after the host mode
         operation was complete.  This was incompatible with polling
         mode operation.

         Changed how the bus sensing works.  In previous revisions it
         was impossible to use the USBDeviceDetach to detach from the
         bus if the bus voltage was still present.  This is now
         possible.  It was also possible to move the device to the 
         ATTACHED state in interrupt mode even if the bus voltage 
         wasn't available.  This is now prohibited unless VBUS is
         present.
         
         Improved error case handling when the host sends more OUT
         bytes in a control transfer than the firmware was expecting
         to receive (based on the size parameter when calling USBEP0Receive()).
         
         In the USBStdSetCfgHandler(), modified the code so the USBDeviceState
         variable only gets updated to the CONFIGURED_STATE at the end of the 
         function.  

  2.7a   Update to support the PIC18F47J53 A1 and later revision
         devices.

         Fixed an error on 16-bit and 32-bit processors where a word access
         could be performed on a byte pointer resulting in possible address
         errors with odd aligned pointers.
         
  2.8    Several changes to the way control transfers get processed,
         so as to support the ability to allow application/class specific
         handler code to defer the status stage.  
         Implemented USBCtrlEPAllowStatusStage() API function.
         Implemented USBDeferStatusStage() API function (macro).
         These changes also greatly relax the USBDeviceTasks() calling frequency 
         requirement, while allowing USB class handlers more flexibility.
         
         Also implemented the following API functions and macros, for delaying 
         the data stage of a control transfer (with data stage):
         USBDeferINDataStage()
         USBDeferOUTDataStage()
         USBOUTDataStageDeferred()
         USBINDataStageDeferred()
         USBCtrlEPAllowDataStage()      
         
         Fixed USB reset event handler issue, where the USB stack would 
         re-initialize global interrupt settings in the interrupt context, on 
         PIC18 devices with the stack operated in USB_INTERRUPT mode.
         
         Fixed handling of SET/CLEAR FEATURE (endpoint halt) host requests.
         Previous implementation would not always initialize endpoints correctly 
         to DATA0 DTS state after a clear feature endpoint halt request, for 
         all ping pong mode and usage scenarios.
         
   2.9   Fixed an issue with STALL handling behavior on non-EP0 endpoints, for 
         PIC24 and PIC32 devices.  
   		  
   		 Fixed an issue where the ep_data_in[]/ep_data_out[] flags weren't 
   		 getting re-initialized coincident with the hardware ping pong pointer 
   		 reset during set configuration events.
   		 
   		 Implemented USBGetNextHandle() API function (actually a macro, defined
   		 in usb_device.h).

   2.9d  Added build option for disabling DTS checking
   
   2.9f  Adding pragma for PIC18F97J94 Family BDT location.

   2.9h  Updated to be able to support optional Microsoft OS Descriptors

   2.9i  Updated to set UCON<SUSPND> bit on PIC16F USB devices during 
         suspend, so as to save power.
   
********************************************************************/

/*----------------------------------------------------------------------------------
The USBDeviceTasks() function is responsible for detecting and processing various
USB bus events and host requests, such as those required for USB enumeration, when
the USB cable is first attached to the host.  This function is the main dispatcher
routine for the USB stack.

Additional API functions and macros are also provided by the USB stack, which can be 
used to send/receive USB data to/from the host, among other things.  A full list 
of the available implemented functions/macros are provided in the 
"MCHPFSUSB Library Help".  For normal installations of the MCHPFSUSB Framework,
the USB API documentation can be found from:

Start menu --> (All Programs) --> Microchip --> MCHPFSUSB vX.x --> Documents --> MCHPFSUSB Library Help

Once the help file is opened, the API functions/macros are described in the following section:
Library Interface (API) --> Device/Peripheral --> Device Stack --> Interface Routines
Additional API functions may also be provided depending upon the specific USB device class
implemented, and these functions are also documented in the MCHPFSUSB Library Help.


If the USB stack is operated in "USB_POLLING" mode (user selectable option in 
usb_config.h), then the application firmware is reponsible for calling the 
USBDeviceTasks() function periodically.  If the USB stack is operated in the 
"USB_INTERRUPT" mode, then the application firmware does not have to directly 
call USBDeviceTasks(), as it will execute only when necessary as an interrupt handler.

In order to properly operate a USB connection, and to correctly process and respond
to control transfers in the maximum time allowed by the USB specifications, the
USBDeviceTasks() function/interrupt handler must be allowed to execute in a timely
fashion.

When the USB module is enabled, the USB cable is attached to the host, the USB bus
is not in the suspend state, and the USB stack is operated in the USB_POLLING mode 
with ping pong buffering enabled (at least) on EP0 OUT,  then the maximum allowed 
time between calls to the USBDeviceTasks() function needs to be:

The faster of:
1.  Once per ~1.8ms, when USBDeviceState == ADR_PENDING_STATE
2.  Once per ~9.8ms, when USBDeviceState == (any other value other than ADR_PENDING_STATE)
3.  Fast enough to ensure the USTAT FIFO can never get full.  See additional explanation below.

Additional details of the above timing limits are provided:

Timing item #1: This parameter originates from the 2ms set address "recovery interval"
specification dictated by section "9.2.6.3 Set Address Processing" of the official 
USB 2.0 specifications.

Timing item #2: This parameter originates from several "10 ms" criteria in the 
USB 2.0 specifications.  For example, reset recovery intervals, resume recovery 
intervals, suspend to actual current reduction, etc. have timing maximums of 10ms.

Timing item #3: This is not a fixed X.X ms parameter, but depends on the  
transaction rate implemented by the application.  The USBDeviceTasks() function is
responsible for popping entries off the USTAT FIFO.  If the FIFO ever gets full,
then no further USB transactions are allowed to occur, until the firmware pops entries
off the FIFO.  In practice, this means the firmware should call USBDeviceTasks() at
a rate at least as fast as once every three times the USBTransferOnePacket() function
is called.  This ensures that the rate that USTAT FIFO entries are getting added to
the FIFO is lower than the rate that the entries are getting popped off the FIFO (the
USBDeviceTasks() function will pop up to 4 entries per call), which is a
necessary criteria to ensure the USTAT FIFO entries don't "pile up."  Calling
USBDeviceTasks() even more often, ex: >=1 to 1 ratio of USBDeviceTasks() to 
USBTransferOnePacket(), adds further protection against the USTAT FIFO getting full,
and is therefore recommended.

When the USB stack is operated in USB_INTERRUPT mode, then the above timing 
parameters should be interpreted to be the longest allowed time that the USB 
interrupts may be masked/disabled for, before re-enabling the USB interrupts.

Calling USBDeviceTasks() (or allowing USBDeviceTasks() to be called) more often 
will still have potential USB data rate speed and processing latency benefits.
It is also beneficial to call USBDeviceTasks() more often than theoretically 
required, since it has been observed that not all host/drivers/bios/hubs are 
100% consistently compliant with all timing parameters of the USB 2.0 specifications.
Therefore, in a USB_POLLING based application, it is still suggested to call 
USBDeviceTasks() as often as there are free CPU cycles.  This ensures best 
performance, along with best possible compatibility with all existing USB 
hosts/hubs (both those that are compliant and [partially] non-compliant).

If ping pong buffering is not enabled on (at least) EP0 OUT, then it is required
to call (or allow to execute) USBDeviceTasks() much more frequently (ex: once 
per 100us, or preferrably faster).  Therefore, in all applications, it is 
normally recommended to select either the USB_PING_PONG__FULL_PING_PONG or 
USB_PING_PONG__EP0_OUT_ONLY mode (user option in usb_config.h), as these modes
allow for much more relaxed timing requirements, and therefore greater application
firmware design flexibility.
//----------------------------------------------------------------------------------*/

/** INCLUDES *******************************************************/
#include "./USB/usb.h"
#include "HardwareProfile.h"

#include "../USB/usb_device_local.h"


#if defined(USB_USE_MSD)
    #include "./USB/usb_function_msd.h"
#endif

#if !defined(USE_USB_BUS_SENSE_IO)
    #undef USB_BUS_SENSE
    #define USB_BUS_SENSE 1
#endif

#if defined(USB_DEVICE_DISABLE_DTS_CHECKING)
    #define _DTS_CHECKING_ENABLED 0
#else
    #define _DTS_CHECKING_ENABLED _DTSEN
#endif

/** DEFINITIONS ****************************************************/

/** VARIABLES ******************************************************/
#if defined(__18CXX)
    #pragma udata
#endif

#if defined( RAM_BASED_SERIALNUMBER )
// RAMSNt defined in local usb_config.h
extern RAMSNt my_RAMSN;
#endif


USB_VOLATILE USB_DEVICE_STATE USBDeviceState;
USB_VOLATILE BYTE USBActiveConfiguration;
USB_VOLATILE BYTE USBAlternateInterface[USB_MAX_NUM_INT];
volatile BDT_ENTRY *pBDTEntryEP0OutCurrent;
volatile BDT_ENTRY *pBDTEntryEP0OutNext;
volatile BDT_ENTRY *pBDTEntryOut[USB_MAX_EP_NUMBER+1];
volatile BDT_ENTRY *pBDTEntryIn[USB_MAX_EP_NUMBER+1];
USB_VOLATILE BYTE shortPacketStatus;
USB_VOLATILE BYTE controlTransferState;
USB_VOLATILE IN_PIPE inPipes[1];
USB_VOLATILE OUT_PIPE outPipes[1];
USB_VOLATILE BYTE *pDst;
USB_VOLATILE BOOL RemoteWakeup;
USB_VOLATILE BOOL USBBusIsSuspended;
USB_VOLATILE USTAT_FIELDS USTATcopy;
USB_VOLATILE BYTE endpoint_number;
USB_VOLATILE BOOL BothEP0OutUOWNsSet;
USB_VOLATILE EP_STATUS ep_data_in[USB_MAX_EP_NUMBER+1];
USB_VOLATILE EP_STATUS ep_data_out[USB_MAX_EP_NUMBER+1];
USB_VOLATILE BYTE USBStatusStageTimeoutCounter;
volatile BOOL USBDeferStatusStagePacket;
volatile BOOL USBStatusStageEnabledFlag1;
volatile BOOL USBStatusStageEnabledFlag2;
volatile BOOL USBDeferINDataStagePackets;
volatile BOOL USBDeferOUTDataStagePackets;


#if (USB_PING_PONG_MODE == USB_PING_PONG__NO_PING_PONG)
    #define BDT_NUM_ENTRIES      ((USB_MAX_EP_NUMBER + 1) * 2)
#elif (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY)
    #define BDT_NUM_ENTRIES      (((USB_MAX_EP_NUMBER + 1) * 2)+1)
#elif (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
    #define BDT_NUM_ENTRIES      ((USB_MAX_EP_NUMBER + 1) * 4)
#elif (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0)
    #define BDT_NUM_ENTRIES      (((USB_MAX_EP_NUMBER + 1) * 4)-2)
#else
    #error "No ping pong mode defined."
#endif

/** USB FIXED LOCATION VARIABLES ***********************************/
#if defined(__18CXX)
    #pragma udata USB_BDT=USB_BDT_ADDRESS
#endif

volatile BDT_ENTRY BDT[BDT_NUM_ENTRIES] BDT_BASE_ADDR_TAG;

/********************************************************************
 * Section B: EP0 Buffer Space
 *******************************************************************/
volatile CTRL_TRF_SETUP SetupPkt CTRL_TRF_SETUP_ADDR_TAG;
volatile BYTE CtrlTrfData[USB_EP0_BUFF_SIZE] CTRL_TRF_DATA_ADDR_TAG;

/********************************************************************
 * Section C: non-EP0 Buffer Space
 *******************************************************************/
#if defined(USB_USE_MSD)
	//volatile far USB_MSD_CBW_CSW msd_cbw_csw;
	volatile USB_MSD_CBW msd_cbw;
	volatile USB_MSD_CSW msd_csw;
	//#pragma udata

	#if defined(__18CXX)
		#pragma udata myMSD=MSD_BUFFER_ADDRESS
	#endif
	volatile char msd_buffer[512];
#endif



////Depricated in v2.2 - will be removed in a future revision
#if !defined(USB_USER_DEVICE_DESCRIPTOR)
    //Device descriptor
    extern ROM USB_DEVICE_DESCRIPTOR device_dsc;
#else
    USB_USER_DEVICE_DESCRIPTOR_INCLUDE;
#endif

#if !defined(USB_USER_CONFIG_DESCRIPTOR)
    //Array of configuration descriptors
    extern ROM BYTE *ROM USB_CD_Ptr[];
#else
    USB_USER_CONFIG_DESCRIPTOR_INCLUDE;
#endif

extern ROM BYTE *ROM USB_SD_Ptr[];

/** DECLARATIONS ***************************************************/
#if defined(__18CXX)
    #pragma code
#endif

/** Macros *********************************************************/

/** Function Prototypes ********************************************/
//External
//This is the prototype for the required user event handler
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size);

//Internal Functions
static void USBCtrlEPService(void);
static void USBCtrlTrfSetupHandler(void);
static void USBCtrlTrfInHandler(void);
static void USBCheckStdRequest(void);
static void USBStdGetDscHandler(void);
static void USBCtrlEPServiceComplete(void);
static void USBCtrlTrfTxService(void);
static void USBCtrlTrfRxService(void);
static void USBStdSetCfgHandler(void);
static void USBStdGetStatusHandler(void);
static void USBStdFeatureReqHandler(void);
static void USBCtrlTrfOutHandler(void);
static void USBConfigureEndpoint(BYTE EPNum, BYTE direction);
static void USBWakeFromSuspend(void);
static void USBSuspend(void);
static void USBStallHandler(void);

//static BOOL USBIsTxBusy(BYTE EPNumber);
//static void USBPut(BYTE EPNum, BYTE Data);
//static void USBEPService(void);
//static void USBProtocolResetHandler(void);

/******************************************************************************/
/** Function Implementations *************************************************/
/******************************************************************************/

/******************************************************************************/
/** Internal Macros *********************************************************/
/******************************************************************************/

/****************************************************************************
  Function:
    void USBAdvancePingPongBuffer(BDT_ENTRY** buffer)

  Description:
    This function will advance the passed pointer to the next buffer based on
    the ping pong option setting.  This function should be used for EP1-EP15
    only.  This function is not valid for EP0.

  Precondition:
    None

  Parameters:
    BDT_ENTRY** - pointer to the BDT_ENTRY pointer that you want to be advanced
    to the next buffer state

  Return Values:
    None

  Remarks:
    None

  ***************************************************************************/
#define USBAdvancePingPongBuffer(buffer) ((BYTE_VAL*)buffer)->Val ^= USB_NEXT_PING_PONG;
#define USBHALPingPongSetToOdd(buffer)   {((BYTE_VAL*)buffer)->Val |= USB_NEXT_PING_PONG;}
#define USBHALPingPongSetToEven(buffer)  {((BYTE_VAL*)buffer)->Val &= ~USB_NEXT_PING_PONG;}


/******************************************************************************/
/** External API Functions ****************************************************/
/******************************************************************************/

/**************************************************************************
    Function:
        void USBDeviceInit(void)
    
    Description:
        This function initializes the device stack it in the default state. The
        USB module will be completely reset including all of the internal
        variables, registers, and interrupt flags.
                
    Precondition:
        This function must be called before any of the other USB Device
        functions can be called, including USBDeviceTasks().
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  ***************************************************************************/
void USBDeviceInit(void)
{
    BYTE i;

    USBDisableInterrupts();

    // Clear all USB error flags
    USBClearInterruptRegister(U1EIR);  
       
    // Clears all USB interrupts          
    USBClearInterruptRegister(U1IR); 

    //Clear all of the endpoint control registers
    U1EP0 = 0;
    
    DisableNonZeroEndpoints(USB_MAX_EP_NUMBER);

    SetConfigurationOptions();

    //power up the module (if not already powered)
    USBPowerModule();

    //set the address of the BDT (if applicable)
    USBSetBDTAddress(BDT);

    //Clear all of the BDT entries
    for(i=0;i<(sizeof(BDT)/sizeof(BDT_ENTRY));i++)
    {
        BDT[i].Val = 0x00;
    }

    // Assert reset request to all of the Ping Pong buffer pointers
    USBPingPongBufferReset = 1;                    

    // Reset to default address
    U1ADDR = 0x00;                   

    // Make sure packet processing is enabled
    USBPacketDisable = 0;           

    //Stop trying to reset ping pong buffer pointers
    USBPingPongBufferReset = 0;

    // Flush any pending transactions
    while(USBTransactionCompleteIF == 1)      
    {
        USBClearInterruptFlag(USBTransactionCompleteIFReg,USBTransactionCompleteIFBitNum);
        //Initialize USB stack software state variables
        inPipes[0].info.Val = 0;
        outPipes[0].info.Val = 0;
        outPipes[0].wCount.Val = 0;
    }

    //Set flags to TRUE, so the USBCtrlEPAllowStatusStage() function knows not to
    //try and arm a status stage, even before the first control transfer starts.
    USBStatusStageEnabledFlag1 = TRUE;  
    USBStatusStageEnabledFlag2 = TRUE;
    //Initialize other flags
    USBDeferINDataStagePackets = FALSE;
    USBDeferOUTDataStagePackets = FALSE;
    USBBusIsSuspended = FALSE;

	//Initialize all pBDTEntryIn[] and pBDTEntryOut[]
	//pointers to NULL, so they don't get used inadvertently.  
	for(i = 0; i < (BYTE)(USB_MAX_EP_NUMBER+1u); i++)
	{
		pBDTEntryIn[i] = 0u;
		pBDTEntryOut[i] = 0u;		
		ep_data_in[i].Val = 0u;
        ep_data_out[i].Val = 0u;
	}

    //Get ready for the first packet
    pBDTEntryIn[0] = (volatile BDT_ENTRY*)&BDT[EP0_IN_EVEN];
    // Initialize EP0 as a Ctrl EP
    U1EP0 = EP_CTRL|USB_HANDSHAKE_ENABLED;        
	//Prepare for the first SETUP on EP0 OUT
    BDT[EP0_OUT_EVEN].ADR = ConvertToPhysicalAddress(&SetupPkt);
    BDT[EP0_OUT_EVEN].CNT = USB_EP0_BUFF_SIZE;
    BDT[EP0_OUT_EVEN].STAT.Val = _USIE|_DAT0|_BSTALL;

    // Clear active configuration
    USBActiveConfiguration = 0;     

    //Indicate that we are now in the detached state        
    USBDeviceState = DETACHED_STATE;
}

/**************************************************************************
  Function:
        void USBDeviceTasks(void)
    
  Summary:
    This function is the main state machine/transaction handler of the USB 
    device side stack.  When the USB stack is operated in "USB_POLLING" mode 
    (usb_config.h user option) the USBDeviceTasks() function should be called 
    periodically to receive and transmit packets through the stack. This 
    function also takes care of control transfers associated with the USB 
    enumeration process, and detecting various USB events (such as suspend).  
    This function should be called at least once every 1.8ms during the USB 
    enumeration process. After the enumeration process is complete (which can 
    be determined when USBGetDeviceState() returns CONFIGURED_STATE), the 
    USBDeviceTasks() handler may be called the faster of: either once 
    every 9.8ms, or as often as needed to make sure that the hardware USTAT 
    FIFO never gets full.  A good rule of thumb is to call USBDeviceTasks() at
    a minimum rate of either the frequency that USBTransferOnePacket() gets 
    called, or, once/1.8ms, whichever is faster.  See the inline code comments 
    near the top of usb_device.c for more details about minimum timing 
    requirements when calling USBDeviceTasks().
    
    When the USB stack is operated in "USB_INTERRUPT" mode, it is not necessary
    to call USBDeviceTasks() from the main loop context.  In the USB_INTERRUPT
    mode, the USBDeviceTasks() handler only needs to execute when a USB 
    interrupt occurs, and therefore only needs to be called from the interrupt 
    context.

  Description:
    This function is the main state machine/transaction handler of the USB 
    device side stack.  When the USB stack is operated in "USB_POLLING" mode 
    (usb_config.h user option) the USBDeviceTasks() function should be called 
    periodically to receive and transmit packets through the stack. This 
    function also takes care of control transfers associated with the USB 
    enumeration process, and detecting various USB events (such as suspend).  
    This function should be called at least once every 1.8ms during the USB 
    enumeration process. After the enumeration process is complete (which can 
    be determined when USBGetDeviceState() returns CONFIGURED_STATE), the 
    USBDeviceTasks() handler may be called the faster of: either once 
    every 9.8ms, or as often as needed to make sure that the hardware USTAT 
    FIFO never gets full.  A good rule of thumb is to call USBDeviceTasks() at
    a minimum rate of either the frequency that USBTransferOnePacket() gets 
    called, or, once/1.8ms, whichever is faster.  See the inline code comments 
    near the top of usb_device.c for more details about minimum timing 
    requirements when calling USBDeviceTasks().
    
    When the USB stack is operated in "USB_INTERRUPT" mode, it is not necessary
    to call USBDeviceTasks() from the main loop context.  In the USB_INTERRUPT
    mode, the USBDeviceTasks() handler only needs to execute when a USB 
    interrupt occurs, and therefore only needs to be called from the interrupt 
    context.

    Typical usage:
    <code>
    void main(void)
    {
        USBDeviceInit();
        while(1)
        {
            USBDeviceTasks(); //Takes care of enumeration and other USB events
            if((USBGetDeviceState() \< CONFIGURED_STATE) ||
               (USBIsDeviceSuspended() == TRUE))
            {
                //Either the device is not configured or we are suspended,
                // so we don't want to execute any USB related application code
                continue;   //go back to the top of the while loop
            }
            else
            {
                //Otherwise we are free to run USB and non-USB related user 
                //application code.
                UserApplication();
            }
        }
    }
    </code>

  Precondition:
    Make sure the USBDeviceInit() function has been called prior to calling
    USBDeviceTasks() for the first time.
  Remarks:
    USBDeviceTasks() does not need to be called while in the USB suspend mode, 
    if the user application firmware in the USBCBSuspend() callback function
    enables the ACTVIF USB interrupt source and put the microcontroller into 
    sleep mode.  If the application firmware decides not to sleep the 
    microcontroller core during USB suspend (ex: continues running at full 
    frequency, or clock switches to a lower frequency), then the USBDeviceTasks()
    function must still be called periodically, at a rate frequent enough to 
    ensure the 10ms resume recovery interval USB specification is met.  Assuming
    a worst case primary oscillator and PLL start up time of <5ms, then 
    USBDeviceTasks() should be called once every 5ms in this scenario.
   
    When the USB cable is detached, or the USB host is not actively powering 
    the VBUS line to +5V nominal, the application firmware does not always have 
    to call USBDeviceTasks() frequently, as no USB activity will be taking 
    place.  However, if USBDeviceTasks() is not called regularly, some 
    alternative means of promptly detecting when VBUS is powered (indicating 
    host attachment), or not powered (host powered down or USB cable unplugged)
    is still needed.  For self or dual self/bus powered USB applications, see 
    the USBDeviceAttach() and USBDeviceDetach() API documentation for additional 
    considerations.
                     
  **************************************************************************/

#if defined(USB_INTERRUPT) 
#if defined(__18CXX) || defined (_PIC14E)
    void USBDeviceTasks(void)
  #elif defined(__C30__) || defined __XC16__
    void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
  #elif defined(__PIC32MX__)
    void __attribute__((interrupt(),vector(_USB_1_VECTOR))) _USB1Interrupt( void ) 
  #endif
#else
void USBDeviceTasks(void)
#endif
{
    BYTE i;

#ifdef USB_SUPPORT_OTG
    //SRP Time Out Check
    if (USBOTGSRPIsReady())
    {
        if (USBT1MSECIF && USBT1MSECIE)
        {
            if (USBOTGGetSRPTimeOutFlag())
            {
                if (USBOTGIsSRPTimeOutExpired())
                {
                    USB_OTGEventHandler(0,OTG_EVENT_SRP_FAILED,0,0);
                }       
            }

            //Clear Interrupt Flag
            USBClearInterruptFlag(USBT1MSECIFReg,USBT1MSECIFBitNum);
        }
    }
#endif

    #if defined(USB_POLLING)
    //If the interrupt option is selected then the customer is required
    //  to notify the stack when the device is attached or removed from the
    //  bus by calling the USBDeviceAttach() and USBDeviceDetach() functions.
    if (USB_BUS_SENSE != 1)
    {
         // Disable module & detach from bus
         U1CON = 0;             

         // Mask all USB interrupts              
         U1IE = 0;          

         //Move to the detached state                  
         USBDeviceState = DETACHED_STATE;

         #ifdef  USB_SUPPORT_OTG    
             //Disable D+ Pullup
             U1OTGCONbits.DPPULUP = 0;

             //Disable HNP
             USBOTGDisableHnp();

             //Deactivate HNP
             USBOTGDeactivateHnp();
             
             //If ID Pin Changed State
             if (USBIDIF && USBIDIE)
             {  
                 //Re-detect & Initialize
                  USBOTGInitialize();

                  //Clear ID Interrupt Flag
                  USBClearInterruptFlag(USBIDIFReg,USBIDIFBitNum);
             }
         #endif

         #if defined __C30__ || defined __XC16__
             //USBClearInterruptFlag(U1OTGIR, 3); 
         #endif
            //return so that we don't go through the rest of 
            //the state machine
         USBClearUSBInterrupt();
         return;
    }

	#ifdef USB_SUPPORT_OTG
    //If Session Is Started Then
    else
	{
        //If SRP Is Ready
        if (USBOTGSRPIsReady())
        {   
            //Clear SRPReady
            USBOTGClearSRPReady();

            //Clear SRP Timeout Flag
            USBOTGClearSRPTimeOutFlag();

            //Indicate Session Started
            UART2PrintString( "\r\n***** USB OTG B Event - Session Started  *****\r\n" );
        }
    }
	#endif	//#ifdef USB_SUPPORT_OTG

    //if we are in the detached state
    if(USBDeviceState == DETACHED_STATE)
    {
	    //Initialize register to known value
        U1CON = 0;                          

        // Mask all USB interrupts
        U1IE = 0;                                

        //Enable/set things like: pull ups, full/low-speed mode, 
        //set the ping pong mode, and set internal transceiver
        SetConfigurationOptions();

        // Enable module & attach to bus
        while(!U1CONbits.USBEN){U1CONbits.USBEN = 1;}

        //moved to the attached state
        USBDeviceState = ATTACHED_STATE;

        #ifdef  USB_SUPPORT_OTG
            U1OTGCON |= USB_OTG_DPLUS_ENABLE | USB_OTG_ENABLE;  
        #endif
    }
	#endif  //#if defined(USB_POLLING)

    if(USBDeviceState == ATTACHED_STATE)
    {
        /*
         * After enabling the USB module, it takes some time for the
         * voltage on the D+ or D- line to rise high enough to get out
         * of the SE0 condition. The USB Reset interrupt should not be
         * unmasked until the SE0 condition is cleared. This helps
         * prevent the firmware from misinterpreting this unique event
         * as a USB bus reset from the USB host.
         */

        if(!USBSE0Event)
        {
            USBClearInterruptRegister(U1IR);// Clear all USB interrupts
            #if defined(USB_POLLING)
                U1IE=0;                        // Mask all USB interrupts
            #endif
            USBResetIE = 1;             // Unmask RESET interrupt
            USBIdleIE = 1;             // Unmask IDLE interrupt
            USBDeviceState = POWERED_STATE;
        }
    }

    #ifdef  USB_SUPPORT_OTG
        //If ID Pin Changed State
        if (USBIDIF && USBIDIE)
        {  
            //Re-detect & Initialize
            USBOTGInitialize();

            USBClearInterruptFlag(USBIDIFReg,USBIDIFBitNum);
        }
    #endif

    /*
     * Task A: Service USB Activity Interrupt
     */
    if(USBActivityIF && USBActivityIE)
    {
        USBClearInterruptFlag(USBActivityIFReg,USBActivityIFBitNum);
        #if defined(USB_SUPPORT_OTG)
            U1OTGIR = 0x10;        
        #else
            USBWakeFromSuspend();
        #endif
    }

    /*
     * Pointless to continue servicing if the device is in suspend mode.
     */
    if(USBSuspendControl==1)
    {
        USBClearUSBInterrupt();
        return;
    }

    /*
     * Task B: Service USB Bus Reset Interrupt.
     * When bus reset is received during suspend, ACTVIF will be set first,
     * once the UCONbits.SUSPND is clear, then the URSTIF bit will be asserted.
     * This is why URSTIF is checked after ACTVIF.
     *
     * The USB reset flag is masked when the USB state is in
     * DETACHED_STATE or ATTACHED_STATE, and therefore cannot
     * cause a USB reset event during these two states.
     */
    if(USBResetIF && USBResetIE)
    {
        USBDeviceInit();

        //Re-enable the interrupts since the USBDeviceInit() function will
        //  disable them.  This will do nothing in a polling setup
        USBUnmaskInterrupts();

        USBDeviceState = DEFAULT_STATE;

        #ifdef USB_SUPPORT_OTG
             //Disable HNP
             USBOTGDisableHnp();

             //Deactivate HNP
             USBOTGDeactivateHnp();
        #endif

        USBClearInterruptFlag(USBResetIFReg,USBResetIFBitNum);
    }

    /*
     * Task C: Service other USB interrupts
     */
    if(USBIdleIF && USBIdleIE)
    { 
        #ifdef  USB_SUPPORT_OTG 
            //If Suspended, Try to switch to Host
            USBOTGSelectRole(ROLE_HOST);
        #else
            USBSuspend();
        #endif
        
        USBClearInterruptFlag(USBIdleIFReg,USBIdleIFBitNum);
    }

    if(USBSOFIF)
    {
        if(USBSOFIE)
        {
            USB_SOF_HANDLER(EVENT_SOF,0,1);
        }    
        USBClearInterruptFlag(USBSOFIFReg,USBSOFIFBitNum);
        
        #if defined(USB_ENABLE_STATUS_STAGE_TIMEOUTS)
            //Supporting this feature requires a 1ms timebase for keeping track of the timeout interval.
            #if(USB_SPEED_OPTION == USB_LOW_SPEED)
                #warning "Double click this message.  See inline code comments."
                //The "USB_ENABLE_STATUS_STAGE_TIMEOUTS" feature is optional and is
                //not strictly needed in all applications (ex: those that never call 
                //USBDeferStatusStage() and don't use host to device (OUT) control
                //transfers with data stage).  
                //However, if this feature is enabled and used, it requires a timer 
                //(preferrably 1ms) to decrement the USBStatusStageTimeoutCounter.  
                //In USB Full Speed applications, the host sends Start-of-Frame (SOF) 
                //packets at a 1ms rate, which generates SOFIF interrupts.
                //These interrupts can be used to decrement USBStatusStageTimeoutCounter as shown 
                //below.  However, the host does not send SOF packets to Low Speed devices.  
                //Therefore, some other method  (ex: using a general purpose microcontroller 
                //timer, such as Timer0) needs to be implemented to call and execute the below code
                //at a once/1ms rate, in a low speed USB application.
                //Note: Pre-condition to executing the below code: USBDeviceInit() should have
                //been called at least once (since the last microcontroller reset/power up), 
                //prior to executing the below code.
            #endif
            
            //Decrement our status stage counter.
            if(USBStatusStageTimeoutCounter != 0u)
            {
                USBStatusStageTimeoutCounter--;
            }
            //Check if too much time has elapsed since progress was made in 
            //processing the control transfer, without arming the status stage.  
            //If so, auto-arm the status stage to ensure that the control 
            //transfer can [eventually] complete, within the timing limits
            //dictated by section 9.2.6 of the official USB 2.0 specifications.
            if(USBStatusStageTimeoutCounter == 0)
            {
                USBCtrlEPAllowStatusStage();    //Does nothing if the status stage was already armed.
            } 
        #endif
    }

    if(USBStallIF && USBStallIE)
    {
        USBStallHandler();
    }

    if(USBErrorIF && USBErrorIE)
    {
        USB_ERROR_HANDLER(EVENT_BUS_ERROR,0,1);
        USBClearInterruptRegister(U1EIR);               // This clears UERRIF

        //On PIC18, clearing the source of the error will automatically clear
        //  the interrupt flag.  On other devices the interrupt flag must be 
        //  manually cleared. 
        #if defined(__C32__) || defined(__C30__) || defined __XC16__
            USBClearInterruptFlag( USBErrorIFReg, USBErrorIFBitNum );
        #endif
    }

    /*
     * Pointless to continue servicing if the host has not sent a bus reset.
     * Once bus reset is received, the device transitions into the DEFAULT
     * state and is ready for communication.
     */
    if(USBDeviceState < DEFAULT_STATE)
    {
	    USBClearUSBInterrupt();
	    return; 
	}  

    /*
     * Task D: Servicing USB Transaction Complete Interrupt
     */
    if(USBTransactionCompleteIE)
    {
	    for(i = 0; i < 4u; i++)	//Drain or deplete the USAT FIFO entries.  If the USB FIFO ever gets full, USB bandwidth 
		{						//utilization can be compromised, and the device won't be able to receive SETUP packets.
		    if(USBTransactionCompleteIF)
		    {
    		    //Save and extract USTAT register info.  Will use this info later.
                USTATcopy.Val = U1STAT;
                endpoint_number = USBHALGetLastEndpoint(USTATcopy);
                
                USBClearInterruptFlag(USBTransactionCompleteIFReg,USBTransactionCompleteIFBitNum);
                
                //Keep track of the hardware ping pong state for endpoints other
                //than EP0, if ping pong buffering is enabled.
                #if (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0) || (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG) 
                    if(USBHALGetLastDirection(USTATcopy) == OUT_FROM_HOST)
                    {
                        ep_data_out[endpoint_number].bits.ping_pong_state ^= 1;
                    }   
                    else
                    {
                        ep_data_in[endpoint_number].bits.ping_pong_state ^= 1;
                    }         
                #endif    
                
                //USBCtrlEPService only services transactions over EP0.
                //It ignores all other EP transactions.
                if(endpoint_number == 0)
                {
                    USBCtrlEPService();
                }
                else
                {
                    USB_TRANSFER_COMPLETE_HANDLER(EVENT_TRANSFER, (BYTE*)&USTATcopy.Val, 0);
                }
		    }//end if(USBTransactionCompleteIF)
		    else
		    	break;	//USTAT FIFO must be empty.
		}//end for()
	}//end if(USBTransactionCompleteIE)   

    USBClearUSBInterrupt();
}//end of USBDeviceTasks()

/*******************************************************************************
  Function:
        void USBEnableEndpoint(BYTE ep, BYTE options)
    
  Summary:
    This function will enable the specified endpoint with the specified
    options
  Description:
    This function will enable the specified endpoint with the specified
    options.
    
    Typical Usage:
    <code>
    void USBCBInitEP(void)
    {
        USBEnableEndpoint(MSD_DATA_IN_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
        USBMSDInit();
    }
    </code>
    
    In the above example endpoint number MSD_DATA_IN_EP is being configured
    for both IN and OUT traffic with handshaking enabled. Also since
    MSD_DATA_IN_EP is not endpoint 0 (MSD does not allow this), then we can
    explicitly disable SETUP packets on this endpoint.
  Conditions:
    None
  Input:
    BYTE ep -       the endpoint to be configured
    BYTE options -  optional settings for the endpoint. The options should
                    be ORed together to form a single options string. The
                    available optional settings for the endpoint. The
                    options should be ORed together to form a single options
                    string. The available options are the following\:
                    * USB_HANDSHAKE_ENABLED enables USB handshaking (ACK,
                      NAK)
                    * USB_HANDSHAKE_DISABLED disables USB handshaking (ACK,
                      NAK)
                    * USB_OUT_ENABLED enables the out direction
                    * USB_OUT_DISABLED disables the out direction
                    * USB_IN_ENABLED enables the in direction
                    * USB_IN_DISABLED disables the in direction
                    * USB_ALLOW_SETUP enables control transfers
                    * USB_DISALLOW_SETUP disables control transfers
                    * USB_STALL_ENDPOINT STALLs this endpoint
  Return:
    None
  Remarks:
    None                                                                                                          
  *****************************************************************************/
void USBEnableEndpoint(BYTE ep, BYTE options)
{
    unsigned char* p;
        
    //Use USBConfigureEndpoint() to set up the pBDTEntryIn/Out[ep] pointer and 
    //starting DTS state in the BDT entry.
    if(options & USB_OUT_ENABLED)
    {
        USBConfigureEndpoint(ep, OUT_FROM_HOST);
    }
    if(options & USB_IN_ENABLED)
    {
        USBConfigureEndpoint(ep, IN_TO_HOST);
    }

    //Update the relevant UEPx register to actually enable the endpoint with
    //the specified options (ex: handshaking enabled, control transfers allowed,
    //etc.)
    #if defined(__C32__)
        p = (unsigned char*)(&U1EP0+(4*ep));
    #else
        p = (unsigned char*)(&U1EP0+ep);
    #endif
    *p = options;
}


/*************************************************************************
  Function:
    USB_HANDLE USBTransferOnePacket(BYTE ep, BYTE dir, BYTE* data, BYTE len)
    
  Summary:
    Transfers a single packet (one transaction) of data on the USB bus.

  Description:
    The USBTransferOnePacket() function prepares a USB endpoint
    so that it may send data to the host (an IN transaction), or 
    receive data from the host (an OUT transaction).  The 
    USBTransferOnePacket() function can be used both to receive	and 
    send data to the host.  This function is the primary API function 
    provided by the USB stack firmware for sending or receiving application 
    data over the USB port.  

    The USBTransferOnePacket() is intended for use with all application 
    endpoints.  It is not used for sending or receiving applicaiton data 
    through endpoint 0 by using control transfers.  Separate API 
    functions, such as USBEP0Receive(), USBEP0SendRAMPtr(), and
    USBEP0SendROMPtr() are provided for this purpose.

    The	USBTransferOnePacket() writes to the Buffer Descriptor Table (BDT)
    entry associated with an endpoint buffer, and sets the UOWN bit, which 
    prepares the USB hardware to allow the transaction to complete.  The 
    application firmware can use the USBHandleBusy() macro to check the 
    status of the transaction, to see if the data has been successfully 
    transmitted yet.


    Typical Usage
    <code>
    //make sure that the we are in the configured state
    if(USBGetDeviceState() == CONFIGURED_STATE)
    {
        //make sure that the last transaction isn't busy by checking the handle
        if(!USBHandleBusy(USBInHandle))
        {
	        //Write the new data that we wish to send to the host to the INPacket[] array
	        INPacket[0] = USEFUL_APPLICATION_VALUE1;
	        INPacket[1] = USEFUL_APPLICATION_VALUE2;
	        //INPacket[2] = ... (fill in the rest of the packet data)
	      
            //Send the data contained in the INPacket[] array through endpoint "EP_NUM"
            USBInHandle = USBTransferOnePacket(EP_NUM,IN_TO_HOST,(BYTE*)&INPacket[0],sizeof(INPacket));
        }
    }
    </code>

  Conditions:
    Before calling USBTransferOnePacket(), the following should be true.
    1.  The USB stack has already been initialized (USBDeviceInit() was called).
    2.  A transaction is not already pending on the specified endpoint.  This
        is done by checking the previous request using the USBHandleBusy() 
        macro (see the typical usage example).
    3.  The host has already sent a set configuration request and the 
        enumeration process is complete.
        This can be checked by verifying that the USBGetDeviceState() 
        macro returns "CONFIGURED_STATE", prior to calling 
        USBTransferOnePacket().
 					
  Input:
    BYTE ep - The endpoint number that the data will be transmitted or 
	          received on
    BYTE dir - The direction of the transfer
               This value is either OUT_FROM_HOST or IN_TO_HOST
    BYTE* data - For IN transactions: pointer to the RAM buffer containing 
                 the data to be sent to the host.  For OUT transactions: pointer
                 to the RAM buffer that the received data should get written to.
   BYTE len - Length of the data needing to be sent (for IN transactions).
              For OUT transactions, the len parameter should normally be set
              to the endpoint size specified in the endpoint descriptor.    

  Return Values:
    USB_HANDLE - handle to the transfer.  The handle is a pointer to 
                 the BDT entry associated with this transaction.  The
                 status of the transaction (ex: if it is complete or still
                 pending) can be checked using the USBHandleBusy() macro
                 and supplying the USB_HANDLE provided by
                 USBTransferOnePacket().

  Remarks:
    If calling the USBTransferOnePacket() function from within the USBCBInitEP()
    callback function, the set configuration is still being processed and the
    USBDeviceState may not be == CONFIGURED_STATE yet.  In this	special case, 
    the USBTransferOnePacket() may still be called, but make sure that the 
    endpoint has been enabled and initialized by the USBEnableEndpoint() 
    function first.  
    
  *************************************************************************/
USB_HANDLE USBTransferOnePacket(BYTE ep,BYTE dir,BYTE* data,BYTE len)
{
    volatile BDT_ENTRY* handle;

    //If the direction is IN
    if(dir != 0)
    {
        //point to the IN BDT of the specified endpoint
        handle = pBDTEntryIn[ep];
    }
    else
    {
        //else point to the OUT BDT of the specified endpoint
        handle = pBDTEntryOut[ep];
    }
    
    //Error checking code.  Make sure the handle (pBDTEntryIn[ep] or
    //pBDTEntryOut[ep]) is initialized before using it.
    if(handle == 0)
    {
	    return 0;
	}

    //Toggle the DTS bit if required
    #if (USB_PING_PONG_MODE == USB_PING_PONG__NO_PING_PONG)
        handle->STAT.Val ^= _DTSMASK;
    #elif (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY)
        if(ep != 0)
        {
            handle->STAT.Val ^= _DTSMASK;
        }
    #endif

    //Set the data pointer, data length, and enable the endpoint
    handle->ADR = ConvertToPhysicalAddress(data);
    handle->CNT = len;
    handle->STAT.Val &= _DTSMASK;
    handle->STAT.Val |= _USIE | (_DTSEN & _DTS_CHECKING_ENABLED);

    //Point to the next buffer for ping pong purposes.
    if(dir != OUT_FROM_HOST)
    {
        //toggle over the to the next buffer for an IN endpoint
        USBAdvancePingPongBuffer(&pBDTEntryIn[ep]);      
    }
    else
    {
        //toggle over the to the next buffer for an OUT endpoint
        USBAdvancePingPongBuffer(&pBDTEntryOut[ep]);     
    }
    return (USB_HANDLE)handle;
}


/********************************************************************
    Function:
        void USBStallEndpoint(BYTE ep, BYTE dir)
        
    Summary:
         Configures the specified endpoint to send STALL to the host, the next
         time the host tries to access the endpoint.
    
    PreCondition:
        None
        
    Parameters:
        BYTE ep - The endpoint number that should be configured to send STALL.
        BYTE dir - The direction of the endpoint to STALL, either 
                   IN_TO_HOST or OUT_FROM_HOST.
        
    Return Values:
        None
        
    Remarks:
        None

 *******************************************************************/
void USBStallEndpoint(BYTE ep, BYTE dir)
{
    BDT_ENTRY *p;

    if(ep == 0)
    {
        //For control endpoints (ex: EP0), we need to STALL both IN and OUT
        //endpoints.  EP0 OUT must also be prepared to receive the next SETUP 
        //packet that will arrrive.
        pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
        pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&SetupPkt);
        pBDTEntryEP0OutNext->STAT.Val = _USIE|_DAT0|(_DTSEN & _DTS_CHECKING_ENABLED)|_BSTALL;
        pBDTEntryIn[0]->STAT.Val = _USIE|_BSTALL; 
               
    }
    else
    {
        p = (BDT_ENTRY*)(&BDT[EP(ep,dir,0)]);
        p->STAT.Val |= _BSTALL | _USIE;
    
        //If the device is in FULL or ALL_BUT_EP0 ping pong modes
        //then stall that entry as well
        #if (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG) || (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0)
    
        p = (BDT_ENTRY*)(&BDT[EP(ep,dir,1)]);
        p->STAT.Val |= _BSTALL | _USIE;
        #endif
    }
}

/**************************************************************************
    Function:
        void USBCancelIO(BYTE endpoint)
    
    Description:
        This function cancels the transfers pending on the specified endpoint.
        This function can only be used after a SETUP packet is received and 
        before that setup packet is handled.  This is the time period in which
        the EVENT_EP0_REQUEST is thrown, before the event handler function
        returns to the stack.

    Precondition:
  
    Parameters:
        BYTE endpoint - the endpoint number you wish to cancel the transfers for
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void USBCancelIO(BYTE endpoint)
{
    if(USBPacketDisable == 1)
    {
    	//The PKTDIS bit is currently set right now.  It is therefore "safe"
    	//to mess with the BDT right now.
    	pBDTEntryIn[endpoint]->Val &= _DTSMASK;	//Makes UOWN = 0 (_UCPU mode).  Deactivates endpoint.  Only sends NAKs.
    	pBDTEntryIn[endpoint]->Val ^= _DTSMASK;	//Toggle the DTS bit.  This packet didn't get sent yet, and the next call to USBTransferOnePacket() will re-toggle the DTS bit back to the original (correct) value.
    	
    	//Need to do additional handling if ping-pong buffering is being used
        #if ((USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG) || (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0))
        //Point to the next buffer for ping pong purposes.  UOWN getting cleared
        //(either due to SIE clearing it after a transaction, or the firmware
        //clearing it) makes hardware ping pong pointer advance.
        USBAdvancePingPongBuffer(&pBDTEntryIn[endpoint]);       
    
    	pBDTEntryIn[endpoint]->STAT.Val &= _DTSMASK;
    	pBDTEntryIn[endpoint]->STAT.Val ^= _DTSMASK;
        #endif
    }
}

/**************************************************************************
    Function:
        void USBDeviceDetach(void)
   
    Summary:
        This function configures the USB module to "soft detach" itself from
        the USB host.
        
    Description:
        This function configures the USB module to perform a "soft detach"
        operation, by disabling the D+ (or D-) ~1.5k pull up resistor, which
        lets the host know the device is present and attached.  This will make
        the host think that the device has been unplugged.  This is potentially
        useful, as it allows the USB device to force the host to re-enumerate
        the device (on the firmware has re-enabled the USB module/pull up, by
        calling USBDeviceAttach(), to "soft re-attach" to the host).
        
    Precondition:
        Should only be called when USB_INTERRUPT is defined.  See remarks
        section if USB_POLLING mode option is being used (usb_config.h option).

        Additionally, this function should only be called from the main() loop 
        context.  Do not call this function from within an interrupt handler, as 
        this function may modify global interrupt enable bits and settings.
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        If the application firmware calls USBDeviceDetach(), it is strongly
        recommended that the firmware wait at least >= 80ms before calling
        USBDeviceAttach().  If the firmeware performs a soft detach, and then
        re-attaches too soon (ex: after a few micro seconds for instance), some
        hosts may interpret this as an unexpected "glitch" rather than as a
        physical removal/re-attachment of the USB device.  In this case the host
        may simply ignore the event without re-enumerating the device.  To 
        ensure that the host properly detects and processes the device soft
        detach/re-attach, it is recommended to make sure the device remains 
        detached long enough to mimic a real human controlled USB 
        unplug/re-attach event (ex: after calling USBDeviceDetach(), do not
        call USBDeviceAttach() for at least 80+ms, preferrably longer.
        
        Neither the USBDeviceDetach() or USBDeviceAttach() functions are blocking
        or take long to execute.  It is the application firmware's 
        responsibility for adding the 80+ms delay, when using these API 
        functions.
        
        Note: The Windows plug and play event handler processing is fairly 
        slow, especially in certain versions of Windows, and for certain USB
        device classes.  It has been observed that some device classes need to
        provide even more USB detach dwell interval (before calling 
        USBDeviceAttach()), in order to work correctly after re-enumeration.
        If the USB device is a CDC class device, it is recommended to wait
        at least 1.5 seconds or longer, before soft re-attaching to the host,
        to provide the plug and play event handler enough time to finish 
        processing the removal event, before the re-attach occurs.
        
        If the application is using the USB_POLLING mode option, then the 
        USBDeviceDetach() and USBDeviceAttach() functions are not available.  
        In this mode, the USB stack relies on the "#define USE_USB_BUS_SENSE_IO" 
        and "#define USB_BUS_SENSE" options in the 
        HardwareProfile – [platform name].h file. 

        When using the USB_POLLING mode option, and the 
        "#define USE_USB_BUS_SENSE_IO" definition has been commented out, then 
        the USB stack assumes that it should always enable the USB module at 
        pretty much all times.  Basically, anytime the application firmware 
        calls USBDeviceTasks(), the firmware will automatically enable the USB 
        module.  This mode would typically be selected if the application was 
        designed to be a purely bus powered device.  In this case, the 
        application is powered from the +5V VBUS supply from the USB port, so 
        it is correct and sensible in this type of application to power up and 
        turn on the USB module, at anytime that the microcontroller is 
        powered (which implies the USB cable is attached and the host is also 
        powered).

        In a self powered application, the USB stack is designed with the 
        intention that the user will enable the "#define USE_USB_BUS_SENSE_IO" 
        option in the HardwareProfile – [platform name].h file.  When this 
        option is defined, then the USBDeviceTasks() function will automatically 
        check the I/O pin port value of the designated pin (based on the 
        #define USB_BUS_SENSE option in the HardwareProfile – [platform name].h 
        file), every time the application calls USBDeviceTasks().  If the 
        USBDeviceTasks() function is executed and finds that the pin defined by 
        the #define USB_BUS_SENSE is in a logic low state, then it will 
        automatically disable the USB module and tri-state the D+ and D- pins.  
        If however the USBDeviceTasks() function is executed and finds the pin 
        defined by the #define USB_BUS_SENSE is in a logic high state, then it 
        will automatically enable the USB module, if it has not already been 
        enabled.        
                                                          
  **************************************************************************/
#if defined(USB_INTERRUPT)
void USBDeviceDetach(void)
{
    //If the interrupt option is selected then the customer is required
    //  to notify the stack when the device is attached or removed from the
    //  bus by calling the USBDeviceAttach() and USBDeviceDetach() functions.
#ifdef USB_SUPPORT_OTG
    if (USB_BUS_SENSE != 1)
#endif
    {
         // Disable module & detach from bus
         U1CON = 0;             

         // Mask all USB interrupts              
         U1IE = 0;          

         //Move to the detached state                  
         USBDeviceState = DETACHED_STATE;

         #ifdef  USB_SUPPORT_OTG    
             //Disable D+ Pullup
             U1OTGCONbits.DPPULUP = 0;

             //Disable HNP
             USBOTGDisableHnp();

             //Deactivate HNP
             USBOTGDeactivateHnp();
             
             //If ID Pin Changed State
             if (USBIDIF && USBIDIE)
             {  
                 //Re-detect & Initialize
                  USBOTGInitialize();

                  //Clear ID Interrupt Flag
                  USBClearInterruptFlag(USBIDIFReg,USBIDIFBitNum);
             }
         #endif

         #if defined __C30__ || defined __XC16__
             //USBClearInterruptFlag(U1OTGIR, 3); 
         #endif
            //return so that we don't go through the rest of 
            //the state machine
          return;
    }

#ifdef USB_SUPPORT_OTG
    //If Session Is Started Then
   else
   {
        //If SRP Is Ready
        if (USBOTGSRPIsReady())
        {   
            //Clear SRPReady
            USBOTGClearSRPReady();

            //Clear SRP Timeout Flag
            USBOTGClearSRPTimeOutFlag();

            //Indicate Session Started
            UART2PrintString( "\r\n***** USB OTG B Event - Session Started  *****\r\n" );
        }
    }
#endif
}
#endif  //#if defined(USB_INTERRUPT)
/**************************************************************************
    Function:
        void USBDeviceAttach(void)
    
    Summary:
        Checks if VBUS is present, and that the USB module is not already 
        initalized, and if so, enables the USB module so as to signal device 
        attachment to the USB host.   

    Description:
        This function indicates to the USB host that the USB device has been
        attached to the bus.  This function needs to be called in order for the
        device to start to enumerate on the bus.
                
    Precondition:
        Should only be called when USB_INTERRUPT is defined.  Also, should only 
        be called from the main() loop context.  Do not call USBDeviceAttach()
        from within an interrupt handler, as the USBDeviceAttach() function
        may modify global interrupt enable bits and settings.

        For normal USB devices:
        Make sure that if the module was previously on, that it has been turned off 
        for a long time (ex: 100ms+) before calling this function to re-enable the module.
        If the device turns off the D+ (for full speed) or D- (for low speed) ~1.5k ohm
        pull up resistor, and then turns it back on very quickly, common hosts will sometimes 
        reject this event, since no human could ever unplug and reattach a USB device in a 
        microseconds (or nanoseconds) timescale.  The host could simply treat this as some kind 
        of glitch and ignore the event altogether.  
    Parameters:
        None
     
    Return Values:
        None       
    
    Remarks: 
		See also the USBDeviceDetach() API function documentation.                                                 
****************************************************************************/
#if defined(USB_INTERRUPT)
void USBDeviceAttach(void)
{
    //if we are in the detached state
    if(USBDeviceState == DETACHED_STATE)
    {
        if(USB_BUS_SENSE == 1)
        {
    	    //Initialize registers to known states.
            U1CON = 0;          
    
            // Mask all USB interrupts
            U1IE = 0;                                
    
            //Configure things like: pull ups, full/low-speed mode, 
            //set the ping pong mode, and set internal transceiver
            SetConfigurationOptions();
    
            USBEnableInterrupts();  //Modifies global interrupt settings
    
            // Enable module & attach to bus
            while(!U1CONbits.USBEN){U1CONbits.USBEN = 1;}
    
            //moved to the attached state
            USBDeviceState = ATTACHED_STATE;
    
            #ifdef  USB_SUPPORT_OTG
                U1OTGCON = USB_OTG_DPLUS_ENABLE | USB_OTG_ENABLE;  
            #endif
        }
    }
}
#endif  //#if defined(USB_INTERRUPT)


/*******************************************************************************
  Function: void USBCtrlEPAllowStatusStage(void);
    
  Summary: This function prepares the proper endpoint 0 IN or endpoint 0 OUT 
            (based on the controlTransferState) to allow the status stage packet
            of a control transfer to complete.  This function gets used 
            internally by the USB stack itself, but it may also be called from
            the application firmware, IF the application firmware called
            the USBDeferStatusStage() function during the initial processing
            of the control transfer request.  In this case, the application
            must call the USBCtrlEPAllowStatusStage() once, after it has fully
            completed processing and handling the data stage portion of the
            request.  
            
            If the application firmware has no need for delaying control 
            transfers, and therefore never calls USBDeferStatusStage(), then the
            application firmware should not call USBCtrlEPAllowStatusStage().
            
  Description:
    
  Conditions:
    None

  Input:

  Return:

  Remarks:
    None                                                                                                          
  *****************************************************************************/
void USBCtrlEPAllowStatusStage(void)
{
    //Check and set two flags, prior to actually modifying any BDT entries.
    //This double checking is necessary to make certain that 
    //USBCtrlEPAllowStatusStage() can be called twice simultaneously (ex: once 
    //in main loop context, while simultaneously getting an interrupt which 
    //tries to call USBCtrlEPAllowStatusStage() again, at the same time).
    if(USBStatusStageEnabledFlag1 == FALSE)
    {
        USBStatusStageEnabledFlag1 = TRUE;  
        if(USBStatusStageEnabledFlag2 == FALSE)
        {
            USBStatusStageEnabledFlag2 = TRUE;
        
            //Determine which endpoints (EP0 IN or OUT needs arming for the status
            //stage), based on the type of control transfer currently pending.
            if(controlTransferState == CTRL_TRF_RX)
            {
                pBDTEntryIn[0]->CNT = 0;
                pBDTEntryIn[0]->STAT.Val = _USIE|_DAT1|(_DTSEN & _DTS_CHECKING_ENABLED);        
            }
            else if(controlTransferState == CTRL_TRF_TX)
            {
        		BothEP0OutUOWNsSet = FALSE;	//Indicator flag used in USBCtrlTrfOutHandler()
        
                //This buffer (when ping pong buffering is enabled on EP0 OUT) receives the
                //next SETUP packet.
        		#if((USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY) || (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG))
        		pBDTEntryEP0OutCurrent->CNT = USB_EP0_BUFF_SIZE;
        		pBDTEntryEP0OutCurrent->ADR = ConvertToPhysicalAddress(&SetupPkt);
        		pBDTEntryEP0OutCurrent->STAT.Val = _USIE|_BSTALL; //Prepare endpoint to accept a SETUP transaction
        		BothEP0OutUOWNsSet = TRUE;	//Indicator flag used in USBCtrlTrfOutHandler()
        		#endif
        
                //This EP0 OUT buffer receives the 0-byte OUT status stage packet.
        		pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
        		pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&SetupPkt);
        		pBDTEntryEP0OutNext->STAT.Val = _USIE;           // Note: DTSEN is 0
            }
        }    
    }
}   


/*******************************************************************************
  Function: void USBCtrlEPAllowDataStage(void);
    
  Summary: This function allows the data stage of either a host-to-device or
            device-to-host control transfer (with data stage) to complete.
            This function is meant to be used in conjunction with either the
            USBDeferOUTDataStage() or USBDeferINDataStage().  If the firmware
            does not call either USBDeferOUTDataStage() or USBDeferINDataStage(),
            then the firmware does not need to manually call 
            USBCtrlEPAllowDataStage(), as the USB stack will call this function
            instead.
     
  Description:
    
  Conditions: A control transfer (with data stage) should already be pending, 
                if the firmware calls this function.  Additionally, the firmware
                should have called either USBDeferOUTDataStage() or 
                USBDeferINDataStage() at the start of the control transfer, if
                the firmware will be calling this function manually.

  Input:

  Return:

  Remarks: 
  *****************************************************************************/
void USBCtrlEPAllowDataStage(void)
{
    USBDeferINDataStagePackets = FALSE;
    USBDeferOUTDataStagePackets = FALSE;

    if(controlTransferState == CTRL_TRF_RX) //(<setup><out><out>...<out><in>)
    {
        //Prepare EP0 OUT to receive the first OUT data packet in the data stage sequence.
        pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
        pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&CtrlTrfData);
        pBDTEntryEP0OutNext->STAT.Val = _USIE|_DAT1|(_DTSEN & _DTS_CHECKING_ENABLED);
    }   
    else    //else must be controlTransferState == CTRL_TRF_TX (<setup><in><in>...<in><out>)
    {
        //Error check the data stage byte count.  Make sure the user specified
        //value was no greater than the number of bytes the host requested.
		if(SetupPkt.wLength < inPipes[0].wCount.Val)
		{
			inPipes[0].wCount.Val = SetupPkt.wLength;
		}
		USBCtrlTrfTxService();  //Copies one IN data packet worth of data from application buffer
		                        //to CtrlTrfData buffer.  Also keeps track of how many bytes remaining.

	    //Cnt should have been initialized by responsible request owner (ex: by
	    //using the USBEP0SendRAMPtr() or USBEP0SendROMPtr() API function).
		pBDTEntryIn[0]->ADR = ConvertToPhysicalAddress(&CtrlTrfData);
		pBDTEntryIn[0]->STAT.Val = _USIE|_DAT1|(_DTSEN & _DTS_CHECKING_ENABLED);
    }     
}    


/******************************************************************************/
/** Internal Functions *********************************************************/
/******************************************************************************/

/********************************************************************
 * Function:        void USBConfigureEndpoint(BYTE EPNum, BYTE direction)
 *
 * PreCondition:    None
 *
 * Input:           BYTE EPNum - the endpoint to be configured
 *                  BYTE direction - the direction to be configured
 *                                   (either OUT_FROM_HOST or IN_TO_HOST)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function will configure the specified 
 *                  endpoint
 *
 * Note:            None
 *******************************************************************/
static void USBConfigureEndpoint(BYTE EPNum, BYTE direction)
{
    volatile BDT_ENTRY* handle;

    //Compute a pointer to the even BDT entry corresponding to the
    //EPNum and direction values passed to this function.
    handle = (volatile BDT_ENTRY*)&BDT[EP0_OUT_EVEN]; //Get address of start of BDT
    handle += EP(EPNum,direction,0u);     //Add in offset to the BDT of interest
    
    handle->STAT.UOWN = 0;  //mostly redundant, since USBStdSetCfgHandler() 
    //already cleared the entire BDT table

    //Make sure our pBDTEntryIn/Out[] pointer is initialized.  Needed later
    //for USBTransferOnePacket() API calls.
    if(direction == OUT_FROM_HOST)
    {
        pBDTEntryOut[EPNum] = handle;
    }
    else
    {
        pBDTEntryIn[EPNum] = handle;
    }

    #if (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
        handle->STAT.DTS = 0;
        (handle+1)->STAT.DTS = 1;
    #elif (USB_PING_PONG_MODE == USB_PING_PONG__NO_PING_PONG)
        //Set DTS to one because the first thing we will do
        //when transmitting is toggle the bit
        handle->STAT.DTS = 1;
    #elif (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY)
        if(EPNum != 0)
        {
            handle->STAT.DTS = 1;
        }
    #elif (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0)    
        if(EPNum != 0)
        {
            handle->STAT.DTS = 0;
            (handle+1)->STAT.DTS = 1;
        }
    #endif
}


/******************************************************************************
 * Function:        void USBCtrlEPServiceComplete(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine wrap up the remaining tasks in servicing
 *                  a Setup Request. Its main task is to set the endpoint
 *                  controls appropriately for a given situation. See code
 *                  below.
 *                  There are three main scenarios:
 *                  a) There was no handler for the Request, in this case
 *                     a STALL should be sent out.
 *                  b) The host has requested a read control transfer,
 *                     endpoints are required to be setup in a specific way.
 *                  c) The host has requested a write control transfer, or
 *                     a control data stage is not required, endpoints are
 *                     required to be setup in a specific way.
 *
 *                  Packet processing is resumed by clearing PKTDIS bit.
 *
 * Note:            None
 *****************************************************************************/
static void USBCtrlEPServiceComplete(void)
{
    /*
     * PKTDIS bit is set when a Setup Transaction is received.
     * Clear to resume packet processing.
     */
    USBPacketDisable = 0;

	//Check the busy bits and the SetupPtk.DataDir variables to determine what type of
	//control transfer is currently in progress.  We need to know the type of control
	//transfer that is currently pending, in order to know how to properly arm the 
	//EP0 IN and EP0 OUT endpoints.
    if(inPipes[0].info.bits.busy == 0)
    {
        if(outPipes[0].info.bits.busy == 1)
        {
            controlTransferState = CTRL_TRF_RX;
            /*
             * Control Write:
             * <SETUP[0]><OUT[1]><OUT[0]>...<IN[1]> | <SETUP[0]>
             */

            //1. Prepare OUT EP to receive data, unless a USB class request handler
            //   function decided to defer the data stage (ex: because the intended
            //   RAM buffer wasn't available yet) by calling USBDeferDataStage().
            //   If it did so, it is then responsible for calling USBCtrlEPAllowDataStage(),
            //   once it is ready to begin receiving the data.
            if(USBDeferOUTDataStagePackets == FALSE)
            {
                USBCtrlEPAllowDataStage();
            }
            
            //2.  IN endpoint 0 status stage will be armed by USBCtrlEPAllowStatusStage() 
            //after all of the OUT data has been received and consumed, or if a timeout occurs.
            USBStatusStageEnabledFlag2 = FALSE;
            USBStatusStageEnabledFlag1 = FALSE;
        }
        else
        {
            /*
             * If no one knows how to service this request then stall.
             * Must also prepare EP0 to receive the next SETUP transaction.
             */
            pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
            pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&SetupPkt);
            pBDTEntryEP0OutNext->STAT.Val = _USIE|_DAT0|(_DTSEN & _DTS_CHECKING_ENABLED)|_BSTALL;
            pBDTEntryIn[0]->STAT.Val = _USIE|_BSTALL; 
        }
    }
    else    // A module has claimed ownership of the control transfer session.
    {
		if(SetupPkt.DataDir == USB_SETUP_DEVICE_TO_HOST_BITFIELD)
		{
			controlTransferState = CTRL_TRF_TX;
			/*
			 * Control Read:
			 * <SETUP[0]><IN[1]><IN[0]>...<OUT[1]> | <SETUP[0]>
			 *
			 * 1. Prepare IN EP to transfer data to the host.  If however the data
			 *    wasn't ready yet (ex: because the firmware needs to go and read it from
			 *    some slow/currently unavailable resource, such as an external I2C EEPROM),
			 *    Then the class request handler reponsible should call the USBDeferDataStage()
			 *    macro.  In this case, the firmware may wait up to 500ms, before it is required
			 *    to transmit the first IN data packet.  Once the data is ready, and the firmware
			 *    is ready to begin sending the data, it should then call the 
			 *    USBCtrlEPAllowDataStage() function to start the data stage.
			 */
			if(USBDeferINDataStagePackets == FALSE)
            {
                USBCtrlEPAllowDataStage();
			}

            // 2. (Optionally) allow the status stage now, to prepare for early termination.
            //    Note: If a class request handler decided to set USBDeferStatusStagePacket == TRUE,
            //    then it is responsible for eventually calling USBCtrlEPAllowStatusStage() once it
            //    is ready.  If the class request handler does this, it needs to be careful to
            //    be written so that it can handle the early termination scenario.
            //    Ex: It should call USBCtrlEPAllowStatusStage() when any of the following occurs:
            //    1.  The desired total number of bytes were sent to the host.
            //    2.  The number of bytes that the host originally requested (in the SETUP packet that 
            //        started the control transfer) has been reached.
            //    3.  Or, if a timeout occurs (ex: <50ms since the last successful EP0 IN transaction), regardless 
            //        of how many bytes have actually been sent.  This is necessary to prevent a deadlock situation
            //        (where the control transfer can't complete, due to continuous NAK on status stage) if the
            //        host performs early termination.  If enabled, the USB_ENABLE_STATUS_STAGE_TIMEOUTS usb_config.h
            //        option can take care of this for you.
            //    Note: For this type of control transfer, there is normally no harm in simply arming the
            //    status stage packet right now, even if the IN data is not ready yet.  This allows for
            //    immediate early termination, without adding unecessary delay.  Therefore, it is generally not
            //    recommended for the USB class handler firmware to call USBDeferStatusStage(), for this 
            //    type of control transfer.  If the USB class handler firmware needs more time to fetch the IN
            //    data that needs to be sent to the host, it should instead use the USBDeferDataStage() function.
            USBStatusStageEnabledFlag2 = FALSE;
            USBStatusStageEnabledFlag1 = FALSE;
            if(USBDeferStatusStagePacket == FALSE)
            {
                USBCtrlEPAllowStatusStage();
            } 
		}
		else   // (SetupPkt.DataDir == USB_SETUP_DIRECTION_HOST_TO_DEVICE)
		{
			//This situation occurs for special types of control transfers,
			//such as that which occurs when the host sends a SET_ADDRESS
			//control transfer.  Ex:
			//
			//<SETUP[0]><IN[1]> | <SETUP[0]>
				
			//Although the data direction is HOST_TO_DEVICE, there is no data stage
			//(hence: outPipes[0].info.bits.busy == 0).  There is however still
			//an IN status stage.

			controlTransferState = CTRL_TRF_RX;     //Since this is a HOST_TO_DEVICE control transfer
			
			//1. Prepare OUT EP to receive the next SETUP packet.
			pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
			pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&SetupPkt);
			pBDTEntryEP0OutNext->STAT.Val = _USIE|_BSTALL;
				
			//2. Prepare for IN status stage of the control transfer
            USBStatusStageEnabledFlag2 = FALSE;
            USBStatusStageEnabledFlag1 = FALSE;
			if(USBDeferStatusStagePacket == FALSE)
            {
                USBCtrlEPAllowStatusStage();
            } 
		}

    }//end if(ctrl_trf_session_owner == MUID_NULL)

}//end USBCtrlEPServiceComplete


/******************************************************************************
 * Function:        void USBCtrlTrfTxService(void)
 *
 * PreCondition:    pSrc, wCount, and usb_stat.ctrl_trf_mem are setup properly.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine is used for device to host control transfers 
 *					(IN transactions).  This function takes care of managing a
 *                  transfer over multiple USB transactions.
 *					This routine should be called from only two places.
 *                  One from USBCtrlEPServiceComplete() and one from
 *                  USBCtrlTrfInHandler().
 *
 * Note:            
 *****************************************************************************/
static void USBCtrlTrfTxService(void)
{
    BYTE byteToSend;

    //Figure out how many bytes of data to send in the next IN transaction.
    //Assume a full size packet, unless otherwise determined below.
    byteToSend = USB_EP0_BUFF_SIZE;         
    if(inPipes[0].wCount.Val < (BYTE)USB_EP0_BUFF_SIZE)
    {
        byteToSend = inPipes[0].wCount.Val;

        //Keep track of whether or not we have sent a "short packet" yet.
        //This is useful so that later on, we can configure EP0 IN to STALL,
        //after we have sent all of the intended data.  This makes sure the
        //hardware STALLs if the host erroneously tries to send more IN token 
        //packets, requesting more data than intended in the control transfer.
        if(shortPacketStatus == SHORT_PKT_NOT_USED)
        {
            shortPacketStatus = SHORT_PKT_PENDING;
        }
        else if(shortPacketStatus == SHORT_PKT_PENDING)
        {
            shortPacketStatus = SHORT_PKT_SENT;
        }
    }

    //Keep track of how many bytes remain to be sent in the transfer, by
    //subtracting the number of bytes about to be sent from the total.
    inPipes[0].wCount.Val = inPipes[0].wCount.Val - byteToSend;
    
    //Next, load the number of bytes to send to BC7..0 in buffer descriptor.
    //Note: Control endpoints may never have a max packet size of > 64 bytes.
    //Therefore, the BC8 and BC9 bits should always be maintained clear.
    pBDTEntryIn[0]->CNT = byteToSend;

    //Now copy the data from the source location, to the CtrlTrfData[] buffer,
    //which we will send to the host.
    pDst = (USB_VOLATILE BYTE*)CtrlTrfData;                // Set destination pointer
    if(inPipes[0].info.bits.ctrl_trf_mem == USB_EP0_ROM)   // Determine type of memory source
    {
        while(byteToSend)
        {
            *pDst++ = *inPipes[0].pSrc.bRom++;
            byteToSend--;
        }//end while(byte_to_send.Val)
    }
    else  // RAM
    {
        while(byteToSend)
        {
            *pDst++ = *inPipes[0].pSrc.bRam++;
            byteToSend--;
        }//end while(byte_to_send.Val)
    }//end if(usb_stat.ctrl_trf_mem == _ROM)
}//end USBCtrlTrfTxService

/******************************************************************************
 * Function:        void USBCtrlTrfRxService(void)
 *
 * PreCondition:    pDst and wCount are setup properly.
 *                  pSrc is always &CtrlTrfData
 *                  usb_stat.ctrl_trf_mem is always USB_EP0_RAM.
 *                  wCount should be set to 0 at the start of each control
 *                  transfer.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine is used for host to device control transfers
 *					(uses OUT transactions).  This function receives the data that arrives
 *					on EP0 OUT, and copies it into the appropriate outPipes[0].pDst.bRam
 *					buffer.  Once the host has sent all the data it was intending
 *					to send, this function will call the appropriate outPipes[0].pFunc()
 *					handler (unless it is NULL), so that it can be used by the
 *					intended target firmware.
 *
 * Note:            None
 *****************************************************************************/
static void USBCtrlTrfRxService(void)
{
    BYTE byteToRead;
    BYTE i;

    //Load byteToRead with the number of bytes the host just sent us in the 
    //last OUT transaction.
    byteToRead = pBDTEntryEP0OutCurrent->CNT;   

    //Update the "outPipes[0].wCount.Val", which keeps track of the total number
    //of remaining bytes expected to be received from the host, in the control
    //transfer.  First check to see if the host sent us more bytes than the
    //application firmware was expecting to receive.
    if(byteToRead > outPipes[0].wCount.Val)
    {
        byteToRead = outPipes[0].wCount.Val;
    }	
    //Reduce the number of remaining bytes by the number we just received.
	outPipes[0].wCount.Val = outPipes[0].wCount.Val - byteToRead;   

    //Copy the OUT DATAx packet bytes that we just received from the host,
    //into the user application buffer space.
    for(i=0;i<byteToRead;i++)
    {
        *outPipes[0].pDst.bRam++ = CtrlTrfData[i];
    }//end while(byteToRead.Val)

    //If there is more data to receive, prepare EP0 OUT so that it can receive 
	//the next packet in the sequence.
    if(outPipes[0].wCount.Val > 0)
    {
        pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
        pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&CtrlTrfData);
        if(pBDTEntryEP0OutCurrent->STAT.DTS == 0)
        {
            pBDTEntryEP0OutNext->STAT.Val = _USIE|_DAT1|(_DTSEN & _DTS_CHECKING_ENABLED);
        }
        else
        {
            pBDTEntryEP0OutNext->STAT.Val = _USIE|_DAT0|(_DTSEN & _DTS_CHECKING_ENABLED);
        }
    }
    else
    {
	    //We have received all OUT packets that we were expecting to
	    //receive for the control transfer.  Prepare EP0 OUT to receive
		//the next SETUP transaction that may arrive.
        pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
        pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&SetupPkt);
        //Configure EP0 OUT to receive the next SETUP transaction for any future
        //control transfers.  However, set BSTALL in case the host tries to send
        //more data than it claims it was going to send.
        pBDTEntryEP0OutNext->STAT.Val = _USIE|_BSTALL;

		//All data bytes for the host to device control write (OUT) have now been
		//received successfully.
		//Go ahead and call the user specified callback function, to use/consume
		//the control transfer data (ex: if the "void (*function)" parameter 
		//was non-NULL when USBEP0Receive() was called).
        if(outPipes[0].pFunc != NULL)
        {
            #if defined(__XC8)
                //Special pragmas to suppress an expected/harmless warning
                //message when building with the XC8 compiler
                #pragma warning push
                #pragma warning disable 1088
                outPipes[0].pFunc();    //Call the user's callback function
                #pragma warning pop
            #else
                outPipes[0].pFunc();    //Call the user's callback function
            #endif
        }
        outPipes[0].info.bits.busy = 0;    

        //Ready to arm status stage IN transaction now, if the application
        //firmware has completed processing the request.  If it is still busy
        //and needs more time to finish handling the request, then the user
        //callback (the one called by the outPipes[0].pFunc();) should set the
        //USBDeferStatusStagePacket to TRUE (by calling USBDeferStatusStage()).  In 
        //this case, it is the application's firmware responsibility to call 
        //the USBCtrlEPAllowStatusStage() function, once it is fully done handling the request.
        //Note: The application firmware must process the request and call
        //USBCtrlEPAllowStatusStage() in a semi-timely fashion.  "Semi-timely"
        //means either 50ms, 500ms, or 5 seconds, depending on the type of
        //control transfer.  See the USB 2.0 specification section 9.2.6 for
        //more details.
        if(USBDeferStatusStagePacket == FALSE)
        {
            USBCtrlEPAllowStatusStage();
        }            
    }    

}//end USBCtrlTrfRxService


/********************************************************************
 * Function:        void USBStdSetCfgHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine first disables all endpoints by
 *                  clearing UEP registers. It then configures
 *                  (initializes) endpoints by calling the callback
 *                  function USBCBInitEP().
 *
 * Note:            None
 *******************************************************************/
static void USBStdSetCfgHandler(void)
{
    BYTE i;

    // This will generate a zero length packet
    inPipes[0].info.bits.busy = 1;            

    //Clear all of the endpoint control registers
    DisableNonZeroEndpoints(USB_MAX_EP_NUMBER);

    //Clear all of the BDT entries
    memset((void*)&BDT[0], 0x00, sizeof(BDT));

    // Assert reset request to all of the Ping Pong buffer pointers
    USBPingPongBufferReset = 1;                                   

	//Re-Initialize all ping pong software state bits to 0 (which corresponds to
	//the EVEN buffer being the next one that will be used), since we are also 
	//doing a hardware ping pong pointer reset above.
	for(i = 0; i < (BYTE)(USB_MAX_EP_NUMBER+1u); i++)
	{
		ep_data_in[i].Val = 0u;
        ep_data_out[i].Val = 0u;
	}

    //clear the alternate interface settings
    memset((void*)&USBAlternateInterface,0x00,USB_MAX_NUM_INT);

    //Stop trying to reset ping pong buffer pointers
    USBPingPongBufferReset = 0;

    pBDTEntryIn[0] = (volatile BDT_ENTRY*)&BDT[EP0_IN_EVEN];

	//Set the next out to the current out packet
    pBDTEntryEP0OutCurrent = (volatile BDT_ENTRY*)&BDT[EP0_OUT_EVEN];
    pBDTEntryEP0OutNext = pBDTEntryEP0OutCurrent;

    //set the current configuration
    USBActiveConfiguration = SetupPkt.bConfigurationValue;

    //if the configuration value == 0
    if(USBActiveConfiguration == 0)
    {
        //Go back to the addressed state
        USBDeviceState = ADDRESS_STATE;
    }
    else
    {
        //initialize the required endpoints
        USB_SET_CONFIGURATION_HANDLER(EVENT_CONFIGURED,(void*)&USBActiveConfiguration,1);

        //Otherwise go to the configured state.  Update the state variable last,
        //after performing all of the set configuration related initialization
        //tasks.
        USBDeviceState = CONFIGURED_STATE;		
    }//end if(SetupPkt.bConfigurationValue == 0)
}//end USBStdSetCfgHandler


/********************************************************************
 * Function:        void USBStdGetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine handles the standard GET_DESCRIPTOR
 *                  request.
 *
 * Note:            None
 *******************************************************************/
static void USBStdGetDscHandler(void)
{
    if(SetupPkt.bmRequestType == 0x80)
    {
        inPipes[0].info.Val = USB_EP0_ROM | USB_EP0_BUSY | USB_EP0_INCLUDE_ZERO;

        switch(SetupPkt.bDescriptorType)
        {
            case USB_DESCRIPTOR_DEVICE:
                #if !defined(USB_USER_DEVICE_DESCRIPTOR)
                    inPipes[0].pSrc.bRom = (ROM BYTE*)&device_dsc;
                #else
                    inPipes[0].pSrc.bRom = (ROM BYTE*)USB_USER_DEVICE_DESCRIPTOR;
                #endif
                inPipes[0].wCount.Val = sizeof(device_dsc);
                break;
            case USB_DESCRIPTOR_CONFIGURATION:
                #if !defined(USB_USER_CONFIG_DESCRIPTOR)
                    inPipes[0].pSrc.bRom = *(USB_CD_Ptr+SetupPkt.bDscIndex);
                #else
                    inPipes[0].pSrc.bRom = *(USB_USER_CONFIG_DESCRIPTOR+SetupPkt.bDscIndex);
                #endif

                //This must be loaded using byte addressing.  The source pointer
                //  may not be word aligned for the 16 or 32 bit machines resulting
                //  in an address error on the dereference.
                inPipes[0].wCount.byte.LB = *(inPipes[0].pSrc.bRom+2);
                inPipes[0].wCount.byte.HB = *(inPipes[0].pSrc.bRom+3);
                break;
            case USB_DESCRIPTOR_STRING:
                //USB_NUM_STRING_DESCRIPTORS was introduced as optional in release v2.3.  In v2.4 and
                //  later it is now manditory.  This should be defined in usb_config.h and should
                //  indicate the number of string descriptors.
                #if defined( RAM_BASED_SERIALNUMBER )
                if(SetupPkt.bDscIndex<(USB_NUM_STRING_DESCRIPTORS-1) )
                #else
                if(SetupPkt.bDscIndex<(USB_NUM_STRING_DESCRIPTORS) )  // original if-clause
                #endif
                {
                    //Get a pointer to the String descriptor requested
                    inPipes[0].pSrc.bRom = *(USB_SD_Ptr+SetupPkt.bDscIndex);
                    // Set data count
                    inPipes[0].wCount.Val = *inPipes[0].pSrc.bRom;                    
                }
                #if defined( RAM_BASED_SERIALNUMBER )
                else if(SetupPkt.bDscIndex==(USB_NUM_STRING_DESCRIPTORS-1) )  // serial num is last index
                {
                    // NOTE: you must set my_RAMSN.SerialNumber elsewhere
                    // populate struct, or done elsewhere, from EEPROM
                    my_RAMSN.bLength = 2 + sizeof(my_RAMSN.SerialNumber);
                    my_RAMSN.bDscType = USB_DESCRIPTOR_STRING;

                    //Set to RAM
                    inPipes[0].info.Val = USB_EP0_RAM | USB_EP0_BUSY;
                    //Send address of struct to bRam
                    inPipes[0].pSrc.bRam = &my_RAMSN.bLength;//Use pointers if more than one SD needs to come from RAM.
                    // Set data count
                    inPipes[0].wCount.Val = *inPipes[0].pSrc.bRam;
                }
                #endif
                #if defined(IMPLEMENT_MICROSOFT_OS_DESCRIPTOR)
                else if(SetupPkt.bDscIndex == MICROSOFT_OS_DESCRIPTOR_INDEX)
                {
                    //Get a pointer to the special MS OS string descriptor requested
                    inPipes[0].pSrc.bRom = (ROM BYTE*)&MSOSDescriptor;
                    // Set data count
                    inPipes[0].wCount.Val = *inPipes[0].pSrc.bRom;                    
                }    
                #endif
                else
                {
                    inPipes[0].info.Val = 0;
                }
                break;
            default:
                inPipes[0].info.Val = 0;
                break;
        }//end switch
    }//end if
}//end USBStdGetDscHandler

/********************************************************************
 * Function:        void USBStdGetStatusHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine handles the standard GET_STATUS request
 *
 * Note:            None
 *******************************************************************/
static void USBStdGetStatusHandler(void)
{
    CtrlTrfData[0] = 0;                 // Initialize content
    CtrlTrfData[1] = 0;

    switch(SetupPkt.Recipient)
    {
        case USB_SETUP_RECIPIENT_DEVICE_BITFIELD:
            inPipes[0].info.bits.busy = 1;
            /*
             * [0]: bit0: Self-Powered Status [0] Bus-Powered [1] Self-Powered
             *      bit1: RemoteWakeup        [0] Disabled    [1] Enabled
             */
            if(self_power == 1) // self_power is defined in HardwareProfile.h
            {
                CtrlTrfData[0]|=0x01;
            }

            if(RemoteWakeup == TRUE)
            {
                CtrlTrfData[0]|=0x02;
            }
            break;
        case USB_SETUP_RECIPIENT_INTERFACE_BITFIELD:
            inPipes[0].info.bits.busy = 1;     // No data to update
            break;
        case USB_SETUP_RECIPIENT_ENDPOINT_BITFIELD:
            inPipes[0].info.bits.busy = 1;
            /*
             * [0]: bit0: Halt Status [0] Not Halted [1] Halted
             */
            {
                BDT_ENTRY *p;

                if(SetupPkt.EPDir == 0)
                {
                    p = (BDT_ENTRY*)pBDTEntryOut[SetupPkt.EPNum];
                }
                else
                {
                    p = (BDT_ENTRY*)pBDTEntryIn[SetupPkt.EPNum];
                }

                if((p->STAT.UOWN == 1) && (p->STAT.BSTALL == 1))
                    CtrlTrfData[0]=0x01;    // Set bit0
                break;
            }
    }//end switch

    if(inPipes[0].info.bits.busy == 1)
    {
        inPipes[0].pSrc.bRam = (BYTE*)&CtrlTrfData;            // Set Source
        inPipes[0].info.bits.ctrl_trf_mem = USB_EP0_RAM;               // Set memory type
        inPipes[0].wCount.v[0] = 2;                         // Set data count
    }//end if(...)
}//end USBStdGetStatusHandler

/********************************************************************
 * Function:        void USBStallHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    
 *
 * Overview:        This function handles the event of a STALL 
 *                  occuring on the bus
 *
 * Note:            None
 *******************************************************************/
static void USBStallHandler(void)
{
    /*
     * Does not really have to do anything here,
     * even for the control endpoint.
     * All BDs of Endpoint 0 are owned by SIE right now,
     * but once a Setup Transaction is received, the ownership
     * for EP0_OUT will be returned to CPU.
     * When the Setup Transaction is serviced, the ownership
     * for EP0_IN will then be forced back to CPU by firmware.
     */

    /* v2b fix */
    if(U1EP0bits.EPSTALL == 1)
    {
        // UOWN - if 0, owned by CPU, if 1, owned by SIE
        if((pBDTEntryEP0OutCurrent->STAT.Val == _USIE) && (pBDTEntryIn[0]->STAT.Val == (_USIE|_BSTALL)))
        {
            // Set ep0Bo to stall also
            pBDTEntryEP0OutCurrent->STAT.Val = _USIE|_DAT0|(_DTSEN & _DTS_CHECKING_ENABLED)|_BSTALL;
        }//end if
        U1EP0bits.EPSTALL = 0;               // Clear stall status
    }//end if

    USBClearInterruptFlag(USBStallIFReg,USBStallIFBitNum);
}

/********************************************************************
 * Function:        void USBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    
 *
 * Overview:        This function handles if the host tries to 
 *                  suspend the device
 *
 * Note:            None
 *******************************************************************/
static void USBSuspend(void)
{
    /*
     * NOTE: Do not clear UIRbits.ACTVIF here!
     * Reason:
     * ACTVIF is only generated once an IDLEIF has been generated.
     * This is a 1:1 ratio interrupt generation.
     * For every IDLEIF, there will be only one ACTVIF regardless of
     * the number of subsequent bus transitions.
     *
     * If the ACTIF is cleared here, a problem could occur when:
     * [       IDLE       ][bus activity ->
     * <--- 3 ms ----->     ^
     *                ^     ACTVIF=1
     *                IDLEIF=1
     *  #           #           #           #   (#=Program polling flags)
     *                          ^
     *                          This polling loop will see both
     *                          IDLEIF=1 and ACTVIF=1.
     *                          However, the program services IDLEIF first
     *                          because ACTIVIE=0.
     *                          If this routine clears the only ACTIVIF,
     *                          then it can never get out of the suspend
     *                          mode.
     */
    USBActivityIE = 1;                     // Enable bus activity interrupt
    USBClearInterruptFlag(USBIdleIFReg,USBIdleIFBitNum);

    #if defined(__18CXX) || defined(_PIC14E)
        U1CONbits.SUSPND = 1;                   // Put USB module in power conserve
                                                // mode, SIE clock inactive
    #endif
    USBBusIsSuspended = TRUE;
 
    /*
     * At this point the PIC can go into sleep,idle, or
     * switch to a slower clock, etc.  This should be done in the
     * USBCBSuspend() if necessary.
     */
    USB_SUSPEND_HANDLER(EVENT_SUSPEND,0,0);
}

/********************************************************************
 * Function:        void USBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            None
 *******************************************************************/
static void USBWakeFromSuspend(void)
{
    USBBusIsSuspended = FALSE;

    /*
     * If using clock switching, the place to restore the original
     * microcontroller core clock frequency is in the USBCBWakeFromSuspend() callback
     */
    USB_WAKEUP_FROM_SUSPEND_HANDLER(EVENT_RESUME,0,0);

    #if defined(__18CXX) || defined(_PIC14E)
        //To avoid improperly clocking the USB module, make sure the oscillator
        //settings are consistant with USB operation before clearing the SUSPND bit.
        //Make sure the correct oscillator settings are selected in the 
        //"USB_WAKEUP_FROM_SUSPEND_HANDLER(EVENT_RESUME,0,0)" handler.
        U1CONbits.SUSPND = 0;   // Bring USB module out of power conserve
                                // mode.
    #endif


    USBActivityIE = 0;

    /********************************************************************
    Bug Fix: Feb 26, 2007 v2.1
    *********************************************************************
    The ACTVIF bit cannot be cleared immediately after the USB module wakes
    up from Suspend or while the USB module is suspended. A few clock cycles
    are required to synchronize the internal hardware state machine before
    the ACTIVIF bit can be cleared by firmware. Clearing the ACTVIF bit
    before the internal hardware is synchronized may not have an effect on
    the value of ACTVIF. Additonally, if the USB module uses the clock from
    the 96 MHz PLL source, then after clearing the SUSPND bit, the USB
    module may not be immediately operational while waiting for the 96 MHz
    PLL to lock.
    ********************************************************************/

    // UIRbits.ACTVIF = 0;                      // Removed
    #if defined(__18CXX)
    while(USBActivityIF)
    #endif
    {
        USBClearInterruptFlag(USBActivityIFReg,USBActivityIFBitNum);
    }  // Added

}//end USBWakeFromSuspend

/********************************************************************
 * Function:        void USBCtrlEPService(void)
 *
 * PreCondition:    USTAT is loaded with a valid endpoint address.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        USBCtrlEPService checks for three transaction
 *                  types that it knows how to service and services
 *                  them:
 *                  1. EP0 SETUP
 *                  2. EP0 OUT
 *                  3. EP0 IN
 *                  It ignores all other types (i.e. EP1, EP2, etc.)
 *
 * Note:            None
 *******************************************************************/
static void USBCtrlEPService(void)
{
    //If we get to here, that means a successful transaction has just occurred 
    //on EP0.  This means "progress" has occurred in the currently pending 
    //control transfer, so we should re-initialize our timeout counter.
    #if defined(USB_ENABLE_STATUS_STAGE_TIMEOUTS)
        USBStatusStageTimeoutCounter = USB_STATUS_STAGE_TIMEOUT;
    #endif
	
	//Check if the last transaction was on EP0 OUT endpoint (of any kind, to either the even or odd buffer if ping pong buffers used)
    if((USTATcopy.Val & USTAT_EP0_PP_MASK) == USTAT_EP0_OUT_EVEN)
    {
		//Point to the EP0 OUT buffer of the buffer that arrived
        #if defined (_PIC14E) || defined(__18CXX)
            pBDTEntryEP0OutCurrent = (volatile BDT_ENTRY*)&BDT[(USTATcopy.Val & USTAT_EP_MASK)>>1];
        #elif defined(__C30__) || defined(__C32__) || defined __XC16__
            pBDTEntryEP0OutCurrent = (volatile BDT_ENTRY*)&BDT[(USTATcopy.Val & USTAT_EP_MASK)>>2];
        #else
            #error "unimplemented"
        #endif

		//Set the next out to the current out packet
        pBDTEntryEP0OutNext = pBDTEntryEP0OutCurrent;
		//Toggle it to the next ping pong buffer (if applicable)
        ((BYTE_VAL*)&pBDTEntryEP0OutNext)->Val ^= USB_NEXT_EP0_OUT_PING_PONG;

		//If the current EP0 OUT buffer has a SETUP packet
        if(pBDTEntryEP0OutCurrent->STAT.PID == PID_SETUP)
        {
            unsigned char setup_cnt;

	        //The SETUP transaction data may have gone into the the CtrlTrfData 
	        //buffer, or elsewhere, depending upon how the BDT was prepared
	        //before the transaction.  Therefore, we should copy the data to the 
	        //SetupPkt buffer so it can be processed correctly by USBCtrlTrfSetupHandler().		    
            for(setup_cnt = 0; setup_cnt < 8u; setup_cnt++) //SETUP data packets always contain exactly 8 bytes.
            {
                *(BYTE*)((BYTE*)&SetupPkt + setup_cnt) = *(BYTE*)ConvertToVirtualAddress(pBDTEntryEP0OutCurrent->ADR);
                pBDTEntryEP0OutCurrent->ADR++;
            }    
            pBDTEntryEP0OutCurrent->ADR = ConvertToPhysicalAddress(&SetupPkt);

			//Handle the control transfer (parse the 8-byte SETUP command and figure out what to do)
            USBCtrlTrfSetupHandler();
        }
        else
        {
			//Handle the DATA transfer
            USBCtrlTrfOutHandler();
        }
    }
    else if((USTATcopy.Val & USTAT_EP0_PP_MASK) == USTAT_EP0_IN)
    {
		//Otherwise the transmission was and EP0 IN
		//  so take care of the IN transfer
        USBCtrlTrfInHandler();
    }

}//end USBCtrlEPService

/********************************************************************
 * Function:        void USBCtrlTrfSetupHandler(void)
 *
 * PreCondition:    SetupPkt buffer is loaded with valid USB Setup Data
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine is a task dispatcher and has 3 stages.
 *                  1. It initializes the control transfer state machine.
 *                  2. It calls on each of the module that may know how to
 *                     service the Setup Request from the host.
 *                     Module Example: USBD, HID, CDC, MSD, ...
 *                     A callback function, USBCBCheckOtherReq(),
 *                     is required to call other module handlers.
 *                  3. Once each of the modules has had a chance to check if
 *                     it is responsible for servicing the request, stage 3
 *                     then checks direction of the transfer to determine how
 *                     to prepare EP0 for the control transfer.
 *                     Refer to USBCtrlEPServiceComplete() for more details.
 *
 * Note:            Microchip USB Firmware has three different states for
 *                  the control transfer state machine:
 *                  1. WAIT_SETUP
 *                  2. CTRL_TRF_TX (device sends data to host through IN transactions)
 *                  3. CTRL_TRF_RX (device receives data from host through OUT transactions)
 *                  Refer to firmware manual to find out how one state
 *                  is transitioned to another.
 *
 *                  A Control Transfer is composed of many USB transactions.
 *                  When transferring data over multiple transactions,
 *                  it is important to keep track of data source, data
 *                  destination, and data count. These three parameters are
 *                  stored in pSrc,pDst, and wCount. A flag is used to
 *                  note if the data source is from ROM or RAM.
 *
 *******************************************************************/
static void USBCtrlTrfSetupHandler(void)
{
    //--------------------------------------------------------------------------
    //1. Re-initialize state tracking variables related to control transfers.
    //--------------------------------------------------------------------------
    shortPacketStatus = SHORT_PKT_NOT_USED;  
    USBDeferStatusStagePacket = FALSE;
    USBDeferINDataStagePackets = FALSE;
    USBDeferOUTDataStagePackets = FALSE;
    BothEP0OutUOWNsSet = FALSE;    
    controlTransferState = WAIT_SETUP;

    //Abandon any previous control transfers that might have been using EP0.
    //Ordinarily, nothing actually needs abandoning, since the previous control
    //transfer would have completed successfully prior to the host sending the next
    //SETUP packet.  However, in a timeout error case, or after an EP0 STALL event,
    //one or more UOWN bits might still be set.  If so, we should clear the UOWN bits,
    //so the EP0 IN/OUT endpoints are in a known inactive state, ready for re-arming
    //by the class request handler that will be called next.
    pBDTEntryIn[0]->STAT.Val &= ~(_USIE);     
    ((BYTE_VAL*)&pBDTEntryIn[0])->Val ^= USB_NEXT_EP0_IN_PING_PONG;
    pBDTEntryIn[0]->STAT.Val &= ~(_USIE);      
    ((BYTE_VAL*)&pBDTEntryIn[0])->Val ^= USB_NEXT_EP0_IN_PING_PONG;
    pBDTEntryEP0OutNext->STAT.Val &= ~(_USIE);         

    inPipes[0].info.Val = 0;
    inPipes[0].wCount.Val = 0;
    outPipes[0].info.Val = 0;
    outPipes[0].wCount.Val = 0;
    

    //--------------------------------------------------------------------------
    //2. Now find out what was in the SETUP packet, and begin handling the request.
    //--------------------------------------------------------------------------
    USBCheckStdRequest();                                               //Check for standard USB "Chapter 9" requests.
    USB_NONSTANDARD_EP0_REQUEST_HANDLER(EVENT_EP0_REQUEST,0,0); //Check for USB device class specific requests


    //--------------------------------------------------------------------------
    //3. Re-arm EP0 IN and EP0 OUT endpoints, based on the control transfer in 
    //   progress.  If one of the above handlers (in step 2) knew how to process
    //   the request, it will have set one of the inPipes[0].info.bits.busy or
    //   outPipes[0].info.bits.busy flags = 1.  This lets the
    //   USBCtrlEPServiceComplete() function know how and which endpoints to 
    //   arm.  If both info.bits.busy flags are = 0, then no one knew how to
    //   process the request.  In this case, the default behavior will be to
    //   perform protocol STALL on EP0.
    //-------------------------------------------------------------------------- 
    USBCtrlEPServiceComplete();
}//end USBCtrlTrfSetupHandler


/******************************************************************************
 * Function:        void USBCtrlTrfOutHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine handles an OUT transaction according to
 *                  which control transfer state is currently active.
 *
 * Note:            Note that if the the control transfer was from
 *                  host to device, the session owner should be notified
 *                  at the end of each OUT transaction to service the
 *                  received data.
 *
 *****************************************************************************/
static void USBCtrlTrfOutHandler(void)
{
    if(controlTransferState == CTRL_TRF_RX)
    {
        USBCtrlTrfRxService();	//Copies the newly received data into the appropriate buffer and configures EP0 OUT for next transaction.
    }
    else //In this case the last OUT transaction must have been a status stage of a CTRL_TRF_TX (<setup><in><in>...<OUT>  <-- this last OUT just occurred as the status stage)
    {
        //If the status stage is complete, this means we are done with the 
        //control transfer.  Go back to the idle "WAIT_SETUP" state.
        controlTransferState = WAIT_SETUP;

	    //Prepare EP0 OUT for the next SETUP transaction, however, it may have
	    //already been prepared if ping-pong buffering was enabled on EP0 OUT,
	    //and the last control transfer was of direction: device to host, see
	    //USBCtrlEPServiceComplete().  If it was already prepared, do not want
	    //to do anything to the BDT.
		if(BothEP0OutUOWNsSet == FALSE)
		{
	        pBDTEntryEP0OutNext->CNT = USB_EP0_BUFF_SIZE;
	        pBDTEntryEP0OutNext->ADR = ConvertToPhysicalAddress(&SetupPkt);
	        pBDTEntryEP0OutNext->STAT.Val = _USIE|_DAT0|(_DTSEN & _DTS_CHECKING_ENABLED)|_BSTALL;			
		}
		else
		{
			BothEP0OutUOWNsSet = FALSE;
		}
    }
}

/******************************************************************************
 * Function:        void USBCtrlTrfInHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine handles an IN transaction according to
 *                  which control transfer state is currently active.
 *
 * Note:            A Set Address Request must not change the acutal address
 *                  of the device until the completion of the control
 *                  transfer. The end of the control transfer for Set Address
 *                  Request is an IN transaction. Therefore it is necessary
 *                  to service this unique situation when the condition is
 *                  right. Macro mUSBCheckAdrPendingState is defined in
 *                  usb9.h and its function is to specifically service this
 *                  event.
 *****************************************************************************/
static void USBCtrlTrfInHandler(void)
{
    BYTE lastDTS;

    lastDTS = pBDTEntryIn[0]->STAT.DTS;

    //switch to the next ping pong buffer
    ((BYTE_VAL*)&pBDTEntryIn[0])->Val ^= USB_NEXT_EP0_IN_PING_PONG;

    //Must check if in ADR_PENDING_STATE.  If so, we need to update the address
    //now, since the IN status stage of the (set address) control transfer has 
    //evidently completed successfully.
    if(USBDeviceState == ADR_PENDING_STATE)
    {
        U1ADDR = SetupPkt.bDevADR.Val;
        if(U1ADDR != 0u)
        {
            USBDeviceState=ADDRESS_STATE;
        }
        else
        {
            USBDeviceState=DEFAULT_STATE;
        }
    }//end if


    if(controlTransferState == CTRL_TRF_TX)
    {
        pBDTEntryIn[0]->ADR = ConvertToPhysicalAddress(CtrlTrfData);
        USBCtrlTrfTxService();

        //Check if we have already sent a short packet.  If so, configure
        //the endpoint to STALL in response to any further IN tokens (in the
        //case that the host erroneously tries to receive more data than it
        //should).
        if(shortPacketStatus == SHORT_PKT_SENT)
        {
            // If a short packet has been sent, don't want to send any more,
            // stall next time if host is still trying to read.
            pBDTEntryIn[0]->STAT.Val = _USIE|_BSTALL;
        }
        else
        {
            if(lastDTS == 0)
            {
                pBDTEntryIn[0]->STAT.Val = _USIE|_DAT1|(_DTSEN & _DTS_CHECKING_ENABLED);
            }
            else
            {
                pBDTEntryIn[0]->STAT.Val = _USIE|_DAT0|(_DTSEN & _DTS_CHECKING_ENABLED);
            }
        }//end if(...)else
    }
	else // must have been a CTRL_TRF_RX status stage IN packet (<setup><out><out>...<IN>  <-- this last IN just occurred as the status stage)
	{
        //if someone is still expecting data from the control transfer
        //  then make sure to terminate that request and let them know that
        //  they are done
        if(outPipes[0].info.bits.busy == 1)
        {
            if(outPipes[0].pFunc != NULL)
            {
                outPipes[0].pFunc();
            }
            outPipes[0].info.bits.busy = 0;
        }
    	
        controlTransferState = WAIT_SETUP;
        //Don't need to arm EP0 OUT here.  It was already armed by the last <out> that
        //got processed by the USBCtrlTrfRxService() handler.
	}	

}


/********************************************************************
 * Function:        void USBCheckStdRequest(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine checks the setup data packet to see
 *                  if it knows how to handle it
 *
 * Note:            None
 *******************************************************************/
static void USBCheckStdRequest(void)
{
    if(SetupPkt.RequestType != USB_SETUP_TYPE_STANDARD_BITFIELD) return;

    switch(SetupPkt.bRequest)
    {
        case USB_REQUEST_SET_ADDRESS:
            inPipes[0].info.bits.busy = 1;            // This will generate a zero length packet
            USBDeviceState = ADR_PENDING_STATE;       // Update state only
            /* See USBCtrlTrfInHandler() for the next step */
            break;
        case USB_REQUEST_GET_DESCRIPTOR:
            USBStdGetDscHandler();
            break;
        case USB_REQUEST_SET_CONFIGURATION:
            USBStdSetCfgHandler();
            break;
        case USB_REQUEST_GET_CONFIGURATION:
            inPipes[0].pSrc.bRam = (BYTE*)&USBActiveConfiguration;         // Set Source
            inPipes[0].info.bits.ctrl_trf_mem = USB_EP0_RAM;               // Set memory type
            inPipes[0].wCount.v[0] = 1;                         // Set data count
            inPipes[0].info.bits.busy = 1;
            break;
        case USB_REQUEST_GET_STATUS:
            USBStdGetStatusHandler();
            break;
        case USB_REQUEST_CLEAR_FEATURE:
        case USB_REQUEST_SET_FEATURE:
            USBStdFeatureReqHandler();
            break;
        case USB_REQUEST_GET_INTERFACE:
            inPipes[0].pSrc.bRam = (BYTE*)&USBAlternateInterface[SetupPkt.bIntfID];  // Set source
            inPipes[0].info.bits.ctrl_trf_mem = USB_EP0_RAM;               // Set memory type
            inPipes[0].wCount.v[0] = 1;                         // Set data count
            inPipes[0].info.bits.busy = 1;
            break;
        case USB_REQUEST_SET_INTERFACE:
            inPipes[0].info.bits.busy = 1;
            USBAlternateInterface[SetupPkt.bIntfID] = SetupPkt.bAltID;
            break;
        case USB_REQUEST_SET_DESCRIPTOR:
            USB_SET_DESCRIPTOR_HANDLER(EVENT_SET_DESCRIPTOR,0,0);
            break;
        case USB_REQUEST_SYNCH_FRAME:
        default:
            break;
    }//end switch
}//end USBCheckStdRequest

/********************************************************************
 * Function:        void USBStdFeatureReqHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Can alter BDT entries.  Can also modify USB stack
 *                  Maintained variables.
 *
 * Side Effects:    None
 *
 * Overview:        This routine handles the standard SET & CLEAR
 *                  FEATURES requests
 *
 * Note:            This is a private function, intended for internal 
 *                  use by the USB stack, when processing SET/CLEAR
 *                  feature requests.  
 *******************************************************************/
static void USBStdFeatureReqHandler(void)
{
    BDT_ENTRY *p;
    EP_STATUS current_ep_data;
    #if defined(__C32__)
        DWORD* pUEP;
    #else
        unsigned char* pUEP;             
    #endif
    

    #ifdef	USB_SUPPORT_OTG
    //Check for USB On-The-Go (OTG) specific requests
    if ((SetupPkt.bFeature == OTG_FEATURE_B_HNP_ENABLE)&&
        (SetupPkt.Recipient == USB_SETUP_RECIPIENT_DEVICE_BITFIELD))
    {  
        inPipes[0].info.bits.busy = 1;
        if(SetupPkt.bRequest == USB_REQUEST_SET_FEATURE)
            USBOTGEnableHnp();
        else
            USBOTGDisableHnp();
    }

    if ((SetupPkt.bFeature == OTG_FEATURE_A_HNP_SUPPORT)&&
        (SetupPkt.Recipient == USB_SETUP_RECIPIENT_DEVICE_BITFIELD))
    {
        inPipes[0].info.bits.busy = 1;
        if(SetupPkt.bRequest == USB_REQUEST_SET_FEATURE)
            USBOTGEnableSupportHnp();
        else
            USBOTGDisableSupportHnp();
    }

    if ((SetupPkt.bFeature == OTG_FEATURE_A_ALT_HNP_SUPPORT)&&
        (SetupPkt.Recipient == USB_SETUP_RECIPIENT_DEVICE_BITFIELD))
    {
        inPipes[0].info.bits.busy = 1;
        if(SetupPkt.bRequest == USB_REQUEST_SET_FEATURE)
            USBOTGEnableAltHnp();
        else
            USBOTGDisableAltHnp();
    }
    #endif   //#ifdef USB_SUPPORT_OTG 

    //Check if the host sent a valid SET or CLEAR feature (remote wakeup) request.
    if((SetupPkt.bFeature == USB_FEATURE_DEVICE_REMOTE_WAKEUP)&&
       (SetupPkt.Recipient == USB_SETUP_RECIPIENT_DEVICE_BITFIELD))
    {
        inPipes[0].info.bits.busy = 1;
        if(SetupPkt.bRequest == USB_REQUEST_SET_FEATURE)
            RemoteWakeup = TRUE;
        else
            RemoteWakeup = FALSE;
    }//end if

    //Check if the host sent a valid SET or CLEAR endpoint halt request.
    if((SetupPkt.bFeature == USB_FEATURE_ENDPOINT_HALT)&&
       (SetupPkt.Recipient == USB_SETUP_RECIPIENT_ENDPOINT_BITFIELD)&&
       (SetupPkt.EPNum != 0) && (SetupPkt.EPNum <= USB_MAX_EP_NUMBER)&&
       (USBDeviceState == CONFIGURED_STATE))
    {
		//The request was valid.  Take control of the control transfer and
		//perform the host requested action.
		inPipes[0].info.bits.busy = 1;

        //Fetch a pointer to the BDT that the host wants to SET/CLEAR halt on.
        if(SetupPkt.EPDir == OUT_FROM_HOST)
        {
            p = (BDT_ENTRY*)pBDTEntryOut[SetupPkt.EPNum];
            current_ep_data.Val = ep_data_out[SetupPkt.EPNum].Val;
        }
        else
        {
            p = (BDT_ENTRY*)pBDTEntryIn[SetupPkt.EPNum];
            current_ep_data.Val = ep_data_in[SetupPkt.EPNum].Val;
        }

        //If ping pong buffering is enabled on the requested endpoint, need 
        //to point to the one that is the active BDT entry which the SIE will 
        //use for the next attempted transaction on that EP number.
        #if (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0) || (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)   
            if(current_ep_data.bits.ping_pong_state == 0) //Check if even
            {
                USBHALPingPongSetToEven(&p);
            }
            else //else must have been odd
            {
                USBHALPingPongSetToOdd(&p);
            }
        #endif
        
        //Update the BDT pointers with the new, next entry based on the feature
        //  request
        if(SetupPkt.EPDir == OUT_FROM_HOST)
        {
            pBDTEntryOut[SetupPkt.EPNum] = (volatile BDT_ENTRY *)p;
        }
        else
        {
            pBDTEntryIn[SetupPkt.EPNum] = (volatile BDT_ENTRY *)p;
        }

		//Check if it was a SET_FEATURE endpoint halt request
        if(SetupPkt.bRequest == USB_REQUEST_SET_FEATURE)
        {
            if(p->STAT.UOWN == 1)
            {
                //Mark that we are terminating this transfer and that the user
                //  needs to be notified later
                if(SetupPkt.EPDir == OUT_FROM_HOST)
                {
                    ep_data_out[SetupPkt.EPNum].bits.transfer_terminated = 1;
                }
                else
                {
                    ep_data_in[SetupPkt.EPNum].bits.transfer_terminated = 1;
                }
            }

			//Then STALL the endpoint
            p->STAT.Val |= _USIE|_BSTALL;
        }//if(SetupPkt.bRequest == USB_REQUEST_SET_FEATURE)
        else
        {
			//Else the request must have been a CLEAR_FEATURE endpoint halt.
            #if (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0) || (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)   
                //toggle over the to the non-active BDT
                USBAdvancePingPongBuffer(&p);  

                if(p->STAT.UOWN == 1)
                {
                    //Clear UOWN and set DTS state so it will be correct the next time
                    //the application firmware uses USBTransferOnePacket() on the EP.
                    p->STAT.Val &= (~_USIE);    //Clear UOWN bit
                    p->STAT.Val |= _DAT1;       //Set DTS to DATA1
                    USB_TRANSFER_TERMINATED_HANDLER(EVENT_TRANSFER_TERMINATED,p,sizeof(p));
                }
                else
                {
                    //UOWN already clear, but still need to set DTS to DATA1     
					p->STAT.Val |= _DAT1;
                }

                //toggle back to the active BDT (the one the SIE is currently looking at
                //and will use for the next successful transaction to take place on the EP
                USBAdvancePingPongBuffer(&p);    
                
                //Check if we are currently terminating, or have previously terminated
                //a transaction on the given endpoint.  If so, need to clear UOWN,
                //set DTS to the proper state, and call the application callback
                //function.
                if((current_ep_data.bits.transfer_terminated != 0) || (p->STAT.UOWN == 1))
                {
                    if(SetupPkt.EPDir == OUT_FROM_HOST)
                    {
                        ep_data_out[SetupPkt.EPNum].bits.transfer_terminated = 0;
                    }
                    else
                    {
                        ep_data_in[SetupPkt.EPNum].bits.transfer_terminated = 0;
                    }
                    //clear UOWN, clear DTS to DATA0, and finally remove the STALL condition     
                    p->STAT.Val &= ~(_USIE | _DAT1 | _BSTALL);  
                    //Call the application event handler callback function, so it can 
					//decide if the endpoint should get re-armed again or not.
                    USB_TRANSFER_TERMINATED_HANDLER(EVENT_TRANSFER_TERMINATED,p,sizeof(p));
                }
                else
                {
                    //clear UOWN, clear DTS to DATA0, and finally remove the STALL condition     
                    p->STAT.Val &= ~(_USIE | _DAT1 | _BSTALL); 
                } 
            #else //else we must not be using ping-pong buffering on the requested endpoint
                //Check if we need to call the user transfer terminated event callback function.
                //We should call the callback, if the endpoint was previously terminated,
                //or the endpoint is currently armed, and the host is performing clear
                //endpoint halt, even though the endpoint wasn't stalled.
                if((current_ep_data.bits.transfer_terminated != 0) || (p->STAT.UOWN == 1))
                {
                    //We are going to call the user transfer terminated callback.
                    //Clear the flag so we know we took care of it and don't need
                    //to call it again later.
                    if(SetupPkt.EPDir == OUT_FROM_HOST)
                    {
                        ep_data_out[SetupPkt.EPNum].bits.transfer_terminated = 0;
                    }
                    else
                    {
                        ep_data_in[SetupPkt.EPNum].bits.transfer_terminated = 0;
                    }
 
                    //Clear UOWN and remove the STALL condition.   
                    //  In this case we also need to set the DTS bit to 1 so that
                    //  it toggles to DATA0 the next time the application firmware
                    //  calls USBTransferOnePacket() (or equivalent macro).  
                    p->STAT.Val &= ~(_USIE | _BSTALL);  
                    p->STAT.Val |= _DAT1;
                    //Let the application firmware know a transaction just
                    //got terminated by the host, and that it is now free to
                    //re-arm the endpoint or do other tasks if desired.                                        
                    USB_TRANSFER_TERMINATED_HANDLER(EVENT_TRANSFER_TERMINATED,p,sizeof(p));
                }
                else
                {
                    //Clear UOWN and remove the STALL condition.   
                    //  In this case we also need to set the DTS bit to 1 so that
                    //  it toggles to DATA0 the next time the application firmware
                    //  calls USBTransferOnePacket() (or equivalent macro).  
                    p->STAT.Val &= ~(_USIE | _BSTALL);  
                    p->STAT.Val |= _DAT1;
                } 
            #endif //end of #if (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0) || (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)   
            
			//Get a pointer to the appropriate UEPn register
            #if defined(__C32__)
                pUEP = (DWORD*)(&U1EP0);
                pUEP += (SetupPkt.EPNum*4);
            #else
                pUEP = (unsigned char*)(&U1EP0+SetupPkt.EPNum);
            #endif

			//Clear the STALL bit in the UEP register
            *pUEP &= ~UEP_STALL;            
        }//end if(SetupPkt.bRequest == USB_REQUEST_SET_FEATURE)
    }//end if (lots of checks for set/clear endpoint halt)
}//end USBStdFeatureReqHandler


/** EOF USBDevice.c *****************************************************/
