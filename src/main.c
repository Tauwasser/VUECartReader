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
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <asf.h>

#include <LUFA/Common/Common.h>
#include <LUFA/Drivers/USB/USB.h>

#include "USBDescriptors.h"
#include "VUEEvents.h"
#include "global.h"
#include "vue.h"

uint32_t bootloader_boot_key ATTR_NO_INIT;

#define MAGIC_BOOT_KEY            0xDC42ACCA
#define BOOTLOADER_START_ADDRESS  (FLASH_SIZE_BYTES - BOOTLOADER_SEC_SIZE_BYTES)

void bootloader_jmp_chk(void) ATTR_INIT_SECTION(3);
void bootloader_jmp_chk(void)
{	
	
	uint8_t old_mcusr = MCUSR;
	
	MCUSR &= ~(1u << WDRF);
	wdt_disable();

	// If the reset source was the watch dog timer and the key is correct, clear it and jump to the bootloader
	if ((old_mcusr & (1 << WDRF)) && (bootloader_boot_key == MAGIC_BOOT_KEY))
	{
		bootloader_boot_key = 0;
		((void (*)(void)) BOOTLOADER_START_ADDRESS)();
	}
}

void enter_bootloader(void)
{
	// If USB is used, detach from the bus and reset it
	USB_Disable();

	// Disable all interrupts
	cli();

	// Wait two seconds for the USB detachment to register on the host
	delay_ms(2000);

	// Set the bootloader key to the magic value and force a reset
	bootloader_boot_key = MAGIC_BOOT_KEY;
	wdt_enable(WDTO_250MS);
	for (;;);
}

uint32_t bulk_reset = 0xFFFFFFFFul;
extern fsm_state cur_vue_state;

int main (void)
{
	
	uint32_t tmp_bulk_reset;
	
	MCUSR &= ~(1u << WDRF);
	wdt_disable();
	clock_prescale_set(clock_div_1);
	
	board_init();
	USB_Init();
	
	vue_write_address_full(VUE_CART_READER_nSTATUS | VUE_nROM_CE | VUE_nRAM_CS | VUE_nES);

	sei();
	
	while (true) {
	
		USB_USBTask();
		
		tmp_bulk_reset = bulk_reset;
		VUE_Task();
		
		if (tmp_bulk_reset) {
			
			/* write default state */
			VUE_CTRL->out |= VUE_nOE | VUE_nUWR | VUE_nLWR;
			VUE_CTRL->out &= ~VUE_CART_READER_PL;
			vue_write_address_full(VUE_CART_READER_nSTATUS | VUE_nROM_CE | VUE_nRAM_CS | VUE_nES);
			
			/* Reset FSM */
			cur_vue_state = IDLE;
			/* Clear Endpoints */
			Endpoint_ResetEndpoint(VUE_CART_READER_OUT_EP_ADDR);
			Endpoint_ResetEndpoint(VUE_CART_READER_IN_EP_ADDR);
			
			Endpoint_SelectEndpoint(VUE_CART_READER_OUT_EP_ADDR);
			Endpoint_ClearStall();
			Endpoint_ResetDataToggle();
			Endpoint_SelectEndpoint(VUE_CART_READER_IN_EP_ADDR);
			Endpoint_ClearStall();
			Endpoint_ResetDataToggle();
			Endpoint_AbortPendingIN();
			
			bulk_reset = 0x00000000ul;
		}
		
	}
	
}
