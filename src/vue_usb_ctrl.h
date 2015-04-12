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
#ifndef VUE_USB_CTRL_H_
#define VUE_USB_CTRL_H_

enum vue_usb_host2device_ctrl_codes {
	
	REQ_WRITE_DEBUG      = 0x20u,
	REQ_ENTER_BOOTLOADER = 0x40u,
	REQ_RESET_ALL        = 0xA0u,
		
};

enum vue_usb_device2host_ctrl_codes {

	REQ_READ_DEBUG       = 0x20u,
	REQ_READ_HEADER      = 0x50u,
	
	REQ_GET_STATUS       = 0x61u,
	
};

#endif /* VUE_USB_CTRL_H_ */