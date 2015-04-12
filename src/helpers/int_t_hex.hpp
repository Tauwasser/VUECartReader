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
#ifndef INTHEX_HPP_
#define INTHEX_HPP_

#include <iostream>
#include <iomanip>
#include <boost/integer.hpp>
#include <boost/limits.hpp>
#include <boost/program_options.hpp>

template<typename T, int Bits>
class base_int_t_hex {

public:
	base_int_t_hex() : my_value(T()) {};
	base_int_t_hex(const T& v) : my_value(v) {};
	const T get_value() const { return my_value; }
	void set_value(const T& v) { my_value = v; }


private:

	T my_value;

};

template<int Bits>
class int_t_hex : public base_int_t_hex<typename boost::int_t<Bits>::fast, Bits> {

	typedef typename boost::int_t<Bits>::fast value_type;

public:

	using base_int_t_hex<value_type, Bits>::base_int_t_hex;

};

template<int Bits>
class uint_t_hex : public base_int_t_hex<typename boost::uint_t<Bits>::fast, Bits> {

	typedef typename boost::uint_t<Bits>::fast value_type;

public:

	using base_int_t_hex<value_type, Bits>::base_int_t_hex;

};

template<typename T, int Bits>
std::istream& operator>>(std::istream& in, base_int_t_hex<T, Bits>& v)
{

	using boost::program_options::validation_error;

	T token;
	in >> std::hex >> token;

	if (in.fail())
		throw validation_error(validation_error::invalid_option_value);

	if (::std::numeric_limits<T>::is_signed) {

		if ((token > ((1uLL << (Bits - 1)) - 1uLL)) || token < -(1uLL << (Bits - 1)))
			throw validation_error(validation_error::invalid_option_value);

	} else {

		if (token >= (1uLL << Bits))
			throw validation_error(validation_error::invalid_option_value);

	}

	v.set_value(token);
	return in;
}

template<typename T, int Bits>
std::ostream& operator<<(std::ostream& out, const base_int_t_hex<T, Bits>& v)
{
	return out << std::hex << std::setfill('0') << std::setw((Bits + 3) / 4) << std::uppercase << +v.get_value();
}

#endif /* INTHEX_HPP_ */
