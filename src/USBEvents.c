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

#include "global.h"
#include "vue.h"
#include "vue_usb_ctrl.h"

volatile uint32_t address = VUE_CART_READER_nSTATUS | VUE_nROM_CE | VUE_nRAM_CS | VUE_nES;
uint8_t i;
static uint8_t my_buffer[0x20u];
extern uint32_t bulk_reset;

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the Mass Storage management task started.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	/* Setup Bulk Endpoints */
	Endpoint_ConfigureEndpoint(VUE_CART_READER_IN_EP_ADDR,  EP_TYPE_BULK, VUE_CART_READER_EP_SIZE, 1);
	Endpoint_ConfigureEndpoint(VUE_CART_READER_OUT_EP_ADDR, EP_TYPE_BULK, VUE_CART_READER_EP_SIZE, 1);
	
}

void EVENT_USB_Device_ControlRequest(void) {

	switch (USB_ControlRequest.bmRequestType) {
		
	case REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE:

		Endpoint_ClearSETUP();
	
		switch (USB_ControlRequest.bRequest) {
			
		case REQ_ENTER_BOOTLOADER:
		
			Endpoint_ClearStatusStage();
			enter_bootloader();
			return;
			
		case REQ_RESET_ALL:
		
			bulk_reset = 0xFFFFFFFFul;
			break;
			
		case REQ_WRITE_DEBUG:
			
			switch (USB_ControlRequest.wValue) {
			case 0x0000:
				bulk_reset = 0xFFFFFFFFul;
				break;
			default:
				break;
			}
			break;
		
		default:
		
			break;
			
		}

		Endpoint_ClearStatusStage();
		break;
		
	case REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE:
		
		Endpoint_ClearSETUP();
		
		switch (USB_ControlRequest.bRequest) {
			
		case REQ_GET_STATUS:
		
			
			break;
		
		case REQ_READ_HEADER:
		
			address &= VUE_FLAG_MASK;
			address |= (0xFFFFFDE0ul >> 1) & VUE_ADDRESS_MASK;
			address &= ~VUE_nROM_CE;
		
			for (i = 0; i < 0x20u;) {
				
				vue_write_address_full(address++);
				VUE_CTRL->out &= ~VUE_nOE;
				my_buffer[i++] = VUE_ADL->in;
				my_buffer[i++] = VUE_ADH->in;
				VUE_CTRL->out |= VUE_nOE;
				
			}
			
			address |= VUE_nROM_CE;
			vue_write_address_full(address);
		
			Endpoint_Write_Control_Stream_LE(my_buffer, 0x20u);
			break;
		
		case REQ_READ_DEBUG:
			{
			
				uint32_t return_value = 0xDEADBEEFu;
		
				switch (USB_ControlRequest.wValue) {
				default:
					break;
				}
				
				Endpoint_Write_Control_Stream_BE(&return_value, sizeof(return_value));
				break;
				
			}
		
		default:
		
			break;
			
		}
		
		Endpoint_ClearStatusStage();
		break;
	
	default:
	
		break;
		
	}

}