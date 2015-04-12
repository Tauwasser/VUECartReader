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
#ifndef DESC_H
#define DESC_H

#include <LUFA/Drivers/USB/USB.h>
#include <avr/pgmspace.h>

typedef USB_Descriptor_Device_t USBDeviceDescriptor;
typedef USB_Descriptor_String_t USBStringDescriptor;
typedef USB_Descriptor_Configuration_Header_t USBConfigurationDescriptorHeader;
typedef USB_Descriptor_Interface_t USBInterfaceDescriptor;
typedef USB_Descriptor_Endpoint_t USBEndpointDescriptor;

typedef struct {
	USBConfigurationDescriptorHeader ConfigurationHeader;
	USBInterfaceDescriptor           Interface;
	USBEndpointDescriptor            InEndpoint;
	USBEndpointDescriptor            OutEndpoint;
} USBConfigurationDescriptor;

#define VUE_CART_READER_IN_EP_ADDR  (ENDPOINT_DIR_IN  | 0x03)
#define VUE_CART_READER_OUT_EP_ADDR (ENDPOINT_DIR_OUT | 0x04)
#define VUE_CART_READER_EP_SIZE     64

#endif
