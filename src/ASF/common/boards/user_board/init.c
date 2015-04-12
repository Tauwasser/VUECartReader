/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	PORTD = 0xFFu;
	DDRD = 0x00u;
	PORTB = 0xFFu;
	DDRB = 0x00u;
	
	PORTC = (1u << PINC4) | (1u << PINC5) | (1u << PINC6) | (1u << PINC7) | (1u << PINC1);
	DDRC = (1u << PINC2) | (1u << PINC4) | (1u << PINC5) | (1u << PINC6) | (1u << PINC7);
	
}
