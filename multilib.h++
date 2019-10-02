/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2018 SiFive, Inc */

#ifndef FDT__MULTILIB_HXX
#define FDT__MULTILIB_HXX

std::string arch2arch(std::string arch) {
  if (arch == "rv32ea")
    return "rv32e";
  if (arch == "rv32ema")
    return "rv32em";
  if (arch == "rv32emc")
    return "rv32em";

  if (arch == "rv32ia")
    return "rv32i";
  if (arch == "rv32ima")
    return "rv32im";

  if (arch == "rv64ia")
    return "rv64i";
  if (arch == "rv64ima")
    return "rv64im";

  return arch;
}

std::string arch2abi(std::string arch) {
  if (arch.find("rv32e") != std::string::npos) {
    if (arch.find("d") != std::string::npos) {
      return "ilp32ed";
    } else if (arch.find("f") != std::string::npos) {
      return "ilp32ef";
    } else {
      return "ilp32e";
    }
  } else if (arch.find("rv32i") != std::string::npos) {
    if (arch.find("d") != std::string::npos) {
      return "ilp32d";
    } else if (arch.find("f") != std::string::npos) {
      return "ilp32f";
    } else {
      return "ilp32";
    }
  } else if (arch.find("rv64i") != std::string::npos) {
    if (arch.find("d") != std::string::npos) {
      return "lp64d";
    } else if (arch.find("f") != std::string::npos) {
      return "lp64f";
    } else {
      return "lp64";
    }
  }

  std::cerr << "arch2abi(): unknown arch " << arch << std::endl;
  abort();
  return "";
}

std::string arch2elf(std::string arch) {
  if (arch.substr(0, 4) == "rv32")
    return "elf32lriscv";
  if (arch.substr(0, 4) == "rv64")
    return "elf64lriscv";

  std::cerr << "arch2elf(): unknown arch " << arch << std::endl;
  std::cerr << "    arch.substr(0, 4): " << arch.substr(0, 4) << std::endl;
  abort();
  return "";
}

#endif
