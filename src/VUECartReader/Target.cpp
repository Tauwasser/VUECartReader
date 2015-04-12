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
#include <boost/program_options.hpp>
#include <boost/assign.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

#include "Target.hpp"
#include "helpers/program_option_enum.hpp"

using namespace boost::bimaps;

namespace VUECartReader {

typedef bimap<multiset_of<Target>, unordered_set_of<std::string> > target_table_type;

static const target_table_type target_table = boost::assign::list_of<target_table_type::relation>
	(Target::RAM,       "ram")
	(Target::ROM,       "rom")
	(Target::EXPANSION, "exp")
	(Target::EXPANSION, "expansion")
	;

PO_ENUM_DEF_EXTRACT(Target, target_table)
PO_ENUM_DEF_INSERT(Target, target_table)

} /* namespace VUECartReader */
