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
#ifndef LENGTHABBREV_HPP_
#define LENGTHABBREV_HPP_

#include <stdint.h>

struct length_abbrev {

private:

	uint32_t length_;

public:

	void set_value(const uint32_t value) { length_ = value; }
	uint32_t get_value() const { return length_; }

};

std::istream& operator>>(std::istream& in, length_abbrev& v);
std::ostream& operator<<(std::ostream& out, const length_abbrev& v);

#endif /* LENGTHABBREV_HPP_ */
