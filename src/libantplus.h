
// libantplus - http://mylcd.sourceforge.net/
// An LCD framebuffer and text rendering API
// Michael McElligott
// okio@users.sourceforge.net

//  Copyright (c) 2005-2011  Michael McElligott
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU LIBRARY GENERAL PUBLIC LICENSE for more details.
//
//	You should have received a copy of the GNU Library General Public
//	License along with this library; if not, write to the Free
//	Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _LIBANTPLUS_H_
#define _LIBANTPLUS_H_

#include <lusb0_usb.h>

#define ANTSTICK_VID		0x0FCF
#define ANTSTICK_PID		0x1008
#define EP_OUT				0x01
#define EP_IN				0x81
#define WRITE_TIMEOUT		1500
#define READ_TIMEOUT		1000
#define INPUTBUFFERSIZE		4096

#define STREAM_SYNC			0
#define STREAM_LENGTH		1
#define STREAM_MESSAGE		2
#define STREAM_DATA			3

#define STREAM_CHANNEL_ID			0
#define STREAM_CHANNEL_STATUS		1

#define STREAM_EVENT_CHANNEL_ID		0
#define STREAM_EVENT_RESPONSE_ID	1

#define STREAM_CAP_MAXCHANNELS		0
#define STREAM_CAP_MAXNETWORKS		1
#define STREAM_CAP_STDOPTIONS		2
#define STREAM_CAP_ADVANCED			3
#define STREAM_CAP_ADVANCED2		4

#define STREAM_STARTUP_REASON		0

#define STREAM_CHANNELSTATUS_STATUS 1

#define STREAM_VERSION_STRING		0

#define STREAM_CHANNELID_DEVNO_LO	1
#define STREAM_CHANNELID_DEVNO_HI	2
#define STREAM_CHANNELID_DEVTYPE	3
#define STREAM_CHANNELID_TRANTYPE	4

#define STREAM_EVENT_EVENTID		2

#define STREAM_RXBROADCAST_DEV120_SEQ	7
#define STREAM_RXBROADCAST_DEV120_HR	8

enum _eventi
{
	EVENTI_MESSAGE,
	EVENTI_CHANNEL,
	EVENTI_TOTAL
};


#define RESET_FLAGS_MASK                           ((unsigned char)0xE0)
#define RESET_SUSPEND                              ((unsigned char)0x80)              // this must follow bitfield def
#define RESET_SYNC                                 ((unsigned char)0x40)              // this must follow bitfield def
#define RESET_CMD                                  ((unsigned char)0x20)              // this must follow bitfield def
#define RESET_WDT                                  ((unsigned char)0x02)
#define RESET_RST                                  ((unsigned char)0x01)
#define RESET_POR                                  ((unsigned char)0x00)

#define STATUS_CHANNEL_STATE_MASK                  ((unsigned char)0x03)
#define STATUS_UNASSIGNED_CHANNEL                  ((unsigned char)0x00)
#define STATUS_ASSIGNED_CHANNEL                    ((unsigned char)0x01)
#define STATUS_SEARCHING_CHANNEL                   ((unsigned char)0x02)
#define STATUS_TRACKING_CHANNEL                    ((unsigned char)0x03)

//////////////////////////////////////////////
// Assign Channel Parameters
//////////////////////////////////////////////
#define PARAMETER_RX_NOT_TX                        ((unsigned char)0x00)
#define PARAMETER_TX_NOT_RX                        ((unsigned char)0x10)
#define PARAMETER_SHARED_CHANNEL                   ((unsigned char)0x20)
#define PARAMETER_NO_TX_GUARD_BAND                 ((unsigned char)0x40)
#define PARAMETER_ALWAYS_RX_WILD_CARD_SEARCH_ID    ((unsigned char)0x40)                 //Pre-AP2
#define PARAMETER_RX_ONLY                          ((unsigned char)0x40)

//////////////////////////////////////////////
// Ext. Assign Channel Parameters
//////////////////////////////////////////////
#define EXT_PARAM_ALWAYS_SEARCH                    ((unsigned char)0x01)
#define EXT_PARAM_FREQUENCY_AGILITY                ((unsigned char)0x04)


/////////////////////////////////////////////////////////////////////////////
// Message Format
// Messages are in the format:
//
// AX XX YY -------- CK
//
// where: AX    is the 1 byte sync byte either transmit or recieve
//        XX    is the 1 byte size of the message (0-249) NOTE: THIS WILL BE LIMITED BY THE EMBEDDED RECEIVE BUFFER SIZE
//        YY    is the 1 byte ID of the message (1-255, 0 is invalid)
//        ----- is the data of the message (0-249 bytes of data)
//        CK    is the 1 byte Checksum of the message
/////////////////////////////////////////////////////////////////////////////
#define MESG_TX_SYNC                         ((unsigned char)0xA4)
#define MESG_RX_SYNC                         ((unsigned char)0xA5)
#define MESG_SYNC_SIZE                       ((unsigned char)1)
#define MESG_SIZE_SIZE                       ((unsigned char)1)
#define MESG_ID_SIZE                         ((unsigned char)1)
#define MESG_CHANNEL_NUM_SIZE                ((unsigned char)1)
#define MESG_EXT_MESG_BF_SIZE                ((unsigned char)1)  // NOTE: this could increase in the future
#define MESG_CHECKSUM_SIZE                   ((unsigned char)1)
#define MESG_DATA_SIZE                       ((unsigned char)9)

// The largest serial message is an ANT data message with all of the extended fields
#define MESG_ANT_MAX_PAYLOAD_SIZE            ANT_STANDARD_DATA_PAYLOAD_SIZE

#define MESG_MAX_EXT_DATA_SIZE               (ANT_EXT_MESG_DEVICE_ID_FIELD_SIZE + 4 + 2) // ANT device ID (4 bytes) +  (4 bytes) +  (2 bytes)

#define MESG_MAX_DATA_SIZE                   (MESG_ANT_MAX_PAYLOAD_SIZE + MESG_EXT_MESG_BF_SIZE + MESG_MAX_EXT_DATA_SIZE) // ANT data payload (8 bytes) + extended bitfield (1 byte) + extended data (10 bytes)
#define MESG_MAX_SIZE_VALUE                  (MESG_MAX_DATA_SIZE + MESG_CHANNEL_NUM_SIZE)  // this is the maximum value that the serial message size value is allowed to be
#define MESG_BUFFER_SIZE                     (MESG_SIZE_SIZE + MESG_ID_SIZE + MESG_CHANNEL_NUM_SIZE + MESG_MAX_DATA_SIZE + MESG_CHECKSUM_SIZE)
#define MESG_FRAMED_SIZE                     (MESG_ID_SIZE + MESG_CHANNEL_NUM_SIZE + MESG_MAX_DATA_SIZE)
#define MESG_HEADER_SIZE                     (MESG_SYNC_SIZE + MESG_SIZE_SIZE + MESG_ID_SIZE)
#define MESG_FRAME_SIZE                      (MESG_HEADER_SIZE + MESG_CHECKSUM_SIZE)
#define MESG_MAX_SIZE                        (MESG_MAX_DATA_SIZE + MESG_FRAME_SIZE)

#define MESG_SIZE_OFFSET                     (MESG_SYNC_SIZE)
#define MESG_ID_OFFSET                       (MESG_SYNC_SIZE + MESG_SIZE_SIZE)
#define MESG_DATA_OFFSET                     (MESG_HEADER_SIZE)
#define MESG_RECOMMENDED_BUFFER_SIZE         ((unsigned char)64)    


//////////////////////////////////////////////
// Message ID's
//////////////////////////////////////////////
#define MESG_INVALID_ID                      ((unsigned char)0x00)
#define MESG_EVENT_ID                        ((unsigned char)0x01)

#define MESG_VERSION_ID                      ((unsigned char)0x3E)
#define MESG_RESPONSE_EVENT_ID               ((unsigned char)0x40)

#define MESG_UNASSIGN_CHANNEL_ID             ((unsigned char)0x41)
#define MESG_ASSIGN_CHANNEL_ID               ((unsigned char)0x42)
#define MESG_CHANNEL_MESG_PERIOD_ID          ((unsigned char)0x43)
#define MESG_CHANNEL_SEARCH_TIMEOUT_ID       ((unsigned char)0x44)
#define MESG_CHANNEL_RADIO_FREQ_ID           ((unsigned char)0x45)
#define MESG_NETWORK_KEY_ID                  ((unsigned char)0x46)
#define MESG_RADIO_TX_POWER_ID               ((unsigned char)0x47)
#define MESG_RADIO_CW_MODE_ID                ((unsigned char)0x48)
#define MESG_SEARCH_WAVEFORM_ID              ((unsigned char)0x49)

#define MESG_SYSTEM_RESET_ID                 ((unsigned char)0x4A)
#define MESG_OPEN_CHANNEL_ID                 ((unsigned char)0x4B)
#define MESG_CLOSE_CHANNEL_ID                ((unsigned char)0x4C)
#define MESG_REQUEST_ID                      ((unsigned char)0x4D)

#define MESG_BROADCAST_DATA_ID               ((unsigned char)0x4E)
#define MESG_ACKNOWLEDGED_DATA_ID            ((unsigned char)0x4F)
#define MESG_BURST_DATA_ID                   ((unsigned char)0x50)

#define MESG_CHANNEL_ID_ID                   ((unsigned char)0x51)
#define MESG_CHANNEL_STATUS_ID               ((unsigned char)0x52)
#define MESG_RADIO_CW_INIT_ID                ((unsigned char)0x53)
#define MESG_CAPABILITIES_ID                 ((unsigned char)0x54)

#define MESG_STACKLIMIT_ID                   ((unsigned char)0x55)

#define MESG_SCRIPT_DATA_ID                  ((unsigned char)0x56)
#define MESG_SCRIPT_CMD_ID                   ((unsigned char)0x57)

#define MESG_ID_LIST_ADD_ID                  ((unsigned char)0x59)
#define MESG_ID_LIST_CONFIG_ID               ((unsigned char)0x5A)
#define MESG_OPEN_RX_SCAN_ID                 ((unsigned char)0x5B)

#define MESG_EXT_CHANNEL_RADIO_FREQ_ID       ((unsigned char)0x5C)  // OBSOLETE: (for 905 radio)
#define MESG_EXT_BROADCAST_DATA_ID           ((unsigned char)0x5D)
#define MESG_EXT_ACKNOWLEDGED_DATA_ID        ((unsigned char)0x5E)
#define MESG_EXT_BURST_DATA_ID               ((unsigned char)0x5F)

#define MESG_CHANNEL_RADIO_TX_POWER_ID       ((unsigned char)0x60)
#define MESG_GET_SERIAL_NUM_ID               ((unsigned char)0x61)
#define MESG_GET_TEMP_CAL_ID                 ((unsigned char)0x62)
#define MESG_SET_LP_SEARCH_TIMEOUT_ID        ((unsigned char)0x63)
#define MESG_SET_TX_SEARCH_ON_NEXT_ID        ((unsigned char)0x64)
#define MESG_SERIAL_NUM_SET_CHANNEL_ID_ID    ((unsigned char)0x65)
#define MESG_RX_EXT_MESGS_ENABLE_ID          ((unsigned char)0x66)  
#define MESG_RADIO_CONFIG_ALWAYS_ID          ((unsigned char)0x67)
#define MESG_ENABLE_LED_FLASH_ID             ((unsigned char)0x68)
#define MESG_XTAL_ENABLE_ID                  ((unsigned char)0x6D)
#define MESG_STARTUP_MESG_ID                 ((unsigned char)0x6F)
#define MESG_AUTO_FREQ_CONFIG_ID             ((unsigned char)0x70)
#define MESG_PROX_SEARCH_CONFIG_ID           ((unsigned char)0x71)

#define MESG_CUBE_CMD_ID                     ((unsigned char)0x80)

#define MESG_GET_PIN_DIODE_CONTROL_ID        ((unsigned char)0x8D)
#define MESG_PIN_DIODE_CONTROL_ID            ((unsigned char)0x8E)
#define MESG_FIT1_SET_AGC_ID                 ((unsigned char)0x8F)

#define MESG_FIT1_SET_EQUIP_STATE_ID         ((unsigned char)0x91)  // *** CONFLICT: w/ Sensrcore, Fit1 will never have sensrcore enabled

// Sensrcore Messages
#define MESG_SET_CHANNEL_INPUT_MASK_ID       ((unsigned char)0x90)
#define MESG_SET_CHANNEL_DATA_TYPE_ID        ((unsigned char)0x91)
#define MESG_READ_PINS_FOR_SECT_ID           ((unsigned char)0x92)
#define MESG_TIMER_SELECT_ID                 ((unsigned char)0x93)
#define MESG_ATOD_SETTINGS_ID                ((unsigned char)0x94)
#define MESG_SET_SHARED_ADDRESS_ID           ((unsigned char)0x95)
#define MESG_ATOD_EXTERNAL_ENABLE_ID         ((unsigned char)0x96)
#define MESG_ATOD_PIN_SETUP_ID               ((unsigned char)0x97)
#define MESG_SETUP_ALARM_ID                  ((unsigned char)0x98)
#define MESG_ALARM_VARIABLE_MODIFY_TEST_ID   ((unsigned char)0x99)
#define MESG_PARTIAL_RESET_ID                ((unsigned char)0x9A)
#define MESG_OVERWRITE_TEMP_CAL_ID           ((unsigned char)0x9B)
#define MESG_SERIAL_PASSTHRU_SETTINGS_ID     ((unsigned char)0x9C)

#define MESG_READ_SEGA_ID                    ((unsigned char)0xA0)
#define MESG_SEGA_CMD_ID                     ((unsigned char)0xA1)
#define MESG_SEGA_DATA_ID                    ((unsigned char)0xA2)
#define MESG_SEGA_ERASE_ID                   ((unsigned char)0xA3)
#define MESG_SEGA_WRITE_ID                   ((unsigned char)0xA4)
#define AVOID_USING_SYNC_BYTES_FOR_MESG_IDS  ((unsigned char)0xA5)

#define MESG_SEGA_LOCK_ID                    ((unsigned char)0xA6)
#define MESG_FLASH_PROTECTION_CHECK_ID       ((unsigned char)0xA7)
#define MESG_UARTREG_ID                      ((unsigned char)0xA8)
#define MESG_MAN_TEMP_ID                     ((unsigned char)0xA9)
#define MESG_BIST_ID                         ((unsigned char)0xAA)
#define MESG_SELFERASE_ID                    ((unsigned char)0xAB)
#define MESG_SET_MFG_BITS_ID                 ((unsigned char)0xAC)
#define MESG_UNLOCK_INTERFACE_ID             ((unsigned char)0xAD)
#define MESG_SERIAL_ERROR_ID                 ((unsigned char)0xAE)
#define MESG_SET_ID_STRING_ID                ((unsigned char)0xAF)

#define MESG_IO_STATE_ID                     ((unsigned char)0xB0)
#define MESG_CFG_STATE_ID                    ((unsigned char)0xB1)
#define MESG_BLOWFUSE_ID                     ((unsigned char)0xB2)
#define MESG_MASTERIOCTRL_ID                 ((unsigned char)0xB3)
#define MESG_PORT_GET_IO_STATE_ID            ((unsigned char)0xB4)
#define MESG_PORT_SET_IO_STATE_ID            ((unsigned char)0xB5)

#define MESG_SLEEP_ID                        ((unsigned char)0xC5)
#define MESG_GET_GRMN_ESN_ID                 ((unsigned char)0xC6)

#define MESG_DEBUG_ID                        ((unsigned char)0xF0)  // use 2 byte sub-index identifier

//////////////////////////////////////////////
// Message Sizes
//////////////////////////////////////////////
#define MESG_INVALID_SIZE                    ((unsigned char)0)

#define MESG_VERSION_SIZE                    ((unsigned char)13)
#define MESG_RESPONSE_EVENT_SIZE             ((unsigned char)3)
#define MESG_CHANNEL_STATUS_SIZE             ((unsigned char)2)

#define MESG_UNASSIGN_CHANNEL_SIZE           ((unsigned char)1)
#define MESG_ASSIGN_CHANNEL_SIZE             ((unsigned char)3)
#define MESG_CHANNEL_ID_SIZE                 ((unsigned char)5)
#define MESG_CHANNEL_MESG_PERIOD_SIZE        ((unsigned char)3)
#define MESG_CHANNEL_SEARCH_TIMEOUT_SIZE     ((unsigned char)2)
#define MESG_CHANNEL_RADIO_FREQ_SIZE         ((unsigned char)2)
#define MESG_CHANNEL_RADIO_TX_POWER_SIZE     ((unsigned char)2)
#define MESG_NETWORK_KEY_SIZE                ((unsigned char)9)
#define MESG_RADIO_TX_POWER_SIZE             ((unsigned char)2)
#define MESG_RADIO_CW_MODE_SIZE              ((unsigned char)3)
#define MESG_RADIO_CW_INIT_SIZE              ((unsigned char)1)
#define MESG_SYSTEM_RESET_SIZE               ((unsigned char)1)
#define MESG_OPEN_CHANNEL_SIZE               ((unsigned char)1)
#define MESG_CLOSE_CHANNEL_SIZE              ((unsigned char)1)
#define MESG_REQUEST_SIZE                    ((unsigned char)2)

#define MESG_CAPABILITIES_SIZE               ((unsigned char)6)
#define MESG_STACKLIMIT_SIZE                 ((unsigned char)2)

#define MESG_SCRIPT_DATA_SIZE                ((unsigned char)10)
#define MESG_SCRIPT_CMD_SIZE                 ((unsigned char)3)

#define MESG_ID_LIST_ADD_SIZE                ((unsigned char)6)
#define MESG_ID_LIST_CONFIG_SIZE             ((unsigned char)3)
#define MESG_OPEN_RX_SCAN_SIZE               ((unsigned char)1)
#define MESG_EXT_CHANNEL_RADIO_FREQ_SIZE     ((unsigned char)3)

#define MESG_RADIO_CONFIG_ALWAYS_SIZE        ((unsigned char)2)
#define MESG_RX_EXT_MESGS_ENABLE_SIZE        ((unsigned char)2)
#define MESG_SET_TX_SEARCH_ON_NEXT_SIZE      ((unsigned char)2)
#define MESG_SET_LP_SEARCH_TIMEOUT_SIZE      ((unsigned char)2)

#define MESG_SERIAL_NUM_SET_CHANNEL_ID_SIZE  ((unsigned char)3)
#define MESG_ENABLE_LED_FLASH_SIZE           ((unsigned char)2)
#define MESG_GET_SERIAL_NUM_SIZE             ((unsigned char)4)
#define MESG_GET_TEMP_CAL_SIZE               ((unsigned char)4)
#define MESG_CLOCK_DRIFT_DATA_SIZE           ((unsigned char)9)

#define MESG_AGC_CONFIG_SIZE                 ((unsigned char)2)
#define MESG_RUN_SCRIPT_SIZE                 ((unsigned char)2)
#define MESG_ANTLIB_CONFIG_SIZE              ((unsigned char)2)
#define MESG_XTAL_ENABLE_SIZE                ((unsigned char)1)
#define MESG_STARTUP_MESG_SIZE               ((unsigned char)1)
#define MESG_AUTO_FREQ_CONFIG_SIZE           ((unsigned char)4)
#define MESG_PROX_SEARCH_CONFIG_SIZE         ((unsigned char)2)

#define MESG_GET_PIN_DIODE_CONTROL_SIZE      ((unsigned char)1)
#define MESG_PIN_DIODE_CONTROL_ID_SIZE       ((unsigned char)2)
#define MESG_FIT1_SET_EQUIP_STATE_SIZE       ((unsigned char)2)
#define MESG_FIT1_SET_AGC_SIZE               ((unsigned char)3)

#define MESG_READ_SEGA_SIZE                  ((unsigned char)2)
#define MESG_SEGA_CMD_SIZE                   ((unsigned char)3)
#define MESG_SEGA_DATA_SIZE                  ((unsigned char)10)
#define MESG_SEGA_ERASE_SIZE                 ((unsigned char)0)
#define MESG_SEGA_WRITE_SIZE                 ((unsigned char)3)
#define MESG_SEGA_LOCK_SIZE                  ((unsigned char)1)
#define MESG_FLASH_PROTECTION_CHECK_SIZE     ((unsigned char)1)
#define MESG_UARTREG_SIZE                    ((unsigned char)2)
#define MESG_MAN_TEMP_SIZE                   ((unsigned char)2)
#define MESG_BIST_SIZE                       ((unsigned char)6)
#define MESG_SELFERASE_SIZE                  ((unsigned char)2)
#define MESG_SET_MFG_BITS_SIZE               ((unsigned char)2)
#define MESG_UNLOCK_INTERFACE_SIZE           ((unsigned char)1)
#define MESG_SET_SHARED_ADDRESS_SIZE         ((unsigned char)3)

#define MESG_GET_GRMN_ESN_SIZE               ((unsigned char)5)

#define MESG_IO_STATE_SIZE                   ((unsigned char)2)
#define MESG_CFG_STATE_SIZE                  ((unsigned char)2)
#define MESG_BLOWFUSE_SIZE                   ((unsigned char)1)
#define MESG_MASTERIOCTRL_SIZE               ((unsigned char)1)
#define MESG_PORT_SET_IO_STATE_SIZE          ((unsigned char)5)

#define MESG_SLEEP_SIZE                      ((unsigned char)1)
#define MESG_EXT_DATA_SIZE                   ((unsigned char)13)

//////////////////////////////////////////////
// PC Application Event Codes
//////////////////////////////////////////////
//NOTE: These events are not generated by the embedded ANT module

#define EVENT_RX_BROADCAST                         ((UCHAR)0x9A)           // returned when module receives broadcast data
#define EVENT_RX_ACKNOWLEDGED                      ((UCHAR)0x9B)           // returned when module receives acknowledged data
#define EVENT_RX_BURST_PACKET                      ((UCHAR)0x9C)           // returned when module receives burst data

#define EVENT_RX_EXT_BROADCAST                     ((UCHAR)0x9D)           // returned when module receives broadcast data
#define EVENT_RX_EXT_ACKNOWLEDGED                  ((UCHAR)0x9E)           // returned when module receives acknowledged data
#define EVENT_RX_EXT_BURST_PACKET                  ((UCHAR)0x9F)           // returned when module receives burst data


#define EVENT_RX_FLAG_BROADCAST                    ((UCHAR)0xA3)          // returned when module receives broadcast data with flag attached
#define EVENT_RX_FLAG_ACKNOWLEDGED                 ((UCHAR)0xA4)          // returned when module receives acknowledged data with flag attached
#define EVENT_RX_FLAG_BURST_PACKET                 ((UCHAR)0xA5)          // returned when module receives burst data with flag attached           


//////////////////////////////////////////////
// Response / Event Codes
//////////////////////////////////////////////
#define RESPONSE_NO_ERROR                          ((UCHAR)0x00)
#define NO_EVENT                                   ((UCHAR)0x00)

#define EVENT_RX_SEARCH_TIMEOUT                    ((UCHAR)0x01)
#define EVENT_RX_FAIL                              ((UCHAR)0x02)
#define EVENT_TX                                   ((UCHAR)0x03)
#define EVENT_TRANSFER_RX_FAILED                   ((UCHAR)0x04)
#define EVENT_TRANSFER_TX_COMPLETED                ((UCHAR)0x05)
#define EVENT_TRANSFER_TX_FAILED                   ((UCHAR)0x06)
#define EVENT_CHANNEL_CLOSED                       ((UCHAR)0x07)
#define EVENT_RX_FAIL_GO_TO_SEARCH                 ((UCHAR)0x08)
#define EVENT_CHANNEL_COLLISION                    ((UCHAR)0x09)
#define EVENT_TRANSFER_TX_START                    ((UCHAR)0x0A)           // a pending transmit transfer has begun

#define EVENT_CHANNEL_ACTIVE                       ((UCHAR)0x0F)

#define EVENT_TRANSFER_TX_NEXT_MESSAGE             ((UCHAR)0x11)           // only enabled in FIT1

#define CHANNEL_IN_WRONG_STATE                     ((UCHAR)0x15)           // returned on attempt to perform an action from the wrong channel state
#define CHANNEL_NOT_OPENED                         ((UCHAR)0x16)           // returned on attempt to communicate on a channel that is not open
#define CHANNEL_ID_NOT_SET                         ((UCHAR)0x18)           // returned on attempt to open a channel without setting the channel ID
#define CLOSE_ALL_CHANNELS                         ((UCHAR)0x19)           // returned when attempting to start scanning mode, when channels are still open

#define TRANSFER_IN_PROGRESS                       ((UCHAR)0x1F)           // returned on attempt to communicate on a channel with a TX transfer in progress
#define TRANSFER_SEQUENCE_NUMBER_ERROR             ((UCHAR)0x20)           // returned when sequence number is out of order on a Burst transfer
#define TRANSFER_IN_ERROR                          ((UCHAR)0x21)
#define TRANSFER_BUSY                              ((UCHAR)0x22)

#define MESSAGE_SIZE_EXCEEDS_LIMIT                 ((UCHAR)0x27)           // returned if a data message is provided that is too large
#define INVALID_MESSAGE                            ((UCHAR)0x28)           // returned when the message has an invalid parameter
#define INVALID_NETWORK_NUMBER                     ((UCHAR)0x29)           // returned when an invalid network number is provided
#define INVALID_LIST_ID                            ((UCHAR)0x30)           // returned when the provided list ID or size exceeds the limit
#define INVALID_SCAN_TX_CHANNEL                    ((UCHAR)0x31)           // returned when attempting to transmit on channel 0 when in scan mode
#define INVALID_PARAMETER_PROVIDED                 ((UCHAR)0x33)           // returned when an invalid parameter is specified in a configuration message

#define EVENT_QUE_OVERFLOW                         ((UCHAR)0x35)           // ANT event que has overflowed and lost 1 or more events

#define EVENT_CLK_ERROR                            ((UCHAR)0x36)           //!! debug XOSC16M

#define SCRIPT_FULL_ERROR                          ((UCHAR)0x40)           // error writing to script, memory is full
#define SCRIPT_WRITE_ERROR                         ((UCHAR)0x41)           // error writing to script, bytes not written correctly
#define SCRIPT_INVALID_PAGE_ERROR                  ((UCHAR)0x42)           // error accessing script page
#define SCRIPT_LOCKED_ERROR                        ((UCHAR)0x43)           // the scripts are locked and can't be dumped

#define NO_RESPONSE_MESSAGE                        ((UCHAR)0x50)           // returned to the Command_SerialMessageProcess function, so no reply message is generated
#define RETURN_TO_MFG                              ((UCHAR)0x51)           // default return to any mesg when the module determines that the mfg procedure has not been fully completed

#define FIT_ACTIVE_SEARCH_TIMEOUT                  ((UCHAR)0x60)           // Fit1 only event added for timeout of the pairing state after the Fit module becomes active
#define FIT_WATCH_PAIR                             ((UCHAR)0x61)           // Fit1 only
#define FIT_WATCH_UNPAIR                           ((UCHAR)0x62)           // Fit1 only

// Internal only events below this point
#define INTERNAL_ONLY_EVENTS                       ((UCHAR)0x80)
#define EVENT_RX                                   ((UCHAR)0x80)           // INTERNAL: Event for a receive message
#define EVENT_NEW_CHANNEL                          ((UCHAR)0x81)           // INTERNAL: EVENT for a new active channel
#define EVENT_PASS_THRU                            ((UCHAR)0x82)           // INTERNAL: Event to allow an upper stack events to pass through lower stacks
#define EVENT_TRANSFER_RX_COMPLETED                ((UCHAR)0x83)           // INTERNAL: Event for RX completed that indicates ANT library is ready for new messasges

#define EVENT_BLOCKED                              ((UCHAR)0xFF)           // INTERNAL: Event to replace any event we do not wish to go out, will also zero the size of the Tx message


typedef struct {
	int	readTimeout;
	int writeTimeout;
	size_t readBufferSize;
}TIOCFG;

typedef struct {
	int (*cbPtr) (const int channel, const int messageId, const unsigned char *payLoad, const size_t dataLength, void *userPtr);
	void *uPtr;			// user variable sent with each event
}TEVENTFUNC;

typedef struct {
	int initOnce;
	
	struct usb_device *hDev;		// libusb handle to the device
	usb_dev_handle *hInst;			// handle to an instance of the device
	int vid, pid;
	int iDevice;					// index to the antstick we're interested in, if more than one are found
	char *strings[3][256];
	TIOCFG ioVar;
	TEVENTFUNC event[EVENTI_TOTAL];
}TLIBANTPLUS;



int libantplus_Open (TLIBANTPLUS *ant, const int reset);
TLIBANTPLUS *libantplus_Init ();
int libantplus_Discover (TLIBANTPLUS *ant, const int index);
void libantplus_Close (TLIBANTPLUS *ant);
int libantplus_ResetSystem (TLIBANTPLUS *ant);
int libantplus_RequestMessage (TLIBANTPLUS *ant, const int channel, const int message);
int libantplus_AssignChannel (TLIBANTPLUS *ant, const int channel, const int type, const int network);
int libantplus_SetChannelId (TLIBANTPLUS *ant, const int channel, const int deviceNum, const int deviceType, const int transmissionType);
int libantplus_SetNetworkKey (TLIBANTPLUS *ant, const int netNumber, const unsigned char *key);
int libantplus_SetChannelSearchTimeout (TLIBANTPLUS *ant, const int channel, const int searchTimeout);
int libantplus_SetChannelPeriod (TLIBANTPLUS *ant, const int channel, const int period);
int libantplus_SetChannelRFFreq (TLIBANTPLUS *ant, const int channel, const int freq);
int libantplus_OpenChannel (TLIBANTPLUS *ant, const int channel);
int libantplus_CloseChannel (TLIBANTPLUS *ant, const int channel);
int libantplus_SetSearchWaveform (TLIBANTPLUS *ant, const int channel, const int wave);

int libantplus_SendBurstTransfer (TLIBANTPLUS *ant, const int channel, const unsigned char *data, const int nunPackets);
int libantplus_SendBurstTransferPacket (TLIBANTPLUS *ant, const int channelSeq, const unsigned char *data);
int libantplus_SendAcknowledgedData (TLIBANTPLUS *ant, const int channel, const unsigned char *data);
int libantplus_SendBroadcastData (TLIBANTPLUS *ant, const int channel, const unsigned char *data);

int libantplus_SendExtBurstTransfer (TLIBANTPLUS *ant, const int channel, const int devNum, const int devType, const int TranType, const unsigned char *data, const int nunPackets);
int libantplus_SendExtBurstTransferPacket (TLIBANTPLUS *ant, const int chanSeq, const int devNum, const int devType, const int TranType, const unsigned char *data);
int libantplus_SendExtAcknowledgedData (TLIBANTPLUS *ant, const int channel, const int devNum, const int devType, const int TranType, const unsigned char *data);
int libantplus_SendExtBroadcastData (TLIBANTPLUS *ant, const int channel, const int devNum, const int devType, const int TranType, const unsigned char *data);

int libantplus_HandleMessages (TLIBANTPLUS *ant);
int libantplus_SetEventFunction (TLIBANTPLUS *ant, const int which, const void *eventFunc, void *userPtr);

void libantplus_AssignResponseFunction (TLIBANTPLUS *ant, void *response_stub, void *buffer_stub);


#endif
