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
#ifndef HEX_OUTPUT_HPP_
#define HEX_OUTPUT_HPP_

#include <stdint.h>
#include <iostream>

struct hex_output {

	hex_output(const char* buffer, ssize_t length) : buf_(buffer), len_(length) {};

	friend std::ostream& operator<<(std::ostream& out, const hex_output& me);

private:

	const char* buf_;
	ssize_t len_;


};

std::ostream& operator<<(std::ostream& out, const hex_output& me);

#endif /* HEX_OUTPUT_HPP_ */
