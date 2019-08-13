/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_INLINE__H
#define __METAL_HEADER_INLINE__H

#include <list>
#include <string>

class Inline {
public:
  std::string name;
  std::string return_type;
  std::list<std::string> args;
  std::list<std::string> body_cases;
  std::list<std::string> body_returns;

  enum Stage { Start, Middle, End, Empty };

  Inline(std::string name, std::string return_type)
      : name(name), return_type(return_type) {}
};

#endif
