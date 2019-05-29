/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sstream>
#include <iomanip>

using std::stringstream;
using std::hex;

#include "memory.h"

Memory::Memory()
  : compatible(""), size(0)
{
}

Memory::Memory(string compatible)
  : compatible(compatible)
{
}

Memory& Memory::operator=(const Memory& other)
{
  if (this != &other) {
    compatible = other.compatible;
    base = other.base;
    size = other.size;

    name = other.name;
    attributes = other.attributes;
  }

  return *this;
}

bool Memory::operator==(const Memory& other) const
{
  return ((compatible.compare(other.compatible) == 0) &&
         (name.compare(other.name) == 0) &&
         (attributes.compare(other.attributes) == 0) &&
         (base == other.base) &&
         (size == other.size));
}

bool Memory::operator!=(const Memory& other) const
{
  return !(*this == other);
}

bool Memory::operator< (const Memory& rhs) const
{
  return (this->base < rhs.base);
}

string Memory::describe()
{
  stringstream stream;

  stream << hex << name << " (" << attributes << ") : ORIGIN = 0x" << base
         << ", LENGTH = 0x" << size;

  return stream.str();
}

