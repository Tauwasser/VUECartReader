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

#include "vue.h"

#if (VUE_CART_READER_ALWAYS_FULL_ADDRESS)
#else
void vue_write_address_lo15(uint32_t address) {
	
	// Change AD bus from PU IN to OUT
	VUE_ADL->dir = 0xFFu;
	VUE_ADH->dir = 0xFFu;
	
	// A0-A7
	VUE_ADL->out = (address & 0xFFu);
	// A8-A14 + don't toggle second load
	VUE_ADH->out = (address >> 8) & (VUE_ADDRESS_MASK_LO >> 8);
	
	// Write A0-A14
	VUE_CTRL->out |= VUE_CART_READER_PL;
	VUE_CTRL->out &= ~VUE_CART_READER_PL;
	
	// Change AD bus from OUT to PU IN
	VUE_ADL->out = 0xFFu;
	VUE_ADL->dir = 0x00u;
	VUE_ADH->out = 0xFFu;
	VUE_ADH->dir = 0x00u;
	
}

#endif

void vue_write_address_full(uint32_t address) {

	// Change AD bus from PU IN to OUT
	VUE_ADL->dir = 0xFFu;
	VUE_ADH->dir = 0xFFu;
	
	// A0-A7
	VUE_ADL->out = (address & 0xFFu);
	// A8-A14 + Toggle second load
	VUE_ADH->out = VUE_AD15 | ((address >> 8) & (VUE_ADDRESS_MASK_LO >> 8));

	// Write A0-A14
	VUE_CTRL->out |= VUE_CART_READER_PL;
	
	// Flags
	VUE_ADL->out = (address >> 23) & (VUE_FLAG_MASK >> 23);
	// A15-A21 + keep toggle
	VUE_ADH->out = VUE_AD15 | ((address >> 15) & (VUE_ADDRESS_MASK >> 15));
	
	// Write Flags + A15-A22
	VUE_ADH->out = (address >> 15) & (VUE_ADDRESS_MASK >> 15);
	VUE_CTRL->out &= ~VUE_CART_READER_PL;
	
	// Change AD bus from OUT to PU IN
	VUE_ADL->out = 0xFFu;
	VUE_ADL->dir = 0x00u;
	VUE_ADH->out = 0xFFu;
	VUE_ADH->dir = 0x00u;

}

void vue_read_data_16bit(uint32_t* address, uint8_t* buffer, uint8_t length) {

	uint8_t i;
	
	for (i = 0; i < length; i += 2) {
		
		vue_write_address(*address);
		VUE_CTRL->out &= ~VUE_nOE;
		// Increment Address instead of using nops for wait states
		vue_inc_address(address);
		*buffer++ = VUE_ADL->in;
		*buffer++ = VUE_ADH->in;
		
		VUE_CTRL->out |= VUE_nOE;
		
	}
	
}

void vue_write_data_16bit(uint32_t* address, uint8_t* buffer, uint8_t length) {

	uint8_t i;
	
	for (i = 0; i < length; i += 2) {
		
		vue_write_address(*address);
		// D0-D7
		VUE_ADL->out = *buffer++;
		// D8-D15
		VUE_ADH->out = *buffer++;
		// Change AD bus from PU IN to OUT
		VUE_ADL->dir = 0xFFu;
		VUE_ADH->dir = 0xFFu;
		
		VUE_CTRL->out &= ~(VUE_nUWR | VUE_nLWR);
		// Increment Address instead of using nops for wait states
		vue_inc_address(address);
		VUE_CTRL->out |= (VUE_nUWR | VUE_nLWR);
		
	}
	
	// Change AD bus from OUT to PU IN
	VUE_ADL->out = 0xFFu;
	VUE_ADL->dir = 0x00u;
	VUE_ADH->out = 0xFFu;
	VUE_ADH->dir = 0x00u;
	
}

void vue_read_data_8bit_lo(uint32_t* address, uint8_t* buffer, uint8_t length) {

	uint8_t i;
	
	for (i = 0; i < length; i++) {
		
		vue_write_address(*address);
		VUE_CTRL->out &= ~VUE_nOE;
		// Increment Address instead of using nops for wait states
		vue_inc_address(address);
		*buffer++ = VUE_ADL->in;
		
		VUE_CTRL->out |= VUE_nOE;
		
	}
	
}

void vue_write_data_8bit_lo(uint32_t* address, uint8_t* buffer, uint8_t length) {

	uint8_t i;
		
	for (i = 0; i < length; i++) {
		
		vue_write_address(*address);
		
		// D0-D7
		VUE_ADL->out = *buffer++;
		// Change AD bus from PU IN to OUT
		VUE_ADL->dir = 0xFFu;
		
		VUE_CTRL->out &= ~VUE_nLWR;
		// Increment Address instead of using nops for wait states
		vue_inc_address(address);
		VUE_CTRL->out |= VUE_nLWR;
		
	}
	
	// Change AD bus from OUT to PU IN
	VUE_ADL->out = 0xFFu;
	VUE_ADL->dir = 0x00u;
	
}

void vue_read_data_8bit_hi(uint32_t* address, uint8_t* buffer, uint8_t length) {

	uint8_t i;
	
	for (i = 0; i < length; i++) {
		
		vue_write_address(*address);
		VUE_CTRL->out &= ~VUE_nOE;
		// Increment Address instead of using nops for wait states
		vue_inc_address(address);
		*buffer++ = VUE_ADH->in;
		
		VUE_CTRL->out |= VUE_nOE;
		
	}
	
}

void vue_write_data_8bit_hi(uint32_t* address, uint8_t* buffer, uint8_t length) {

	uint8_t i;
	
	for (i = 0; i < length; i++) {
		
		vue_write_address(*address);
		// D0-D7
		VUE_ADH->out = *buffer++;
		// Change AD bus from PU IN to OUT
		VUE_ADL->dir = 0xFFu;
		
		VUE_CTRL->out &= ~VUE_nUWR;
		// Increment Address instead of using nops for wait states
		vue_inc_address(address);
		VUE_CTRL->out |= VUE_nUWR;
		
	}
	
	// Change AD bus from OUT to PU IN
	VUE_ADH->out = 0xFFu;
	VUE_ADH->dir = 0x00u;
	
}


void vue_write_address(uint32_t address) {
	
	//Decide if only 15-bit or full write
	if (address & VUE_ADDRESS_MASK_LO)
		vue_write_address_lo15(address);
	else
		vue_write_address_full(address);
	
}

void vue_inc_address(uint32_t* address) {
		
	*address = (*address & VUE_FLAG_MASK) | (((*address & VUE_ADDRESS_MASK) + 1) & VUE_ADDRESS_MASK);
	
}