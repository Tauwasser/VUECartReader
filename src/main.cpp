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
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/locale.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>

#include "helpers/int_t_hex.hpp"
#include "helpers/hex_output.hpp"
#include "helpers/lengthabbrev.hpp"

#include "VUECartReader/Reader.hpp"
#include "VUECartReader/Mapper.hpp"
#include "VUECartReader/Expansion.hpp"
#include "VUECartReader/Target.hpp"

#define VUE_CART_READER_DEFAULT_VID 0x03EBu
#define VUE_CART_READER_DEFAULT_PID 0x2040u
#define VUE_CART_READER_SOFTWARE_VERSION "1.0"

using namespace std;
using namespace boost::program_options;
using namespace boost::interprocess;
using namespace VUECartReader;

/* Auxiliary functions for checking input for validity. */

bool is_default_map(boost::program_options::variables_map& map) {

	bool result = true;

	for (auto it = map.begin(); it != map.end(); it++) {

		if (!(it->second.defaulted()))
			return false;

	}

	return result;

}

/* Function used to check that 'opt1' and 'opt2' are not specified
 * at the same time.
 */
void conflicting_options(
	const variables_map& vm,
	const char* opt1,
	const char* opt2) {

	if (vm.count(opt1) && !vm[opt1].defaulted()
		&& vm.count(opt2) && !vm[opt2].defaulted()
		)
		throw logic_error(string("Conflicting options '") + opt1 + "' and '" + opt2 + "'.");
}

/* Function used to check that of 'for_what' is specified, then
 * 'required_option' is specified too.
 */
void option_dependency(
	const variables_map& vm,
	const char* for_what,
	const char* required_option) {

	if (vm.count(for_what) && !vm[for_what].defaulted())
		if (vm.count(required_option) == 0 || vm[required_option].defaulted())
			throw logic_error(string("Option '") + for_what + "' requires option '" + required_option + "'.");
}

int main(int argc, char** argv)
{

	namespace po = boost::program_options;

	int return_code = 0;
	int result = LIBUSB_ERROR_OTHER;
	Reader* instance = NULL;

	cout << "VUE Cart Reader Software v" << VUE_CART_READER_SOFTWARE_VERSION << "\n";

	po::options_description top_opt("");
	po::options_description cart_opt("Cartridge Options");
	po::options_description io_opt("Input/Output Options");
	po::options_description misc_opt("Miscellaneous Options");
	po::options_description debug_opt("Debug Options");

	cart_opt.add_options()
		("mapper", po::value<Mapper>()->default_value(Mapper::NONE), "Cartridge Mapper\n"
		                                                             "  none - no mapper")
		("expansion", po::value<Expansion>()->default_value(Expansion::NONE), "Expansion Area IC\n"
		                                                                      "  none - no expansion")
		;

	io_opt.add_options()
		("start,s", po::value<uint_t_hex<24> >(), "Start byte address in target memory. (24-bit)")
		("length,l", po::value<uint_t_hex<32> >(), "Length in bytes for I/O access. (32-bit)")
		("abbrev-length,L", po::value<length_abbrev>(), "Abbreviated length in bytes for I/O access.\n"
		                                                "  1B - 1 byte\n"
		                                                "  1K - 1 kilobyte\n"
		                                                "  1M - 1 megabyte")
		("target,t", po::value<Target>()->default_value(Target::ROM), "Target\n"
		                                                              "  ram - RAM\n"
		                                                              "  rom - ROM\n"
		                                                              "  exp - Expansion")
		("read-header", "Display game header only")
		("input,i", po::value<std::string>(), "Input file path")
		("output,o", po::value<std::string>(), "Output file path")
		;

	misc_opt.add_options()
		("bootloader", "Enter bootloader")
		("no-reset", "Don't reset cartridge before I/O access.")
		("vid", po::value<uint_t_hex<16> >()->default_value(VUE_CART_READER_DEFAULT_VID), "Vendor ID of VUE Cart Reader")
		("pid", po::value<uint_t_hex<16> >()->default_value(VUE_CART_READER_DEFAULT_PID), "Product ID of VUE Cart Reader")
		("help,h", "Print usage message")
		;

	debug_opt.add_options()
		("debug", "Do debugging")
		("ddir", po::value<uint16_t>()->default_value(0x0001u), "Debug Direction\n"
		                                                       "  0 - Read\n"
		                                                       "  1 - Write")
		("dval0", po::value<uint16_t>()->default_value(0x0000u), "Debug Value wValue")
		("dval1", po::value<uint16_t>()->default_value(0x0000u), "Debug Value wIndex")
		("dtest0", po::value<std::basic_string<char> >()->default_value("Narf"), "Debug String")
		;

	top_opt.add(cart_opt);
	top_opt.add(io_opt);
	top_opt.add(misc_opt);
	top_opt.add(debug_opt);

	try {

		po::variables_map var_map;
		po::store(po::parse_command_line(argc, argv, top_opt), var_map);
		po::notify(var_map);

		if (var_map.empty() || is_default_map(var_map) || var_map.count("help")) {

			cout << top_opt << "\n";
			return 0;
		}

		/**
		 * Check command line arguments
		 * mutual exclusion/dependencies etc.
		 */

		conflicting_options(var_map, "bootloader", "input");
		conflicting_options(var_map, "bootloader", "output");
		conflicting_options(var_map, "bootloader", "read-header");
		conflicting_options(var_map, "input", "output");
		conflicting_options(var_map, "read-header", "input");
		conflicting_options(var_map, "read-header", "output");

		conflicting_options(var_map, "length", "abbrev-length");

		// Combine length settings
		if (var_map.count("abbrev-length"))
			var_map.insert(
				po::variables_map::value_type(string("length"),
				                              po::variable_value(uint_t_hex<32>(var_map["abbrev-length"].as<length_abbrev>().get_value()),
				                                                 false
				                              )
				)
			);

		option_dependency(var_map, "vid", "pid");
		option_dependency(var_map, "input", "start");
		option_dependency(var_map, "output", "start");
		option_dependency(var_map, "start", "length");

		if (var_map.count("bootloader") == 0
		    && var_map.count("input") == 0
		    && var_map.count("output") == 0
		    && var_map.count("read-header") == 0
		    && var_map.count("debug") == 0
		    ) {
			cout << "Nothing to do. Exiting." << "\n";
			cout << top_opt << "\n";
			return 0;
		}

		/**
		 * Initialize libusb
		 */
		result = libusb_init(NULL);
//		libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);

		if (result < 0)
			throw new runtime_error(
				boost::str(boost::format("libusb_init failed with %s.") % libusb_strerror((enum libusb_error) result))
			);

		instance = new Reader(
			var_map["vid"].as<uint_t_hex<16>>().get_value(),
			var_map["pid"].as<uint_t_hex<16>>().get_value()
			);

		if (var_map.count("debug")) {

			cout << "Debugging: " << "wValue " << var_map["dval0"].as<uint16_t>() << " wIndex " << var_map["dval1"].as<uint16_t>() << "\n";
			instance->exec_debug_cmd(var_map["dval0"].as<uint16_t>(), var_map["dval1"].as<uint16_t>(), var_map["ddir"].as<uint16_t>() == 0x00u);

		} else if (var_map.count("bootloader")) {

			instance->invoke_bootloader();

		} else if (var_map.count("read-header")) {

			uint8_t header[0x20];
			bufferstream header_ostream(reinterpret_cast<char*>(header), sizeof(header));

			instance->bulk_read(header_ostream, 0xFFFDE0ul, 0x20ul);

			// Convert Shift-JIS title to UTF-8
			boost::locale::generator gen;
			std::locale loc = gen.generate("ja_JP.shiftjis");
			std::string orig_sjis = std::string((const char*) &header[0], 20);
			std::string conv_utf8 = boost::locale::conv::to_utf<char>(orig_sjis, loc);

			cout <<
				boost::str(boost::format(
					"Raw Game Header Data:\n"
					"%s\n"
					"Internal Name: "
					"%s\n"
					"Reserved:      "
					"%02X%02X%02X%02X%02X\n"
					"Maker:         "
					"%c%c\n"
					"Game Code:     "
					"%s\n"
					"Game Version:  "
					"1.%d"
					)
					% hex_output((const char*)header, sizeof(header))
					% conv_utf8
					% (header[0x14] & 0xFFu) % (header[0x15] & 0xFFu) % (header[0x16] & 0xFFu) % (header[0x17] & 0xFFu) % (header[0x18] & 0xFFu)
					% header[0x19] % header[0x1A]
					% std::string((const char*) &header[0x1B], 4)
					% (header[0x1F] & 0xFFu)
				);

		} else if (var_map.count("input") || var_map.count("output")) {

			boolean is_output = (var_map.count("output") > 0);

			// Open Input/Output File in binary mode, truncating output file
			string filename = is_output ? var_map["output"].as<string>() : var_map["input"].as<string>();
			fstream fs(filename, (is_output ? fstream::out | fstream::trunc : fstream::in) | fstream::binary);

			if (!fs.good()) {
				throw runtime_error(boost::str(boost::format("Error opening file %s for %s.") % filename % (is_output ? "output" : "input")));
			}

			// Set correct mode
//			instance->set_mode();

			// Seek to correct offset
			//instance->seek(var_map["start"].as<uint_t_hex<24> >().get_value());

			// Begin reading/writing

			if (is_output)
				instance->bulk_read(
					fs,
					var_map["start"].as<uint_t_hex<24> >().get_value(),
					var_map["length"].as<uint_t_hex<32> >().get_value()
					);
			else
				instance->bulk_write(
					fs,
					var_map["start"].as<uint_t_hex<24> >().get_value(),
					var_map["length"].as<uint_t_hex<32> >().get_value()
					);

			// Close Input/Output File
			fs.close();

		}

	} catch(const logic_error& e) {

		cerr << "Error: " << e.what() << "\n";
		cout << top_opt << "\n";
		return_code = -2;

	} catch(const exception& e) {

		cerr << "Error: " << e.what() << "\n";
		return_code = -1;
	}
	catch(...) {
		cerr << "Exception of unknown type!\n";
		return_code = -1;
	}

	delete instance;

	if (result == LIBUSB_SUCCESS)
		libusb_exit(NULL);

	return return_code;

}
