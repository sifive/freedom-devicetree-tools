/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2018 SiFive, Inc */

#ifndef FDT__MULTILIB_HXX
#define FDT__MULTILIB_HXX

std::string arch2arch(std::string arch)
{
    if (arch == "rv32e")      return "rv32e";
    if (arch == "rv32ea")     return "rv32e";
    if (arch == "rv32ec")     return "rv32ec";
    if (arch == "rv32em")     return "rv32em";
    if (arch == "rv32ema")    return "rv32em";
    if (arch == "rv32emc")    return "rv32em";
    if (arch == "rv32eac")    return "rv32eac";
    if (arch == "rv32emac")   return "rv32emac";
    if (arch == "rv32emafc")  return "rv32emafc";
    if (arch == "rv32emafdc") return "rv32emafdc";
    if (arch == "rv32i")      return "rv32i";
    if (arch == "rv32ia")     return "rv32i";
    if (arch == "rv32ic")     return "rv32ic";
    if (arch == "rv32im")     return "rv32im";
    if (arch == "rv32ima")    return "rv32im";
    if (arch == "rv32imc")    return "rv32imc";
    if (arch == "rv32imf")    return "rv32imf";
    if (arch == "rv32imaf")   return "rv32imaf";
    if (arch == "rv32iac")    return "rv32iac";
    if (arch == "rv32imac")   return "rv32imac";
    if (arch == "rv32imafc")  return "rv32imafc";
    if (arch == "rv32imafdc") return "rv32imafdc";
    if (arch == "rv64i")      return "rv64i";
    if (arch == "rv64ia")     return "rv64i";
    if (arch == "rv64ic")     return "rv64ic";
    if (arch == "rv64im")     return "rv64im";
    if (arch == "rv64ima")    return "rv64im";
    if (arch == "rv64imc")    return "rv64imc";
    if (arch == "rv64imf")    return "rv64imf";
    if (arch == "rv64imaf")   return "rv64imaf";
    if (arch == "rv64iac")    return "rv64iac";
    if (arch == "rv64imac")   return "rv64imac";
    if (arch == "rv64imafc")  return "rv64imafc";
    if (arch == "rv64imafdc") return "rv64imafdc";

    std::cerr << "arch2arch(): unknown arch " << arch << std::endl;
    abort();
    return "";
}

std::string arch2abi(std::string arch)
{
    if (arch == "rv32e")      return "ilp32e";
    if (arch == "rv32ea")     return "ilp32e";
    if (arch == "rv32ec")     return "ilp32e";
    if (arch == "rv32em")     return "ilp32e";
    if (arch == "rv32ema")    return "ilp32e";
    if (arch == "rv32emc")    return "ilp32e";
    if (arch == "rv32eac")    return "ilp32e";
    if (arch == "rv32emac")   return "ilp32e";
    if (arch == "rv32emafc")  return "ilp32f";
    if (arch == "rv32emafdc") return "ilp32d";
    if (arch == "rv32i")      return "ilp32";
    if (arch == "rv32ia")     return "ilp32";
    if (arch == "rv32ic")     return "ilp32";
    if (arch == "rv32im")     return "ilp32";
    if (arch == "rv32ima")    return "ilp32";
    if (arch == "rv32imc")    return "ilp32";
    if (arch == "rv32imf")    return "ilp32f";
    if (arch == "rv32imaf")   return "ilp32f";
    if (arch == "rv32iac")    return "ilp32";
    if (arch == "rv32imac")   return "ilp32";
    if (arch == "rv32imafc")  return "ilp32f";
    if (arch == "rv32imafdc") return "ilp32d";
    if (arch == "rv64i")      return "lp64";
    if (arch == "rv64ia")     return "lp64";
    if (arch == "rv64ic")     return "lp64";
    if (arch == "rv64im")     return "lp64";
    if (arch == "rv64ima")    return "lp64";
    if (arch == "rv64imc")    return "lp64";
    if (arch == "rv64imf")    return "lp64f";
    if (arch == "rv64imaf")   return "lp64f";
    if (arch == "rv64iac")    return "lp64";
    if (arch == "rv64imac")   return "lp64";
    if (arch == "rv64imafc")  return "lp64f";
    if (arch == "rv64imafdc") return "lp64d";

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
