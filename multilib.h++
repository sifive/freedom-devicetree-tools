/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2018 SiFive, Inc */

#ifndef FDT__MULTILIB_HXX
#define FDT__MULTILIB_HXX

std::string arch2arch(std::string arch)
{
    if (arch == "rv32i")      return "rv32i";
    if (arch == "rv32im")     return "rv32im";
    if (arch == "rv32iac")    return "rv32iac";
    if (arch == "rv32imac")   return "rv32imac";
    if (arch == "rv32imafc")  return "rv32imafc";
    if (arch == "rv64imac")   return "rv64imac";
    if (arch == "rv64imafdc") return "rv64imafdc";

    if (arch == "rv32imafdc") return "rv32imafdc";

    std::cerr << "arch2arch(): unknown arch " << arch << std::endl;
    abort();
    return "";
}

std::string arch2abi(std::string arch)
{
    if (arch == "rv32i")      return "ilp32";
    if (arch == "rv32im")     return "ilp32";
    if (arch == "rv32iac")    return "ilp32";
    if (arch == "rv32imac")   return "ilp32";
    if (arch == "rv32imafc")  return "ilp32f";
    if (arch == "rv64imac")   return "lp64";
    if (arch == "rv64imafdc") return "lp64d";

    if (arch == "rv32imafdc") return "ilp32f";

    std::cerr << "arch2abi(): unknown arch " << arch << std::endl;
    abort();
    return "";
}

std::string arch2elf(std::string arch)
{
    if (arch.substr(0, 4) == "rv32") return "elf32lriscv";
    if (arch.substr(0, 4) == "rv64") return "elf64lriscv";

    std::cerr << "arch2elf(): unknown arch " << arch << std::endl;
    std::cerr << "    arch.substr(0, 4): " << arch.substr(0, 4) << std::endl;
    abort();
    return "";
}

#endif
