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
#ifndef PROGRAM_OPTION_ENUM_HPP_
#define PROGRAM_OPTION_ENUM_HPP_

#include "boost/program_options.hpp"

#define PO_ENUM_DECL_EXTRACT(_ENUM_T_) std::istream& operator>>(std::istream& in, _ENUM_T_& mapper)
#define PO_ENUM_DECL_INSERT(_ENUM_T_) std::ostream& operator<<(std::ostream& out, const _ENUM_T_& mapper)


#define PO_ENUM_DEF_EXTRACT(_ENUM_T_, _BIMAP_) \
	std::istream& operator>>(std::istream& in, _ENUM_T_& mapper) \
	{ \
	\
		using boost::program_options::validation_error; \
	\
		std::string token; \
		in >> token; \
	\
		auto it = _BIMAP_.right.find(token); \
	\
		if (it == _BIMAP_.right.end()) \
			throw validation_error(validation_error::invalid_option_value); \
	\
		mapper = it->second; \
	\
		return in; \
	}


#define PO_ENUM_DEF_INSERT(_ENUM_T_, _BIMAP_) \
	std::ostream& operator<<(std::ostream& out, const _ENUM_T_& mapper) \
	{ \
	\
		using boost::program_options::validation_error; \
	\
		auto it = _BIMAP_.left.find(mapper); \
	\
		if (it == _BIMAP_.left.end()) \
			throw validation_error(validation_error::invalid_option_value); \
	\
		out << it->second; \
	\
		return out; \
	}

#endif /* PROGRAM_OPTION_ENUM_HPP_ */
