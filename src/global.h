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
#ifndef GLOBAL_H_
#define GLOBAL_H_

struct avr_gpio {

	uint8_t in;
	uint8_t dir;
	uint8_t out;
	
};

#define IOPORTB ((volatile struct avr_gpio*)&PINB)
#define IOPORTC ((volatile struct avr_gpio*)&PINC)
#define IOPORTD ((volatile struct avr_gpio*)&PIND)

void enter_bootloader(void);

#endif /* GLOBAL_H_ */