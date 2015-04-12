/* VUE Cart Reader Microcontroller Firmware
 * Copyright (C) 2015 Tauwasser (tauwasser@tauwasser.eu)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>

#include <avr/io.h>
#include <asf.h>
#include <LUFA/LUFA/Drivers/USB/USB.h>

#include "USBDescriptors.h"
#include "VUEEvents.h"

#include "vue.h"

extern uint32_t bulk_reset;

fsm_state cur_vue_state = IDLE;

static uint8_t bulk_buffer[VUE_CART_READER_EP_SIZE];
static struct vue_command cur_command;
static uint32_t cur_cmd_phase = 0;
static uint32_t bulk_address;
static void (*bulk_access_func)(uint32_t*, uint8_t*, uint8_t);

static const uint32_t target_select_map[] = {
	/* TARGET_RAM */       VUE_CART_READER_nSTATUS | VUE_nROM_CE | VUE_nES,
	/* TARGET_ROM */       VUE_CART_READER_nSTATUS | VUE_nRAM_CS | VUE_nES,
	/* TARGET_EXPANSION */ VUE_CART_READER_nSTATUS | VUE_nROM_CE | VUE_nRAM_CS,
};

void VUE_Task(void) {
		
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;
	
	if (bulk_reset)
		return;

	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(VUE_CART_READER_OUT_EP_ADDR);
	
	/* Abort if no data has been sent from the host */
	if (Endpoint_IsOUTReceived()) {

		switch (cur_vue_state) {
		
		case WRITE:
			VUE_BulkWrite_Task();
			break;
			
		case IDLE:
		
			VUE_Command_Task();
			break;
			
		default:
			/* Break */
			Endpoint_ResetEndpoint(VUE_CART_READER_OUT_EP_ADDR);
			Endpoint_StallTransaction();
			break;
			
		}

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearOUT();
		
	}

	/* Select the Data In endpoint */
	Endpoint_SelectEndpoint(VUE_CART_READER_IN_EP_ADDR);
	
	if (Endpoint_IsINReady()) {
		
		/* Check if the endpoint is currently full */
		if (!(Endpoint_IsReadWriteAllowed())) {
		
			/* Clear the endpoint bank to send its contents to the host */
			Endpoint_ClearIN();

			/* Wait until the endpoint is ready for more data */
			if (Endpoint_WaitUntilReady())
				return;
		
		}
		
		switch (cur_vue_state) {
			
		case READ:
			VUE_BulkRead_Task();
			break;
		default:
			/* Ignore */
			break;
			
		}

		/* If the endpoint is full, send its contents to the host */
		if (!(Endpoint_IsReadWriteAllowed()))
			Endpoint_ClearIN();
			
	}
	
}

void VUE_Command_Task(void) {
	
	uint16_t BytesTransferred;
	
	/* Read in command */
	BytesTransferred = 0;
	while (Endpoint_Read_Stream_LE((char*) &cur_command, (sizeof(cur_command)), &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		/* Check if the current command is being aborted by the host */
		if (bulk_reset)
			return;
	}
	
	/* correct BigEndian entries */
	cur_command.address = BE32_TO_CPU(cur_command.address);
	cur_command.length = BE32_TO_CPU(cur_command.length);
	cur_command.operation = BE32_TO_CPU(cur_command.operation);
	cur_command.dummy0 = BE32_TO_CPU(cur_command.dummy0);
	cur_command.dummy1 = BE32_TO_CPU(cur_command.dummy1);
	cur_command.dummy2 = BE32_TO_CPU(cur_command.dummy2);
	cur_command.dummy3 = BE32_TO_CPU(cur_command.dummy3);
	
	/* clamp address range */
	cur_command.address &= (VUE_ADDRESS_MASK << 1) | 1;
	
	/* input validation */
	if (!(IS_ACCESS_TYPE_DEFINED(cur_command.access_mode)
		&& IS_TARGET_DEFINED(cur_command.target_region)
		&& IS_OPERATION_DEFINED(cur_command.operation))) {
		
		Endpoint_ResetEndpoint(VUE_CART_READER_OUT_EP_ADDR);
		Endpoint_StallTransaction();
		return;
			
	}
	
	/* no length, no operation */
	if (!cur_command.length)
		return;
	
	cur_vue_state = cur_command.operation + OP_OFFSET;
	cur_cmd_phase = 0;
	
	switch (cur_vue_state) {
	case READ:
		VUE_BulkRead_Task();
		break;
	case WRITE:
		VUE_BulkWrite_Task();
		break;
	default:
		break;
		
	}
	
}

void VUE_BulkWrite_Task(void) {
	
	uint16_t BytesTransferred;
	uint32_t length = VUE_CART_READER_EP_SIZE;
	
	switch (cur_cmd_phase) {
	
	case 0:
	
		switch (cur_command.access_mode) {
			case ACCESS_16BIT:
			bulk_access_func = &vue_write_data_16bit;
			break;
			case ACCESS_8BIT_LO:
			bulk_access_func = &vue_write_data_8bit_lo;
			break;
			case ACCESS_8BIT_HI:
			bulk_access_func = &vue_write_data_8bit_hi;
			break;
			default:
			bulk_access_func = &vue_write_data_16bit;
			break;
		}
	
		/* turn busy lamp on and select target */
		bulk_address = target_select_map[cur_command.target_region] | (cur_command.address >> 1);
		bulk_address &= ~VUE_CART_READER_nSTATUS;
	
		vue_write_address_full(bulk_address);
	
		cur_cmd_phase++;
			break;
	
	case 1:
	
		if (cur_command.length < VUE_CART_READER_EP_SIZE)
			length = cur_command.length;
	
		/* read bytes from host */
		BytesTransferred = 0;
		while (Endpoint_Read_Stream_LE((char*) bulk_buffer, length, &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
		{
			/* Check if the current command is being aborted by the host */
			if (bulk_reset)
				return;
		}
	
		/* write to GEC */
		bulk_access_func(&bulk_address, bulk_buffer, length);
	
		/* adjust total length */
		cur_command.length -= length;
	
		if (cur_command.length)
			break;
	
	default:
	
		vue_write_address_full(bulk_address | VUE_FLAG_MASK);
		cur_vue_state = IDLE;
		break;
		
	}
	
}

void VUE_BulkRead_Task(void) {
	
	uint16_t BytesTransferred;
	uint32_t length = VUE_CART_READER_EP_SIZE;
	
	switch (cur_cmd_phase) {
	
	case 0:
	
		switch (cur_command.access_mode) {
		case ACCESS_16BIT:
			bulk_access_func = &vue_read_data_16bit;
			break;
		case ACCESS_8BIT_LO:
			bulk_access_func = &vue_read_data_8bit_lo;
			break;
		case ACCESS_8BIT_HI:
			bulk_access_func = &vue_read_data_8bit_hi;
			break;
		default:
			bulk_access_func = &vue_read_data_16bit;
			break;
		}
		
		/* turn busy lamp on and select target */
		bulk_address = target_select_map[cur_command.target_region] | (cur_command.address >> 1);
		bulk_address &= ~VUE_CART_READER_nSTATUS;
		
		vue_write_address_full(bulk_address);
		
		cur_cmd_phase++;
		break;
		
	case 1:
	
		if (cur_command.length < VUE_CART_READER_EP_SIZE)
			length = cur_command.length;
	
		/* read from GEC */
		bulk_access_func(&bulk_address, bulk_buffer, length);
		
		/* transfer to host */
		BytesTransferred = 0;
		while (Endpoint_Write_Stream_LE((const char*) bulk_buffer, length, &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
		{
			/* Check if the current command is being aborted by the host */
			if (bulk_reset)
				return;
		}
		
		/* adjust total length */
		cur_command.length -= length;
		
		if (cur_command.length)
			break;
		
	default:
	
		vue_write_address_full(bulk_address | VUE_FLAG_MASK);
	
		Endpoint_ClearIN();
		cur_vue_state = IDLE;
		break;
		
	}
	
}