#include "fdt.h++"
#ifdef __MINGW32__
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

char *dts_read(const char *filename) {
  if (strcmp(filename, "-") != 0) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();
    int length = pos;
    char *buf = (char *)malloc(length);
    ifs.seekg(0, std::ios::beg);
    ifs.read(buf, length);
    if (!ifs) {
      free(buf);
      buf = NULL;
    }
    ifs.close();
    return buf;
  } else {
    /* stdin/cin is not supported yet */
    return NULL;
  }
}

static uint8_t *read_fdt_from_path(const char *filename) {
  return (uint8_t *)dts_read(filename);
}

node node::parent(void) const {
  auto parent_offset = fdt_parent_offset(_dts_blob, _offset);
  return node(_dts_blob, parent_offset, _depth - 1);
}

int node::num_addr_cells(void) const {
  auto f = parent().get_fields<uint32_t>("#address-cells");
  if (f.size() == 0)
    return 2;
  if (f.size() == 1)
    return f[0];

  std::cerr << "#address-cells has more than one cell\n";
  abort();
}

int node::num_size_cells(void) const {
  auto f = parent().get_fields<uint32_t>("#size-cells");
  if (f.size() == 0)
    return 2;
  if (f.size() == 1)
    return f[0];

  std::cerr << "#size-cells has more than one cell\n";
  abort();
}

void node::obtain_one(std::vector<node> &v, const uint8_t *buf, int len,
                      int offset, int *consumed) const {
  std::vector<uint32_t> phandles;
  obtain_one(phandles, buf, len, offset, consumed);
  auto phandle_offset = fdt_node_offset_by_phandle(_dts_blob, phandles[0]);
  v.push_back(node(_dts_blob, phandle_offset, -1));
}

void node::obtain_one(std::vector<uint32_t> &v, const uint8_t *buf, int len,
                      int offset, int *consumed) const {
  uint32_t n = *((uint32_t *)(buf + offset));
  v.push_back(ntohl(n));
  *consumed = 4;
}

void node::obtain_one(std::vector<target_addr> &v, const uint8_t *buf, int len,
                      int offset, int *consumed) const {
  auto n = ((uint32_t *)(buf + offset));
  switch (num_addr_cells()) {
  case 1:
    v.push_back(ntohl(n[0]));
    *consumed = 4;
    return;
  case 2:
    v.push_back(ntohl(n[1]) | ((uint64_t)(ntohl(n[0])) << 32));
    *consumed = 8;
    return;
  }

  std::cerr << "Unknown number of address cells\n";
  abort();
}

void node::obtain_one(std::vector<target_size> &v, const uint8_t *buf, int len,
                      int offset, int *consumed) const {
  auto n = ((uint32_t *)(buf + offset));
  switch (num_size_cells()) {
  case 1:
    v.push_back(ntohl(n[0]));
    *consumed = 4;
    return;
  case 2:
    v.push_back(ntohl(n[1]) | ((uint64_t)(ntohl(n[0])) << 32));
    *consumed = 8;
    return;
  }

  std::cerr << "Unknown number of address cells\n";
  abort();
}

void node::obtain_one(std::vector<std::string> &v, const uint8_t *buf, int len,
                      int offset, int *consumed) const {
  int i = offset;
  while (i < len && buf[i] != '\0') {
    ++i;
  }
  v.push_back(std::string((const char *)(buf + offset)));
  *consumed = i + 1;
}

std::string node::name(void) const {
  auto node_name = fdt_get_name(_dts_blob, _offset, NULL);
  if (node_name == nullptr)
    return std::string();
  std::string n(node_name);
  std::transform(n.begin(), n.end(), n.begin(),
                 [](unsigned char c) { return (c == '-') ? '_' : c; });
  return n;
}

std::string node::handle(void) const {
  auto n = name();
  std::transform(n.begin(), n.end(), n.begin(),
                 [](unsigned char c) { return (c == '@') ? '_' : c; });
  return n;
}

std::string node::handle_cap(void) const {
  auto n = name();
  std::transform(n.begin(), n.end(), n.begin(),
                 [](unsigned char c) { return (c == '@') ? '_' : toupper(c); });
  return n;
}

std::string node::instance(void) const {
  auto n = name();
  std::size_t p = n.find('@');
  return (p != std::string::npos) ? n.substr(p + 1) : "";
}

bool node::field_exists(std::string name) const {
  int len;
  auto out = fdt_get_property(_dts_blob, _offset, name.c_str(), &len);
  return out != nullptr;
}

fdt::fdt(const uint8_t *blob) : _dts_blob(blob), _allocated(false) {
  assert(blob != nullptr);
  assert(_dts_blob != nullptr);
}

fdt::fdt(std::string path)
    : _dts_blob(read_fdt_from_path(path.c_str())), _allocated(true) {
  assert(_dts_blob != nullptr);
}

bool fdt::path_exists(std::string path) const {
  auto offset = fdt_path_offset(_dts_blob, path.c_str());
  return offset > 0;
}

node fdt::node_by_path(std::string path) const {
  auto offset = fdt_path_offset(_dts_blob, path.c_str());
  return node(_dts_blob, offset, 0);
}

int fdt::match(const std::regex &r, std::function<void(const node &)> f) const {
  int depth, offset;
  int matches = 0;

  depth = 0;
  offset = fdt_path_offset(_dts_blob, "/");
  while (offset >= 0 && depth >= 0) {
    int compat_len;
    auto device_bytes = (const char *)fdt_getprop(_dts_blob, offset,
                                                  "device_type", &compat_len);
    if (device_bytes != nullptr) {
      for (int i = 0; i < compat_len; i += strlen(&device_bytes[i]) + 1) {
        auto compat = std::string(&device_bytes[i]);
        if (i >= compat_len)
          break;
        if (!std::regex_match(compat, r))
          continue;
        f(node(_dts_blob, offset, depth));
        matches++;
      }
    } else {
      auto compat_bytes = (const char *)fdt_getprop(_dts_blob, offset,
                                                    "compatible", &compat_len);
      if (compat_bytes != nullptr) {
        for (int i = 0; i < compat_len; i += strlen(&compat_bytes[i]) + 1) {
          auto compat = std::string(&compat_bytes[i]);
          if (i >= compat_len)
            break;
          if (!std::regex_match(compat, r))
            continue;
          f(node(_dts_blob, offset, depth));
          matches++;
        }
      }
    }

    offset = fdt_next_node(_dts_blob, offset, &depth);
  }

  return matches;
}
