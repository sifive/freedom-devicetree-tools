/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "phdr.h"

Phdr::Phdr()
  : name(""), flags("")
{
}

Phdr::Phdr(string name, string flags)
  : name(name), flags(flags)
{
}

