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
#include "USBDescriptors.h"

#define QUOTE(_X_)              #_X_
#define EXPAND_AND_QUOTE(_X_)   QUOTE(_X_)
#define VUE_CART_READER_VERSION 1.00
#define VUE_CART_READER_VID     0x03EBu
#define VUE_CART_READER_PID     0x2040u

static const USBDeviceDescriptor PROGMEM deviceDescriptor = {
	.Header = {
		.Size = sizeof(USBDeviceDescriptor),
		.Type = DTYPE_Device
	},
	.USBSpecification = VERSION_BCD(02.00),
	.Class = 0xff,
	.SubClass = 0xff,
	.Protocol = 0xff,
	.Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
	.VendorID = VUE_CART_READER_VID,
	.ProductID = VUE_CART_READER_PID,
	.ReleaseNumber = VERSION_BCD(VUE_CART_READER_VERSION),
	.ManufacturerStrIndex = 0x01,
	.ProductStrIndex = 0x02,
	.SerialNumStrIndex = NO_DESCRIPTOR,
	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

static const USBConfigurationDescriptor PROGMEM configurationDescriptor = {
	.ConfigurationHeader = {
		.Header = {
			.Size = sizeof(USBConfigurationDescriptorHeader),
			.Type = DTYPE_Configuration
		},
		.TotalConfigurationSize = sizeof(USBConfigurationDescriptor),
		.TotalInterfaces = 1,
		.ConfigurationNumber = 1,
		.ConfigurationStrIndex = NO_DESCRIPTOR,
		.ConfigAttributes = USB_CONFIG_ATTR_RESERVED,
		.MaxPowerConsumption = USB_CONFIG_POWER_MA(500)
	},
	.Interface = {
		.Header = {
			.Size = sizeof(USBInterfaceDescriptor),
			.Type = DTYPE_Interface
		},
		.InterfaceNumber = 0,
		.AlternateSetting = 0,
		.TotalEndpoints = 2,
		.Class = 0xff,
		.SubClass = 0xff,
		.Protocol = 0xff,
		.InterfaceStrIndex = NO_DESCRIPTOR
	},
	.InEndpoint = {
		.Header = {
			.Size = sizeof(USBEndpointDescriptor),
			.Type = DTYPE_Endpoint
		},
		.EndpointAddress = VUE_CART_READER_IN_EP_ADDR,
		.Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize = VUE_CART_READER_EP_SIZE,
		.PollingIntervalMS = 0xFF
	},
	.OutEndpoint = {
		.Header = {
			.Size = sizeof(USBEndpointDescriptor),
			.Type = DTYPE_Endpoint
		},
		.EndpointAddress = VUE_CART_READER_OUT_EP_ADDR,
		.Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize = VUE_CART_READER_EP_SIZE,
		.PollingIntervalMS = 0xFF
	}
};

static const USBStringDescriptor PROGMEM languageString = {
	.Header = {
		.Size = USB_STRING_LEN(1),
		.Type = DTYPE_String
	},
	.UnicodeString = {LANGUAGE_ID_ENG}
};

static const USBStringDescriptor PROGMEM manufacturerString = {
	.Header = {
		.Size = USB_STRING_LEN(9),
		.Type = DTYPE_String
	},
	.UnicodeString          = L"Tauwasser"
};

static const USBStringDescriptor PROGMEM productString = {
	.Header = {
		.Size = USB_STRING_LEN(17+sizeof(EXPAND_AND_QUOTE(VUE_CART_READER_VERSION))),
		.Type = DTYPE_String
	},
	.UnicodeString          = L"VUE Cart Reader v" EXPAND_AND_QUOTE(VUE_CART_READER_VERSION)
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, const void** const DescriptorAddress) {
	const uint8_t descriptorType = (wValue >> 8);
	const uint8_t descriptorNumber = (wValue & 0xFF);

	void *address = NULL;
	uint16_t size = NO_DESCRIPTOR;

	switch ( descriptorType ) {
		case DTYPE_Device:
			address = (void*)&deviceDescriptor;
			size = sizeof(USBDeviceDescriptor);
			break;
		case DTYPE_Configuration: 
			address = (void*)&configurationDescriptor;
			size = sizeof(USBConfigurationDescriptor);
			break;
		case DTYPE_String: 
			switch ( descriptorNumber ) {
				case 0x00: 
					address = (void*)&languageString;
					size = pgm_read_byte(&languageString.Header.Size);
					break;
				case 0x01: 
					address = (void*)&manufacturerString;
					size = pgm_read_byte(&manufacturerString.Header.Size);
					break;
				case 0x02: 
					address = (void*)&productString;
					size = pgm_read_byte(&productString.Header.Size);
					break;
			}
			break;
	}
	
	*DescriptorAddress = address;		
	return size;
}
