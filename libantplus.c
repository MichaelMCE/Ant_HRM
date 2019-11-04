
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


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <lusb0_usb.h>

#include "libantplus.h"

#define LIBUSB_VENDERID		ANTSTICK_VID
#define LIBUSB_PRODUCTID	ANTSTICK_PID


unsigned char calc_checksum (const unsigned char *buffer, const unsigned char len){

	unsigned char checksum = 0x00;
	for (unsigned char i = 0; i < len; i++)
		checksum ^= buffer[i];
	return checksum;
}

struct usb_device *antstick_find (int index, const int vid, const int pid)
{
    struct usb_bus *usb_bus;
    struct usb_device *dev;
    struct usb_bus *busses = usb_get_busses();
    
    for (usb_bus = busses; usb_bus; usb_bus = usb_bus->next) {
        for (dev = usb_bus->devices; dev; dev = dev->next) {
            if ((dev->descriptor.idVendor == vid) && (dev->descriptor.idProduct == pid)){
				//if (dev->config->interface->altsetting->bInterfaceNumber == 0){
            		if (index-- == 0){
						//printf("found device i:%i 0x%X:0x%X '%s'\n", index+1, dev->descriptor.idVendor, dev->descriptor.idProduct, dev->filename);
                		return dev;
                	}
                //}
			}
        }
    }
    return NULL;
}

usb_dev_handle *antstick_open (TLIBANTPLUS *ant)
{
	usb_dev_handle *usbdev = usb_open(ant->hDev); 
	if (usbdev == NULL){
		printf("failed to open ant device %i: V:0x%.4X P:0x%.4X\n", ant->iDevice, ant->vid, ant->pid);
		printf("%s\n", usb_strerror());
		return NULL;
	}
	
	if (usb_set_configuration(usbdev, 1) < 0){
		printf("error: setting config 1 failed\n");
		printf("%s\n", usb_strerror());
		usb_close(usbdev);
		return NULL;
	}
		
	if (usb_claim_interface(usbdev, 0) < 0){
		printf("error: claiming interface 0 failed\n");
		printf("%s\n", usb_strerror());
		usb_close(usbdev);
		return NULL;
	}
	
	//printf("opened ant device %i: V:0x%.4X P:0x%.4X\n", ant->iDevice, ant->vid, ant->pid);
	return usbdev;
}

int libantplus_Open (TLIBANTPLUS *ant, const int reset)
{
	ant->hInst = antstick_open(ant);
	if (ant->hInst){
		//usb_clear_halt(ant->hInst, EP_OUT);
		//usb_clear_halt(ant->hInst, EP_IN);
		if (reset)
			libantplus_ResetSystem(ant);

		if (usb_get_string_simple(ant->hInst, 1, (char*)ant->strings[0], 255) > 0){
			//if (ant->strings[0][0])
			//	printf("device i:%i string i:1 '%s'\n", ant->iDevice, (char*)ant->strings[0]);
		}
		if (usb_get_string_simple(ant->hInst, 2, (char*)ant->strings[1], 255) > 0){
			//if (ant->strings[1][0])
			//	printf("device i:%i string i:2 '%s'\n", ant->iDevice, (char*)ant->strings[1]);
		}
		if (usb_get_string_simple(ant->hInst, 3, (char*)ant->strings[2], 255) > 0){
			//if (ant->strings[2][0])
			//	printf("device i:%i string i:3 '%s'\n", ant->iDevice, (char*)ant->strings[2]);
		}
	}else{
		printf("ant device not found\n");
	}

	return (ant->hInst != NULL);
}

int libantplus_Discover (TLIBANTPLUS *ant, const int dongleIndex)
{
	usb_find_devices();
	ant->hDev = antstick_find(dongleIndex, ant->vid, ant->pid);
	if (ant->hDev){
		ant->iDevice = dongleIndex;
	}
	return (ant->hDev != NULL);
}

TLIBANTPLUS *libantplus_Init ()
{
	usb_init();
    usb_find_busses();
    usb_find_devices();

	TLIBANTPLUS *ant = (TLIBANTPLUS*)calloc(1, sizeof(TLIBANTPLUS));
	if (ant){
		ant->initOnce = 1;
		ant->iDevice  = -1;
		ant->vid = LIBUSB_VENDERID;
		ant->pid = LIBUSB_PRODUCTID;
		ant->ioVar.readTimeout = READ_TIMEOUT;
		ant->ioVar.writeTimeout = WRITE_TIMEOUT;
		ant->ioVar.readBufferSize = INPUTBUFFERSIZE;
	}
	return ant;
}

void libantplus_Close (TLIBANTPLUS *ant)
{
	if (ant->hInst){
#if 0
		// ensure all pending writes from transmitter are read
		char buffer[2048];
		usb_bulk_read(ant->hInst, EP_IN, buffer, sizeof(buffer), 30);
		usb_bulk_read(ant->hInst, EP_IN, buffer, sizeof(buffer), 30);
		usb_bulk_read(ant->hInst, EP_IN, buffer, sizeof(buffer), 30);
#endif
		usb_release_interface(ant->hInst, 0);
		usb_close(ant->hInst);
		ant->hInst = NULL;
	}
}

int antstick_write (TLIBANTPLUS *ant, const unsigned char *buffer, const size_t len)
{
	const int ret = usb_interrupt_write(ant->hInst, EP_OUT, (char*)buffer, len, ant->ioVar.writeTimeout);
	if (ret != len)
		printf("antstick_write() failed (len:%i)\n", (int)len);

	return (ret == len);
}

int antstick_read (TLIBANTPLUS *ant, unsigned char *buffer, const size_t len)
{
	int total = usb_bulk_read(ant->hInst, EP_IN, (char*)buffer, len, ant->ioVar.writeTimeout);
	
	if (total > 0){
		return total;
	}else if (total == -5){
		printf("antstick_read(): device %i not found or disconnected (-5)\n", ant->iDevice);
		printf("%s\n", usb_strerror());
		return total;
	}else if (total == -116){
		printf("antstick_read(): device %i read error (timeout -116)\n", ant->iDevice);
		return total;
	}

	printf("%s\n", usb_strerror());
	return -1;
}


unsigned char *antstick_findStreamSync (unsigned char *stream, const size_t rlen, int *pos)
{
	// find and sync with input stream
	*pos = 0;
	while (*pos < rlen /*&& *pos < INPUTBUFFERSIZE-3*/){
		if (stream[*pos] == MESG_TX_SYNC)
			return stream + *pos;
		(*pos)++;
	}
	return NULL;
}

int antstick_msgCheckIntegrity (unsigned char *stream, const int len)
{
	// min message length is 5
	if (len < 5) return 0;

	int crc = stream[STREAM_SYNC];
	crc ^= stream[STREAM_LENGTH];
	crc ^= stream[STREAM_MESSAGE];
	int mlen = 0;
	
	do{
		crc ^= stream[STREAM_DATA+mlen];
	}while (++mlen < stream[STREAM_LENGTH]);
		
	//printf("crc == 0x%X: msg crc = 0x%X\n", crc, stream[stream[STREAM_LENGTH] + 3]);
	return (crc == stream[stream[STREAM_LENGTH] + 3]);
}

int antstick_msgGetLength (unsigned char *stream)
{
	// eg; {A4 1 6F 20 EA} = {SYNC DATALEN MSGID DATA CRC}
	return stream[STREAM_LENGTH] + 4;
}

int antstick_registerEventCallback (TLIBANTPLUS *ant, const int which, const void *eventFunc, void *userPtr)
{
	if (which < EVENTI_TOTAL){
		ant->event[which].cbPtr = (void*)eventFunc;
		ant->event[which].uPtr = userPtr;
		return 1;
	}
	
	printf("invalid callback id {%i}\n.", which);
	return 0;
}

int libantplus_SetEventFunction (TLIBANTPLUS *ant, const int which, const void *eventFunc, void *userPtr)
{
	return antstick_registerEventCallback(ant, which, eventFunc, userPtr);
}

int antstick_dispatchMessage (TLIBANTPLUS *ant, const unsigned char *stream, const int len)
{
	return ant->event[EVENTI_MESSAGE].cbPtr(-1, stream[STREAM_MESSAGE], &stream[STREAM_DATA], (size_t)stream[STREAM_LENGTH], ant->event[EVENTI_MESSAGE].uPtr);
}

int antstick_handleMessages (TLIBANTPLUS *ant)
{
	int syncOffset = 0;
	unsigned char buffer[ant->ioVar.readBufferSize];
	unsigned char *stream = buffer;
	
	int tBytes = antstick_read(ant, buffer, ant->ioVar.readBufferSize);
	if (tBytes <= 0) return tBytes;

	while (tBytes){
		stream = antstick_findStreamSync(stream, tBytes, &syncOffset);
		if (stream == NULL){
			printf("stream sync not found {size:%i}\n", tBytes);
			return 0;
		}
		tBytes -= syncOffset;

		if (!antstick_msgCheckIntegrity(stream, tBytes)){
			printf("stream integrity failed {size:%i}\n", tBytes);
			return 0;
		}

		//we have a valid message!
		if (antstick_dispatchMessage(ant, stream, tBytes) == -1){
			printf("quiting..\n");
			return 0;
		}

		int len = antstick_msgGetLength(stream);
		stream += len;
		tBytes -= len;

	}
	return 1;
}

int libantplus_HandleMessages (TLIBANTPLUS *ant)
{
	return antstick_handleMessages(ant);
}

void libantplus_AssignResponseFunction (TLIBANTPLUS *ant, void *response_stub, void *buffer_stub)
{
	printf("libantplus_AssignResponseFunction(): not implemented\n");
}

int libantplus_ResetSystem (TLIBANTPLUS *ant)
{
	unsigned char msg[5];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 1;						// length
	msg[2] = MESG_SYSTEM_RESET_ID;	// msg id
	msg[3] = 0;						// nop
	msg[4] = calc_checksum(msg, 4);	

	// send the message
	return antstick_write(ant, msg, 5);
}

int libantplus_RequestMessage (TLIBANTPLUS *ant, const int channel, const int message)
{
	unsigned char msg[6];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 2;						// length
	msg[2] = MESG_REQUEST_ID;		// msg id
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)message;
	msg[5] = calc_checksum(msg, 5);	

	// send the message
	return antstick_write(ant, msg, 6);
}

int libantplus_AssignChannel (TLIBANTPLUS *ant, const int channel, const int channelType, const int network)
{
	unsigned char msg[7];

	msg[0] = MESG_TX_SYNC;
	msg[1] = 3;
	msg[2] = MESG_ASSIGN_CHANNEL_ID;
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)channelType;
	msg[5] = (unsigned char)network;
	msg[6] = calc_checksum(msg, 6);	

	// send the message
	return antstick_write(ant, msg, 7);
}

int libantplus_SetChannelId (TLIBANTPLUS *ant, const int channel, const int deviceNum, const int deviceType, const int transmissionType)
{
	unsigned char msg[9];

	msg[0] = MESG_TX_SYNC;
	msg[1] = 5;
	msg[2] = MESG_CHANNEL_ID_ID;
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)(deviceNum & 0xFF);
	msg[5] = (unsigned char)(deviceNum >> 8);
	msg[6] = (unsigned char)deviceType;
	msg[7] = (unsigned char)transmissionType;
	msg[8] = calc_checksum(msg, 8);	

	return antstick_write(ant, msg, 9);
}

int libantplus_SetNetworkKey (TLIBANTPLUS *ant, const int netNumber, const unsigned char *key)
{
	unsigned char msg[13];
	
	msg[0] = MESG_TX_SYNC;
	msg[1] = 9;
	msg[2] = MESG_NETWORK_KEY_ID;
	msg[3] = (unsigned char)netNumber;
	msg[4] = key[0];
	msg[5] = key[1];
	msg[6] = key[2];
	msg[7] = key[3];
	msg[8] = key[4];
	msg[9] = key[5];
	msg[10] = key[6];
	msg[11] = key[7];
	msg[12] = calc_checksum(msg, 12); 			// xor checksum
	
	return antstick_write(ant, msg, 13);
}

int libantplus_SetChannelSearchTimeout (TLIBANTPLUS *ant, const int channel, const int searchTimeout)
{
	unsigned char msg[6];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 2;						// length
	msg[2] = MESG_CHANNEL_SEARCH_TIMEOUT_ID;		// msg id
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)searchTimeout;
	msg[5] = calc_checksum(msg, 5);	

	// send the message
	return antstick_write(ant, msg, 6);	
}

int libantplus_SetChannelPeriod (TLIBANTPLUS *ant, const int channel, const int period)
{
	unsigned char msg[7];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 3;						// length
	msg[2] = MESG_CHANNEL_MESG_PERIOD_ID;		// msg id
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)(period & 0xFF);
	msg[5] = (unsigned char)(period >> 8);
	msg[6] = calc_checksum(msg, 6);	

	// send the message
	return antstick_write(ant, msg, 7);
}

int libantplus_SetChannelRFFreq (TLIBANTPLUS *ant, const int channel, const int freq)
{
	unsigned char msg[6];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 2;						// length
	msg[2] = MESG_CHANNEL_RADIO_FREQ_ID;		// msg id
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)freq;
	msg[5] = calc_checksum(msg, 5);	

	// send the message
	return antstick_write(ant, msg, 6);
}

int libantplus_SetSearchWaveform (TLIBANTPLUS *ant, const int channel, const int wave)
{
	unsigned char msg[7];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 3;						// length
	msg[2] = MESG_SEARCH_WAVEFORM_ID;		// msg id
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)wave & 0xFF;
	msg[5] = (unsigned char)wave >> 8;
	msg[6] = calc_checksum(msg, 6);	

	// send the message
	return antstick_write(ant, msg, 7);
}

int libantplus_OpenChannel (TLIBANTPLUS *ant, const int channel)
{
	unsigned char msg[5];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 1;						// length
	msg[2] = MESG_OPEN_CHANNEL_ID;	// msg id
	msg[3] = (unsigned char)channel;
	msg[4] = calc_checksum(msg, 4);	

	// send the message
	return antstick_write(ant, msg, 5);
}

int libantplus_CloseChannel (TLIBANTPLUS *ant, const int channel)
{
	unsigned char msg[5];

	msg[0] = MESG_TX_SYNC;			// sync
	msg[1] = 1;						// length
	msg[2] = MESG_CLOSE_CHANNEL_ID;	// msg id
	msg[3] = (unsigned char)channel;
	msg[4] = calc_checksum(msg, 4);	

	// send the message
	return antstick_write(ant, msg, 5);
}

int libantplus_SendBurstTransferPacket (TLIBANTPLUS *ant, const int channelSeq, const unsigned char *data)
{
	unsigned char msg[13];
	
	msg[0] = MESG_TX_SYNC;
	msg[1] = 9;
	msg[2] = MESG_BURST_DATA_ID;
	msg[3] = (unsigned char)channelSeq;
	msg[4] = data[0];
	msg[5] = data[1];
	msg[6] = data[2];
	msg[7] = data[3];
	msg[8] = data[4];
	msg[9] = data[5];
	msg[10] = data[6];
	msg[11] = data[7];
	msg[12] = calc_checksum(msg, 12); 			// xor checksum
	
	return antstick_write(ant, msg, 13);
}

int libantplus_SendBurstTransfer (TLIBANTPLUS *ant, const int channel, const unsigned char *data, const int nunPackets)
{
	int ret = 0;
	int seq = 0;
	
	for (int i = 0; i < nunPackets; i++){
		if (i == nunPackets-1) seq |= 0x04;
		ret = libantplus_SendBurstTransferPacket(ant, (seq<<5) | (channel&0x1F), data+(i<<3));
		seq = (seq+1)&0x03;
	}
	return ret;
}

int libantplus_SendBroadcastData (TLIBANTPLUS *ant, const int channel, const unsigned char *data)
{
	unsigned char msg[13];
	
	msg[0] = MESG_TX_SYNC;
	msg[1] = 9;
	msg[2] = MESG_BROADCAST_DATA_ID;
	msg[3] = (unsigned char)channel;
	msg[4] = data[0];
	msg[5] = data[1];
	msg[6] = data[2];
	msg[7] = data[3];
	msg[8] = data[4];
	msg[9] = data[5];
	msg[10] = data[6];
	msg[11] = data[7];
	msg[12] = calc_checksum(msg, 12);
	
	return antstick_write(ant, msg, 13);
}

int libantplus_SendAcknowledgedData (TLIBANTPLUS *ant, const int channel, const unsigned char *data)
{
	unsigned char msg[13];
	
	msg[0] = MESG_TX_SYNC;
	msg[1] = 9;
	msg[2] = MESG_ACKNOWLEDGED_DATA_ID;
	msg[3] = (unsigned char)channel;
	msg[4] = data[0];
	msg[5] = data[1];
	msg[6] = data[2];
	msg[7] = data[3];
	msg[8] = data[4];
	msg[9] = data[5];
	msg[10] = data[6];
	msg[11] = data[7];
	msg[12] = calc_checksum(msg, 12);
	
	return antstick_write(ant, msg, 13);
}

int libantplus_SendExtAcknowledgedData (TLIBANTPLUS *ant, const int channel, const int devNum, const int devType, const int TranType, const unsigned char *data)
{
	unsigned char msg[17];
	
	msg[0] = MESG_TX_SYNC;
	msg[1] = 13;
	msg[2] = MESG_EXT_ACKNOWLEDGED_DATA_ID;
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)(devNum & 0xFF);
	msg[5] = (unsigned char)(devNum >> 8);
	msg[6] = (unsigned char)devType;
	msg[7] = (unsigned char)TranType;
	msg[8] = data[0];
	msg[9] = data[1];
	msg[10] = data[2];
	msg[11] = data[3];
	msg[12] = data[4];
	msg[13] = data[5];
	msg[14] = data[6];
	msg[15] = data[7];
	msg[16] = calc_checksum(msg, 16);
	
	return antstick_write(ant, msg, 17);
}

int libantplus_SendExtBroadcastData (TLIBANTPLUS *ant, const int channel, const int devNum, const int devType, const int TranType, const unsigned char *data)
{
	unsigned char msg[17];
	
	msg[0] = MESG_TX_SYNC;
	msg[1] = 13;
	msg[2] = MESG_EXT_BROADCAST_DATA_ID;
	msg[3] = (unsigned char)channel;
	msg[4] = (unsigned char)(devNum & 0xFF);
	msg[5] = (unsigned char)(devNum >> 8);
	msg[6] = (unsigned char)devType;
	msg[7] = (unsigned char)TranType;
	msg[8] = data[0];
	msg[9] = data[1];
	msg[10] = data[2];
	msg[11] = data[3];
	msg[12] = data[4];
	msg[13] = data[5];
	msg[14] = data[6];
	msg[15] = data[7];
	msg[16] = calc_checksum(msg, 16);
	
	return antstick_write(ant, msg, 17);
}

int libantplus_SendExtBurstTransferPacket (TLIBANTPLUS *ant, const int chanSeq, const int devNum, const int devType, const int TranType, const unsigned char *data)
{
	unsigned char msg[17];
	
	msg[0] = MESG_TX_SYNC;
	msg[1] = 13;
	msg[2] = MESG_EXT_BROADCAST_DATA_ID;
	msg[3] = (unsigned char)chanSeq;
	msg[4] = (unsigned char)(devNum & 0xFF);
	msg[5] = (unsigned char)(devNum >> 8);
	msg[6] = (unsigned char)devType;
	msg[7] = (unsigned char)TranType;
	msg[8] = data[0];
	msg[9] = data[1];
	msg[10] = data[2];
	msg[11] = data[3];
	msg[12] = data[4];
	msg[13] = data[5];
	msg[14] = data[6];
	msg[15] = data[7];
	msg[16] = calc_checksum(msg, 16);
	
	return antstick_write(ant, msg, 17);
}

int libantplus_SendExtBurstTransfer (TLIBANTPLUS *ant, const int channel, const int devNum, const int devType, const int tranType, const unsigned char *data, const int nunPackets)
{
	int ret = 0;
	int seq = 0;
	
	for (int i = 0; i < nunPackets; i++){
		if (i == nunPackets-1) seq |= 0x04;
		ret = libantplus_SendExtBurstTransferPacket(ant, (seq<<5) | (channel&0x1F), devNum, devType, tranType, data+(i<<3));
		seq = (seq+1)&0x03;
	}
	return ret;
}
