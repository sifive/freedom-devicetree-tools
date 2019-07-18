
#include <iostream>

#include <fdt.h++>

#include <memory.h>
#include <strategies/default_rocket_strategy.h>

using std::cout;
using std::dec;
using std::endl;
using std::hex;

DefaultRocketStrategy strategy;
fdt dtb("linker_script/test/blank_rv64.dtb");

void print_test(Memory ram, Memory rom, Memory itim) {
  cout << hex;
  cout << "ram:  " << ram.base << "-" << ((int64_t)ram.base + ram.size - 1) << endl;
  cout << "rom:  " << rom.base << "-" << ((int64_t)rom.base + rom.size - 1) << endl;
  cout << "itim: " << itim.base << "-" << ((int64_t)itim.base + itim.size - 1) << endl;
  cout << dec;
}

int run_test(uint64_t ram_base, uint64_t ram_size,
             uint64_t rom_base, uint64_t rom_size,
             uint64_t itim_base, uint64_t itim_size)
{
  Memory ram("sifive,dtim");
  ram.base = ram_base;
  ram.size = ram_size;

  Memory rom("sifive,spi0");
  rom.base = rom_base;
  rom.size = rom_size;

  Memory itim("sifive,itim");
  itim.base = itim_base;
  itim.size = itim_size;

  cout << "--------------------------------------" << endl;
  print_test(ram, rom, itim);

  int supported = 0;

  if (strategy.check_cmodel_support(dtb, CODE_MODEL_MEDLOW, ram, rom, itim)) {
    supported += 1;
    cout << "Supports medlow" << endl;
  }
  if (strategy.check_cmodel_support(dtb, CODE_MODEL_MEDANY, ram, rom, itim)) {
    supported += 1;
    cout << "Supports medany" << endl;
  }

  return supported;
}

int main()
{
  int failed = 0;
  int supported = 0;

  /* 
   * All memories are indexable from address 0
   */
  if (run_test(0x40000000, 0x20000000, 0x0, 0x20000000, 0x20000000, 0x20000000) != 2) {
    failed += 1;
  }

  /*
   * ROM and ITIM are indexable from address 0
   * RAM is not indexable from address 0, but is indexable from the PC
   */
  if (run_test(0x80000000, 0x40000000, 0x40000000, 0x20000000, 0x60000000, 0x20000000) != 1) {
    failed += 1;
  }

  /*
   * ROM and ITIM are indexable from address 0
   * RAM is not indexable from address 0, and most is indexable from the PC,
   *  but not all of it.
   */
  if (run_test(0x80000000, 0x80000000, 0x40000000, 0x20000000, 0x60000000, 0x20000000) != 1) {
    failed += 1;
  }

  /*
   * ROM and ITIM are indexable from address 0
   * RAM is not indexable from either address 0 or the PC
   */
  if (run_test(0x180000000, 0x4000, 0x20000000, 0x20000000, 0x40000000, 0x20000000) != 0) {
    failed += 1;
  }

  /*
   * RAM and ROM are indexable from address 0
   * ITIM is indexable from the PC
   */
  if (run_test(0x0, 0x40000000, 0x40000000, 0x40000000, 0x80000000, 0x40000000) != 1) {
    failed += 1;
  }

  /*
   * RAM and ROM are indexable from address 0
   * ITIM is not indexable from the PC
   */
  if (run_test(0x40000000, 0x40000000, 0x0, 0x40000000, 0x80000000, 0x40000000) != 0) {
    failed += 1;
  }
  if (run_test(0x0, 0x40000000, 0x40000000, 0x40000000, 0xC0000000, 0x40000000) != 0) {
    failed += 1;
  }

  /*
   * RAM and ITIM are indexable from address 0
   * ROM is indexable from the PC
   */
  if (run_test(0x0, 0x40000000, 0x80000000, 0x40000000, 0x40000000, 0x40000000) != 1) {
    failed += 1;
  }

  /*
   * RAM and ITIM are indexable from address 0
   * ROM is not indexable from the PC
   */
  if (run_test(0x40000000, 0x40000000, 0x80000000, 0x40000000, 0x0, 0x40000000) != 0) {
    failed += 1;
  }
  if (run_test(0x0, 0x40000000, 0xC0000000, 0x40000000, 0x40000000, 0x40000000) != 0) {
    failed += 1;
  }

  /* 
   * ROM and ITIM are within a 2 GiB region not indexable from address 0
   * RAM is indexable from address 0, but not from the PC
   */
  if (run_test((int64_t)INT32_MIN, 0x100000000, 0x80000000, 0x40000000, 0xC0000000, 0x40000000) != 1) {
    failed += 1;
  }

  /*
   * ROM and ITIM are not within a 2 GiB region not indexable from address 0
   * RAM is indexable from address 0, but not from the PC
   */
  if (run_test((int64_t)INT32_MIN, 0x100000000, 0x80000000, 0x40000000, 0x180000000, 0x40000000) != 0) {
    failed += 1;
  }

  /* 
   * ROM and ITIM are within a 2 GiB region not indexable from address 0
   * RAM is mostly indexable from address 0, but extends past the +/- 2GiB boundary on both sides
   */
  if (run_test(((int64_t)INT32_MIN - 0x10000000), 0x120000000, 0xA0000000, 0x40000000, 0xF0000000, 0x40000000) != 0) {
    failed += 1;
  }

  cout << "--------------------------------------" << endl;

  if (failed == 0) {
    cout << "PASSED" << endl;
  } else {
    cout << "FAILED " << failed << endl;
  }

  return failed;
}
