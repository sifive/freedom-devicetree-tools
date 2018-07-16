#include "fdt.h++"
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <string>

static uint8_t *read_fdt_from_path(const char *filename)
{
    uint8_t *buf = NULL;
    int fd = 0; /* assume stdin */
    size_t bufsize = 1024, offset =0;;
    int ret = 0;

    if (strcmp(filename, "-") != 0) {
        fd = open(filename, O_RDONLY);
        if (fd < 0)
            return buf;
    }

    /* Loop until we have read everything */
    buf = (uint8_t *)malloc(bufsize);
    do {
        /* Expand the buffer to hold the next chunk */
        if (offset == bufsize) {
            bufsize *= 2;
            buf = (uint8_t *)realloc(buf, bufsize);
        }

        ret = read(fd, &buf[offset], bufsize - offset);
        if (ret < 0) {
            break;
        }
        offset += ret;
    } while (ret != 0);

    /* Clean up, including closing stdin; return errno on error */
    close(fd);
    if (ret) {
        free(buf);
        return NULL;
    }
    return buf;
}

void node::obtain_one(std::vector<node> &v, const uint8_t *buf, int len, int offset, int *consumed) const {
    std::vector<target_long> phandles;
    obtain_one(phandles, buf, len, offset, consumed);
    auto phandle_offset = fdt_node_offset_by_phandle(_dts_blob, phandles[0]);
    v.push_back(node(_dts_blob, phandle_offset, -1));
}

void node::obtain_one(std::vector<target_long> &v, const uint8_t *buf, int len, int offset, int *consumed) const {
    uint32_t n = *((uint32_t *)(buf + offset));
    v.push_back(ntohl(n));
    *consumed = 4;
}

void node::obtain_one(std::vector<std::string> &v, const uint8_t *buf, int len, int offset, int *consumed) const {
    int i = offset;
    while (i < len && buf[i] != '\0') {
        ++i;
    }
    v.push_back(std::string((const char *)(buf + offset)));
    *consumed = i+1;
}

std::string node::name(void) const
{
    auto node_name = fdt_get_name(_dts_blob, _offset, NULL);
    if (node_name == nullptr)
        return std::string();
    return node_name;
}

std::string node::handle(void) const
{
    auto n = name();
    std::transform(n.begin(), n.end(), n.begin(),
                   [](unsigned char c) { return (c == '@') ? '_' : c; });
    return n;
}

bool node::field_exists(std::string name) const
{
    int len;
    auto out = fdt_get_property(_dts_blob, _offset, name.c_str(), &len);
    return out != nullptr;
}

fdt::fdt(const char *path)
: _dts_blob(read_fdt_from_path(path))
{}

fdt::fdt(std::string path)
: _dts_blob(read_fdt_from_path(path.c_str()))
{}

bool fdt::path_exists(std::string path) const
{
    auto offset = fdt_path_offset(_dts_blob, path.c_str());
    return offset > 0;
}

node fdt::node_by_path(std::string path) const
{
    auto offset = fdt_path_offset(_dts_blob, path.c_str());
    return node(_dts_blob, offset, 0);
}

int fdt::match(const std::regex& r, std::function<void(const node&)> f) const
{
    int depth, offset;
    int matches = 0;

    depth = 0;
    offset = fdt_path_offset(_dts_blob, "/");
    while (offset >= 0 && depth >= 0) {
        int compat_len;
        auto compat_bytes = (const char *)fdt_getprop(_dts_blob, offset, "compatible", &compat_len);
        if (compat_bytes != nullptr) {
            for (int i = 0; i < compat_len; i += strlen(compat_bytes)) {
                auto compat = std::string(&compat_bytes[i]);
                if (compat.length() == 0 || !std::regex_match(compat, r))
                    continue;
                f(node(_dts_blob, offset, depth));
                matches++;
            }
        }

        offset = fdt_next_node(_dts_blob, offset, &depth);
    }

    return matches;
}
