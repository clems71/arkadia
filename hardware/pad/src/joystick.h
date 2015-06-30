/*******************************************************************************
  USB HID Function Driver

  Company:
    Microchip Technology Inc.

  File Name:
    usb_device_hid_joystick.h

  Summary:
    USB HID JOYSTICK Function Driver 

  Description:
    USB HID JOYSTICK Function Driver
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _USB_FUNCTION_HID_JOYSTICK_H
#define _USB_FUNCTION_HID_JOYSTICK_H


#include <stdint.h>
#include <stdbool.h>


// *****************************************************************************
// *****************************************************************************
// Section: USB Device HID Joystick types and definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

#define HAT_SWITCH_NORTH            0x0
#define HAT_SWITCH_NORTH_EAST       0x1
#define HAT_SWITCH_EAST             0x2
#define HAT_SWITCH_SOUTH_EAST       0x3
#define HAT_SWITCH_SOUTH            0x4
#define HAT_SWITCH_SOUTH_WEST       0x5
#define HAT_SWITCH_WEST             0x6
#define HAT_SWITCH_NORTH_WEST       0x7
#define HAT_SWITCH_NULL             0x8

// *****************************************************************************
/* USB HID Joystick Report

  Summary:
    USB HID Joystick Report.

  Description:
    This type represents all joystick related data.
    
  Remarks:
    None.
*/
typedef struct
{
    uint8_t buttons;
    uint8_t x;
    uint8_t y;
//    uint8_t leds;
} USB_HID_JOYSTICK_REPORT_WR;


typedef struct
{
//    uint8_t buttons;
    uint8_t leds;
} USB_HID_JOYSTICK_REPORT_RD;


// *****************************************************************************
// *****************************************************************************
// Section: USB Device HID Joystick functions 
// *****************************************************************************
// *****************************************************************************


/* This does not contains any functions. The joystick report created in 
 * this solution is an example one and not a definitive solution. The
 * joystick report can then be sent to the host by using the 
 * USB_DEVICE_HID_ReportSend function. The report descriptor for the
 * USB_HID_JOYSTICK_REPORT is contained in system_config.c */


#endif
