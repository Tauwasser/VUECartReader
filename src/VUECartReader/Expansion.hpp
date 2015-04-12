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
#ifndef EXPANSION_HPP_
#define EXPANSION_HPP_

#include "helpers/program_option_enum.hpp"

namespace VUECartReader {

enum class Expansion : uint8_t {
	NONE = 0x00u,
};

PO_ENUM_DECL_EXTRACT(Expansion);
PO_ENUM_DECL_INSERT(Expansion);

} /* namespace VUECartReader */

#endif /* EXPANSION_HPP_ */
