/* VUE Cart Reader Microcontroller Software
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
#include <iostream>
#include <iomanip>
#include <boost/format.hpp>

#include "hex_output.hpp"

using namespace std;

std::ostream&
operator<<(std::ostream& out, const hex_output& me) {

	ssize_t length = me.len_;
	const char* buffer = me.buf_;
	const char* start_line;
	ssize_t col;
	ssize_t row;

	out << hex << setfill('0') << uppercase;

	for (row = 0; row < (me.len_ + 15) / 16; row++) {

		start_line = buffer;

		for (col = 0; col < 16 && col < length; col++) {

			out << setw(2) << (*buffer++ & 0xFFu) << " ";

		}

		buffer = start_line;

		// Align data
		if (col != 16) {
			for(; col < 16; col++) {

				out << "   ";

			}
		}

		out << "\t";

		for (col = 0; col < 16 && col < length; col++) {

			if (isprint(*buffer))
				out << *buffer;
			else
				out << ".";

			buffer++;

		}

		length -= 16;
		out << "\n";
	}

	return out;

}
