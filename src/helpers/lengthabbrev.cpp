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
#include <boost/limits.hpp>
#include <boost/program_options.hpp>

#include "lengthabbrev.hpp"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct table_entry {

	char unit_symbol;
	uint32_t unit_value;

};

static const table_entry length_abbrev_table[] = {

	{'B', 1ul},
	{'b', 1ul},
	{'K', 1024ul},
	{'k', 1024ul},
	{'M', 1024ul * 1024ul},
	{'m', 1024ul * 1024ul},
};


std::istream& operator>>(std::istream& in, length_abbrev& v) {

	using boost::program_options::validation_error;

	uint32_t token_scalar;
	size_t i;
	char token_unit;

	in >> token_scalar;

	if (in.fail())
		throw validation_error(validation_error::invalid_option_value);

	in >> token_unit;

	if (in.fail())
		throw validation_error(validation_error::invalid_option_value);

	for (i = 0; i < ARRAY_SIZE(length_abbrev_table); i++) {

		if (length_abbrev_table[i].unit_symbol == token_unit) {

			v.set_value(length_abbrev_table[i].unit_value * token_scalar);
			return in;

		}

	}

	throw validation_error(validation_error::invalid_option_value);

	return in;

}
std::ostream& operator<<(std::ostream& out, const length_abbrev& v) {

	// traverse table and find best match
	size_t i = 0;
	size_t best_i = 0;

	uint32_t value = v.get_value();
	uint32_t min_scalar = ::std::numeric_limits<uint32_t>::max();

	for (; i < ARRAY_SIZE(length_abbrev_table); i++) {

		if (!(value % length_abbrev_table[i].unit_value)) {

			if (min_scalar > (value / length_abbrev_table[i].unit_value)) {
				min_scalar = value / length_abbrev_table[i].unit_value;
				best_i = i;
			}

		}

	}

	out << (value / length_abbrev_table[best_i].unit_value) << length_abbrev_table[best_i].unit_symbol;

	return out;

}
