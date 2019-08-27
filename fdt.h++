/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef FREEDOM_DEVICETREE_TOOLS_H
#define FREEDOM_DEVICETREE_TOOLS_H

#include "libfdt.h++"
#include <iostream>
#include <regex>
#include <stdint.h>
#include <string>
#include <tuple>

class target_addr {
private:
  const int64_t _value;

public:
  target_addr() : _value(-1) {}

  target_addr(int64_t value) : _value(value) {}

public:
  std::string as_string(void) const { return std::to_string(_value); }

  operator uint64_t() const { return _value; }
};

class target_size {
private:
  const int64_t _value;

public:
  target_size() : _value(-1) {}

  target_size(int64_t value) : _value(value) {}

public:
  std::string as_string(void) const { return std::to_string(_value); }

  operator uint64_t() const { return _value; }
};

class node;

template <typename func_t> void apply(func_t f) { return f(); };

template <typename func_t, typename a_t>
void apply(func_t f, std::tuple<a_t> t) {
  return f(std::get<0>(t));
}

template <typename func_t, typename a_t>
void apply_index(func_t f, std::tuple<a_t> t, int idx) {
  return f(idx, std::get<0>(t));
}

template <typename func_t, typename a_t>
void apply_size(func_t f, std::tuple<a_t> t, int a_s) {
  return f(a_s, std::get<0>(t));
}

template <typename func_t, typename a_t, typename b_t>
void apply(func_t f, std::tuple<a_t, b_t> t) {
  return f(std::get<0>(t), std::get<1>(t));
}

template <typename func_t, typename a_t, typename b_t>
void apply_index(func_t f, std::tuple<a_t, b_t> t, int idx) {
  return f(idx, std::get<0>(t), std::get<1>(t));
}

template <typename func_t, typename a_t, typename b_t>
void apply_size(func_t f, std::tuple<a_t, b_t> t, int a_s) {
  return f(a_s, std::get<0>(t), std::get<1>(t));
}

template <typename func_t, typename a_t, typename b_t, typename c_t>
void apply(func_t f, std::tuple<a_t, b_t, c_t> t) {
  return f(std::get<0>(t), std::get<1>(t), std::get<2>(t));
}

template <typename func_t, typename a_t, typename b_t, typename c_t>
void apply_index(func_t f, std::tuple<a_t, b_t, c_t> t, int idx) {
  return f(idx, std::get<0>(t), std::get<1>(t), std::get<2>(t));
}

template <typename func_t, typename a_t, typename b_t, typename c_t>
void apply_size(func_t f, std::tuple<a_t, b_t, c_t> t, int a_s) {
  return f(a_s, std::get<0>(t), std::get<1>(t), std::get<2>(t));
}

template <typename... element_types> class tuple_t {
public:
  using tuple = std::tuple<element_types...>;
};

namespace std {
static inline std::string to_string(const target_size &tl) {
  return tl.as_string();
}
static inline std::string to_string(const target_addr &tl) {
  return tl.as_string();
}
} // namespace std

/* Represents a node within a device tree, which may point to other nodes. */
class node {
private:
  const uint8_t *_dts_blob;
  int _offset;
  int _depth;

public:
  node(const uint8_t *dts_blob, int offset, int depth)
      : _dts_blob(dts_blob), _offset(offset), _depth(depth) {}

  std::string name(void) const;

  std::string handle(void) const;

  std::string handle_cap(void) const;

  std::string instance(void) const;

  node parent(void) const;

  bool field_exists(std::string) const;

  int num_addr_cells(void) const;
  int num_size_cells(void) const;

  void obtain_one(std::vector<node> &v, const uint8_t *buf, int len, int offset,
                  int *consumed) const;
  void obtain_one(std::vector<uint32_t> &v, const uint8_t *buf, int len,
                  int offset, int *consumed) const;
  void obtain_one(std::vector<target_addr> &v, const uint8_t *buf, int len,
                  int offset, int *consumed) const;
  void obtain_one(std::vector<target_size> &v, const uint8_t *buf, int len,
                  int offset, int *consumed) const;
  void obtain_one(std::vector<std::string> &v, const uint8_t *buf, int len,
                  int offset, int *consumed) const;

  template <typename a_t>
  void obtain_one(std::vector<std::tuple<a_t>> &v, const uint8_t *buf, int len,
                  int offset, int *consumed) const {
    int one_consumed;
    *consumed = 0;

    std::vector<a_t> a_v;
    obtain_one(a_v, buf, len, offset, &one_consumed);
    offset += one_consumed;
    *consumed += one_consumed;

    v.push_back(std::make_tuple(a_v[0]));
  }

  template <typename a_t, typename b_t>
  void obtain_one(std::vector<std::tuple<a_t, b_t>> &v, const uint8_t *buf,
                  int len, int offset, int *consumed) const {
    int one_consumed;
    *consumed = 0;

    std::vector<a_t> a_v;
    obtain_one(a_v, buf, len, offset, &one_consumed);
    offset += one_consumed;
    *consumed += one_consumed;

    std::vector<b_t> b_v;
    obtain_one(b_v, buf, len, offset, &one_consumed);
    offset += one_consumed;
    *consumed += one_consumed;

    v.push_back(std::make_tuple(a_v[0], b_v[0]));
  }

  template <typename a_t, typename b_t, typename c_t>
  void obtain_one(std::vector<std::tuple<a_t, b_t, c_t>> &v, const uint8_t *buf,
                  int len, int offset, int *consumed) const {
    int one_consumed;
    *consumed = 0;

    std::vector<a_t> a_v;
    obtain_one(a_v, buf, len, offset, &one_consumed);
    offset += one_consumed;
    *consumed += one_consumed;

    std::vector<b_t> b_v;
    obtain_one(b_v, buf, len, offset, &one_consumed);
    offset += one_consumed;
    *consumed += one_consumed;

    std::vector<c_t> c_v;
    obtain_one(c_v, buf, len, offset, &one_consumed);
    offset += one_consumed;
    *consumed += one_consumed;

    v.push_back(std::make_tuple(a_v[0], b_v[0], c_v[0]));
  }

  template <typename t> t get_field(std::string name) const {
    auto out = get_fields<t>(name);
    if (out.size() == 0) {
      std::cerr << "requested field \"" << name << "\" in node \""
                << this->name() << ", but none found\n";
      abort();
    } else if (out.size() > 1) {
      std::cerr << "requested field \"" << name << "\" in node \""
                << this->name() << ", but multiple found\n";
      abort();
    }
    return out[0];
  }

  template <typename t> std::vector<t> get_fields(std::string name) const {
    int len;
    auto buf =
        (const uint8_t *)fdt_getprop(_dts_blob, _offset, name.c_str(), &len);

    if (buf == nullptr)
      return std::vector<t>();

    int i = 0;
    auto out = std::vector<t>();
    while (i < len) {
      int consumed = -1;
      obtain_one(out, buf, len, i, &consumed);
      if (consumed == -1)
        break;
      i += consumed;
    }
    return out;
  }

  template <typename t> int get_fields_count(std::string name) const {
    int len;
    auto buf =
        (const uint8_t *)fdt_getprop(_dts_blob, _offset, name.c_str(), &len);

    if (buf == nullptr)
      return 0;

    int i = 0;
    int count = 0;
    auto out = std::vector<t>();
    while (i < len) {
      int consumed = -1;
      obtain_one(out, buf, len, i, &consumed);
      if (consumed == -1)
        break;
      i += consumed;
      count++;
    }
    return count;
  }

  template <typename value_t, typename function_t>
  int named(std::string name, value_t value, function_t function) const {
    auto vals = get_fields<typename value_t::tuple>(name);

    apply(function, vals[0]);

    return 1;
  }

  template <typename value_t, typename function_t, typename... args_v>
  int named(std::string name, value_t value, function_t function,
            args_v... args) const {
    return named(name, std::forward<args_v>(args)...) +
           named(name, value, function);
  }

  template <typename value_t, typename function_t>
  int named_tuples(std::string key_name, std::string value_name,
                   std::string key, value_t value, function_t function) const {
    auto keys = get_fields<std::string>(key_name);
    auto vals = get_fields<typename value_t::tuple>(value_name);

    int out = 0;
    for (size_t i = 0; i < keys.size(); ++i) {
      if (keys[i].compare(key) == 0) {
        out++;
        apply(function, vals[i]);
      }
    }

    return out;
  }

  template <typename value_t, typename function_t, typename... args_v>
  int named_tuples(std::string key_name, std::string value_name,
                   std::string key, value_t value, function_t function,
                   args_v... args) const {
    return named_tuples(key_name, value_name, std::forward<args_v>(args)...) +
           named_tuples(key_name, value_name, key, value, function);
  }

  template <typename zero_function_t, typename one_function_t,
            typename... args_v>
  void maybe_tuple(std::string key, tuple_t<args_v...> value,
                   zero_function_t zero_func, one_function_t one_func) const {
    auto out = get_fields<std::tuple<args_v...>>(key);
    int s = out.size();
    ;
    for (int i = 0; s >= 0; s--, i++) {
      if (s == 0)
        return apply(zero_func);
      if (s == 1)
        return apply(one_func, out[i]);
      apply(one_func, out[i]);
    }
    std::cerr << "requested field \"" << key << "\" in node \"" << name()
              << "\", found " << out.size() << " fields\n";
    abort();
  }

  template <typename zero_function_t, typename one_function_t,
            typename... args_v>
  void maybe_tuple_index(std::string key, tuple_t<args_v...> value,
                         zero_function_t zero_func,
                         one_function_t one_func) const {
    auto out = get_fields<std::tuple<args_v...>>(key);
    int s = out.size();
    ;
    for (int i = 0; s >= 0; s--, i++) {
      if (s == 0)
        return apply(zero_func);
      if (s == 1)
        return apply_index(one_func, out[i], i);
      apply_index(one_func, out[i], i);
    }
    std::cerr << "requested field \"" << key << "\" in node \"" << name()
              << "\", found " << out.size() << " fields\n";
    abort();
  }

  template <typename zero_function_t, typename one_function_t,
            typename... args_v>
  void maybe_tuple_size(std::string key, tuple_t<args_v...> value,
                        zero_function_t zero_func,
                        one_function_t one_func) const {
    auto out = get_fields<std::tuple<args_v...>>(key);
    int s = out.size();
    ;
    for (int i = 0; s >= 0; s--, i++) {
      if (s == 0)
        return apply(zero_func);
      if (s == 1)
        return apply_size(one_func, out[i], out.size());
      apply_size(one_func, out[i], out.size());
    }
    std::cerr << "requested field \"" << key << "\" in node \"" << name()
              << "\", found " << out.size() << " fields\n";
    abort();
  }
};

/* Represents a single FDT, which is read in as a binary from a file. */
class fdt {
private:
  const uint8_t *_dts_blob;
  const bool _allocated;

public:
  fdt(const uint8_t *blob);
  fdt(const std::string path);

  ~fdt(void) {
    if (_allocated)
      delete _dts_blob;
  }

  bool path_exists(std::string) const;
  node node_by_path(std::string) const;

  template <typename... args>
  int match(const std::regex &r, std::function<void(const node &)> f,
            args... a) const {
    return match(r, f) + match(std::forward<args>(a)...);
  }

  int match(const std::regex &r, std::function<void(const node &)> f) const;

  template <typename... tuple_args, typename... args, typename func>
  int chosen(std::string r, tuple_t<tuple_args...> types, func f,
             args... a) const {
    return chosen(r, types, f) + chosen(std::forward<args>(a)...);
  }

  template <typename... tuple_args, typename... args, typename func>
  int chosen(std::string r, tuple_t<tuple_args...> types, func f) const {
    if (path_exists("/chosen") == false)
      return 0;

    auto n = node_by_path("/chosen");
    if (n.field_exists(r) == false)
      return 0;

    auto t = n.get_field<std::tuple<tuple_args...>>(r);
    apply(f, t);
    return 1;
  }
};

char *dts_read(const char *filename);

#endif
