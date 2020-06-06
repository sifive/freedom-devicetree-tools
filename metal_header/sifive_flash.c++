/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_flash.h>

sifive_flash::sifive_flash(std::ostream &os, const fdt &dtb)
    	: Device(os, dtb, "sifive,flash")
{
	num_flash = 0;
	dtb.match(
	std::regex(compat_string),
	[&](node n) {
	num_flash += 1;
	});
}

void sifive_flash::include_headers() {
	dtb.match(std::regex(compat_string),
			[&](node n) { emit_include(compat_string); });
}

void sifive_flash::create_defines()
{
	dtb.match(
	std::regex(compat_string),
	[&](node n) {
	uint32_t baud = 0;
	uint32_t chip = 0;
	
	if (n.field_exists("label")) {
		std::string str = n.get_fields<std::string>("label").back();
		os << "#define " << "METAL_FLASH_LABEL" << " " << "\"" << str << "\"" << "\n\n";
	}

	if (n.field_exists("jedec-id")){
		auto id = n.get_fields<std::tuple<uint32_t, uint32_t, uint32_t>>("jedec-id");
		if (!id.empty()) {
			uint32_t id0 = std::get<0>(id.front());
			uint32_t id1 = std::get<1>(id.front());
			uint32_t id2 = std::get<2>(id.front());

			emit_def("METAL_JEDEC_ID_0", std::to_string(id0));
			emit_def("METAL_JEDEC_ID_1", std::to_string(id1));
			emit_def("METAL_JEDEC_ID_2", std::to_string(id2));
		}
	}

	if (n.field_exists("baud")) {
		baud = n.get_fields<uint32_t>("baud").back();	
		emit_def("METAL_FLASH_BAUD", std::to_string(baud));
	 }
	
	 if (n.field_exists("chip-select")) {
		chip = n.get_fields<uint32_t>("chip-select").back();
		emit_def("METAL_FLASH_CHIP_SELECT", std::to_string(chip));
	 }

	 if (n.field_exists("spi-max-frequency")) {
		 uint32_t freq = n.get_fields<uint32_t>("spi-max-frequency").back();
		 emit_def("METAL_FLASH_SPI_MAX_FREQUENCY", std::to_string(freq));
	 }
	 if (n.field_exists("size")) {
		 uint32_t size = n.get_fields<uint32_t>("size").back();
		 emit_def("METAL_FLASH_SIZE", std::to_string(size));
	 }
	 if (n.field_exists("erase-block-size")) {
		 uint32_t erase_block_size = n.get_fields<uint32_t>("erase-block-size").back();
		 emit_def("METAL_FLASH_ERASE_BLOCK_SIZE", std::to_string(erase_block_size));
	 }
	 if (n.field_exists("write-block-size")) {
		 uint32_t write_block_size = n.get_fields<uint32_t>("write-block-size").back();
		 emit_def("METAL_FLASH_WRITE_BLOCK_SIZE", std::to_string(write_block_size));
	 }
	});
}

void sifive_flash::declare_structs() {
	dtb.match(std::regex(compat_string),
			[&](node n) { emit_struct_decl("sifive_flash", n); });
}


void sifive_flash::define_structs() {
	dtb.match(std::regex(compat_string), [&](node n) {
			emit_struct_begin("sifive_flash", n);
			emit_struct_field("flash.vtable",
					"&__metal_driver_vtable_sifive_flash.flash");
			emit_struct_end();
	});
}


void sifive_flash::create_handles() {
	emit_def("__METAL_DT_MAX_FLASH", std::to_string(num_flash));

	emit_struct_pointer_begin("sifive_flash", "__metal_flash_table", "[]");
	if (num_flash) {
		int i = 0;
		dtb.match(std::regex(compat_string), [&](node n) {
				os << "\t\t\t\t\t&__metal_dt_" << n.handle();
		
		if ((i + 1) == num_flash) {
			os << "};\n\n";
		} else {
		  os << ",\n";
		  }

		  i++;
		  });
	} else {
		emit_struct_pointer_end("NULL");
	}
}
