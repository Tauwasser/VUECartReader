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
#include <stdint.h>
#include <stdexcept>

#include <boost/format.hpp>

#include "Reader.hpp"

#define VUE_CART_READER_IN_EP_ADDR  3
#define VUE_CART_READER_OUT_EP_ADDR 4

namespace VUECartReader {

Reader::
Reader(uint16_t vid, uint16_t pid) :
	vid_(vid),
	pid_(pid) {

	int result;

	// TODO: This is only temporary!
	h_device_ = libusb_open_device_with_vid_pid(NULL, vid_, pid_);

	if (!h_device_)
		throw std::runtime_error(
			boost::str(boost::format("Error opening device with VID/PID %04X/%04X.") % vid_ % pid_)
		);

	result = libusb_set_auto_detach_kernel_driver(h_device_, 1);

	if (result < 0 && result != LIBUSB_ERROR_NOT_SUPPORTED)
		throw std::runtime_error(
			boost::str(boost::format("libusb_set_auto_detach_kernel_driver failed: %s") % libusb_strerror((enum libusb_error) result))
		);

	result = libusb_claim_interface(h_device_, 0);

	if (result < 0)
		throw std::runtime_error(
			boost::str(boost::format("libusb_claim_interface failed: %s") % libusb_strerror((enum libusb_error) result))
		);

	// This should probably use the value read from the device!
	result = libusb_set_configuration(h_device_, 1);

	if (result < 0)
		throw std::runtime_error(
			boost::str(boost::format("libusb_set_configuration failed: %s") % libusb_strerror((enum libusb_error) result))
		);

	result = libusb_set_interface_alt_setting(h_device_, 0, 0);

	if (result < 0)
		throw std::runtime_error(
			boost::str(boost::format("libusb_set_interface_alt_setting failed: %s") % libusb_strerror((enum libusb_error) result))
		);

}

Reader::
~Reader() {

	if (h_device_) {
		libusb_release_interface(h_device_, 0);
		libusb_close(h_device_);
	}

	h_device_ = NULL;

}

void
Reader::
invoke_bootloader() {

	int result;

	result = libusb_control_transfer(h_device_, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, 0x40u, 0x0000u, 0x0000, 0x00u, 0x00, 20);

	if (result < 0 && result != LIBUSB_ERROR_PIPE)
		throw std::runtime_error(
			boost::str(boost::format("libusb_control_transfer failed: %s") % libusb_strerror((enum libusb_error) result))
		);

}

void
Reader::
bulk_read(std::ostream& out, const uint32_t address, const uintmax_t length) {

	uint8_t buffer[0x400u];
	int current_length;
	int result;

	uintmax_t num_bulk_reads = length / sizeof(buffer);
	int trailing_length = length % sizeof(buffer);

	struct vue_command cmd;

	memset(&cmd, 0, sizeof(cmd));

	cmd.access_mode = 0x00u; /* 16bit */
	cmd.target_region = 0x01u; /* ROM */
	cmd.operation = htonl(0x00ul); /* Read */
	cmd.address = htonl(address);
	cmd.length = htonl(length);

	current_length = 0;
	result = libusb_bulk_transfer(h_device_, LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE | VUE_CART_READER_OUT_EP_ADDR, (unsigned char*)&cmd, sizeof(cmd), &current_length, 200);

	if (result < 0)
		std::cout << "Whoops0: " << libusb_strerror((enum libusb_error) result) << "\n";

	for (; num_bulk_reads; num_bulk_reads--) {

		current_length = 0;
		result = libusb_bulk_transfer(h_device_, LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_DEVICE | VUE_CART_READER_IN_EP_ADDR, buffer, sizeof(buffer), &current_length, 200);

		// TODO: unstall EP + throw exception
		if (result < 0)
			std::cout << "Whoops: " << libusb_strerror((enum libusb_error) result) << "\n";

		// TODO: Error handling for stream
		out.write((char*) buffer, sizeof(buffer));

	}

	if (trailing_length) {

		current_length = 0;
		result = libusb_bulk_transfer(h_device_, LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_DEVICE | VUE_CART_READER_IN_EP_ADDR, buffer, trailing_length, &current_length, 200);

		out.write((char*) buffer, trailing_length);

	}

}

void
Reader::
bulk_write(std::istream& in, const uint32_t address, const uintmax_t length) {

	uint8_t buffer[0x400u];
	int current_length;
	int result;

	uintmax_t num_bulk_writes = length / sizeof(buffer);
	int trailing_length = length % sizeof(buffer);

	struct vue_command cmd;

	memset(&cmd, 0, sizeof(cmd));

	cmd.access_mode = 0x00u; /* 16bit */
	cmd.target_region = 0x01u; /* ROM */
	cmd.operation = htonl(0x01ul); /* Write */
	cmd.address = htonl(address);
	cmd.length = htonl(length);

	current_length = 0;
	result = libusb_bulk_transfer(h_device_, LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE | VUE_CART_READER_OUT_EP_ADDR, (unsigned char*)&cmd, sizeof(cmd), &current_length, 200);

	if (result < 0)
		std::cout << "Whoops0: " << libusb_strerror((enum libusb_error) result) << "\n";

	for (; num_bulk_writes; num_bulk_writes--) {

		// TODO: Error handling for stream
		in.read((char*) buffer, sizeof(buffer));

		current_length = 0;
		result = libusb_bulk_transfer(h_device_, LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE | VUE_CART_READER_OUT_EP_ADDR, buffer, sizeof(buffer), &current_length, 200);

		// TODO: unstall EP + throw exception
		if (result < 0)
			std::cout << "Whoops: " << libusb_strerror((enum libusb_error) result) << "\n";

	}

	if (trailing_length) {

		in.read((char*) buffer, trailing_length);

		current_length = 0;
		result = libusb_bulk_transfer(h_device_, LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE | VUE_CART_READER_OUT_EP_ADDR, buffer, trailing_length, &current_length, 200);

	}

}

void
Reader::
exec_debug_cmd(const uint16_t wValue, const uint16_t wIndex, const boolean is_read) {

	int result;
	uint32_t debug_value;

	if (!is_read)
		result = libusb_control_transfer(h_device_, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, 0x20u, wValue, wIndex, 0x00u, 0x00, 20);
	else
		result = libusb_control_transfer(h_device_, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, 0x20u, wValue, wIndex, (unsigned char*)&debug_value, sizeof(debug_value), 20);

	if (result < 0)
		std::cout << "Whoops: " << libusb_strerror((enum libusb_error) result) << "\n";

	if ((result >= 0) && is_read)
		std::cout << boost::str(boost::format("Debug Read: %08X.\n") % ntohl(debug_value));
}

} /* namespace VUECartReader */
