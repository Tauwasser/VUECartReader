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
#ifndef VUE_H_
#define VUE_H_

#include <stdint.h>
#include "global.h"

#define VUE_CART_READER_PL          (1u << PINC2)
#define VUE_AD0                     (1u << PIND0)
#define VUE_AD1                     (1u << PIND1)
#define VUE_AD2                     (1u << PIND2)
#define VUE_AD3                     (1u << PIND3)
#define VUE_AD4                     (1u << PIND4)
#define VUE_AD5                     (1u << PIND5)
#define VUE_AD6                     (1u << PIND6)
#define VUE_AD7                     (1u << PIND7)
#define VUE_AD8                     (1u << PINB0)
#define VUE_AD9                     (1u << PINB1)
#define VUE_AD10                    (1u << PINB2)
#define VUE_AD11                    (1u << PINB3)
#define VUE_AD12                    (1u << PINB4)
#define VUE_AD13                    (1u << PINB5)
#define VUE_AD14                    (1u << PINB6)
#define VUE_AD15                    (1u << PINB7)

#define VUE_nUWR                    (1u << PINC4)
#define VUE_nLWR                    (1u << PINC7)
#define VUE_nOE                     (1u << PINC6)

#define VUE_CTRL                    IOPORTC
#define VUE_ADL                     IOPORTD
#define VUE_ADH                     IOPORTB

#define VUE_CART_READER_nSTATUS  (1ul << 23)
#define VUE_nROM_CE              (1ul << 24)
#define VUE_nRAM_CS              (1ul << 25)
#define VUE_nES                  (1ul << 27)

#define VUE_FLAG_MASK               (VUE_CART_READER_nSTATUS | VUE_nROM_CE | VUE_nRAM_CS | VUE_nES)
#define VUE_ADDRESS_MASK_LO         (0x007FFFul)
#define VUE_ADDRESS_MASK            (0x7FFFFFul)

//////////////////////////////////////////

void vue_write_address_full(uint32_t address);

#if (VUE_CART_READER_ALWAYS_FULL_ADDRESS)
#define vue_write_address_lo15(_ADDRESS_) vue_write_address(_ADDRESS_)
#else
void vue_write_address_lo15(uint32_t address);
#endif

void vue_read_data_16bit(uint32_t* address, uint8_t* buffer, uint8_t length);
void vue_write_data_16bit(uint32_t* address, uint8_t* buffer, uint8_t length);
void vue_read_data_8bit_lo(uint32_t* address, uint8_t* buffer, uint8_t length);
void vue_write_data_8bit_lo(uint32_t* address, uint8_t* buffer, uint8_t length);
void vue_read_data_8bit_hi(uint32_t* address, uint8_t* buffer, uint8_t length);
void vue_write_data_8bit_hi(uint32_t* address, uint8_t* buffer, uint8_t length);
void vue_write_address(uint32_t address);
void vue_inc_address(uint32_t* address);


#endif /* VUE_H_ */