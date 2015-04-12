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
#ifndef VUEEVENTS_H_
#define VUEEVENTS_H_

typedef enum fsm_state {

	IDLE = 0,
	READ,
	WRITE,
	
} fsm_state;

#define OP_OFFSET                 (READ - IDLE)
#define IS_OPERATION_DEFINED(_X_) ((READ - OP_OFFSET) <= _X_ && _X_ <= (WRITE - OP_OFFSET))

enum access_type {
	ACCESS_16BIT = 0,
	ACCESS_8BIT_LO,
	ACCESS_8BIT_HI
};

#define IS_ACCESS_TYPE_DEFINED(_X_) (ACCESS_16BIT <= _X_ && _X_ <= ACCESS_8BIT_HI)

enum target {
	TARGET_RAM = 0,
	TARGET_ROM,
	TARGET_EXPANSION
};

#define IS_TARGET_DEFINED(_X_) (TARGET_RAM <= _X_ && _X_ <= TARGET_EXPANSION)

struct vue_command {
	
	uint8_t access_mode;
	uint8_t target_region;
	uint8_t dummy4;
	uint8_t dummy5;
	uint32_t operation;
	uint32_t address;
	uint32_t length;
	uint32_t dummy0;
	uint32_t dummy1;
	uint32_t dummy2;
	uint32_t dummy3;
	
} __attribute__((packed));

void VUE_Task(void);
void VUE_Command_Task(void);
void VUE_BulkWrite_Task(void);
void VUE_BulkRead_Task(void);

#endif /* VUEEVENTS_H_ */