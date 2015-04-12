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
#ifndef VUECARTREADER_HPP_
#define VUECARTREADER_HPP_

#include <iostream>
#include <libusb.h>

namespace VUECartReader {

struct vue_command {

	uint8_t access_mode;
	uint8_t target_region;
	uint8_t dummy4;
	uint8_t dummy5;
	uint32_t operation;
	uint32_t address;
	uint32_t length;
	uint32_t dummy0;
	uint32_t dummy1;
	uint32_t dummy2;
	uint32_t dummy3;

} __attribute__((packed));

class Reader {

public:
	Reader(uint16_t vid, uint16_t pid);
	virtual ~Reader();
	void invoke_bootloader();

	void bulk_read(std::ostream& out, const uint32_t address, const uintmax_t length);
	void bulk_write(std::istream& in, const uint32_t address, const uintmax_t length);
	void exec_debug_cmd(const uint16_t wValue, const uint16_t wIndex, const boolean is_read);

private:

public:

private:

	uint16_t vid_;
	uint16_t pid_;
	libusb_device_handle* h_device_;

};

} /* namespace VUECartReader */

#endif /* VUECARTREADER_HPP_ */
