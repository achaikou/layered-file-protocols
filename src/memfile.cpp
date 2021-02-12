#include <algorithm>
#include <cstdint>
#include <cassert>
#include <vector>

#include <fmt/format.h>

#include <lfp/protocol.hpp>
#include <lfp/memfile.h>

namespace lfp { namespace {

/*
 * A fixed-size file in memory - is right now a vector, but could just as well
 * be a memory mapped file.
 *
 * It is largely intended for testing, but it can surely be used for other
 * things too.
 *
 * TODO: proper mmap implementation
 */
class memfile : public lfp_protocol {
public:
    memfile() = default;
    memfile(const unsigned char* p, std::size_t len) : mem(p, p + len) {}

    void close() noexcept (true) override;
    lfp_status readinto(
            void* dst,
            std::int64_t len,
            std::int64_t* bytes_read)
        noexcept (true) override;

    int eof() const noexcept (true) override;

    void seek(std::int64_t) noexcept (false) override;
    std::int64_t tell() const noexcept (true) override;
    std::int64_t ptell() const noexcept (true) override;

    lfp_protocol* peel() noexcept (false) override;
    lfp_protocol* peek() const noexcept (false) override;

private:
    std::vector< unsigned char > mem;
    std::int64_t pos = 0;
};

void memfile::close() noexcept (true) {}

lfp_status memfile::readinto(void* p, std::int64_t len, std::int64_t* nread)
noexcept (true) {
    const auto remaining = std::int64_t(this->mem.size() - this->pos);
    const auto n = (std::min)(len, remaining);
    assert(n >= 0);
    assert(this->pos >= 0);
    assert(std::size_t(this->pos + n) <= this->mem.size());
    std::memcpy(p, this->mem.data() + this->pos, n);
    this->pos += n;

    if (nread)
        *nread = n;

    if (n == len)
        return LFP_OK;

    if (this->eof())
        return LFP_EOF;
    else
        return LFP_OKINCOMPLETE;
}

int memfile::eof() const noexcept (true) {
    return std::size_t(this->pos) == this->mem.size();
}

void memfile::seek(std::int64_t n) noexcept (false) {
    assert(n >= 0);
    if (std::size_t(n) >= this->mem.size()) {
        const auto msg = "memfile: seek: offset (= {}) >= file size (= {})";
        throw invalid_args(fmt::format(msg, n, this->mem.size()));
    }

    this->pos = n;
}

std::int64_t memfile::tell() const noexcept (true) {
    return this->pos;
}

std::int64_t memfile::ptell() const noexcept (true) {
    /* It's impossible to open memfile on anything else other than 0,
     * so ptell will always match logical tell.
     */
    return this->tell();
}

lfp_protocol* memfile::peel() noexcept (false) {
    throw lfp::leaf_protocol("peel: not supported for leaf protocol");
}

lfp_protocol* memfile::peek() const noexcept (false) {
    throw lfp::leaf_protocol("peek: not supported for leaf protocol");
}

}

}

lfp_protocol* lfp_memfile_open() {
    try {
        return new lfp::memfile();
    } catch (...) {
        return nullptr;
    }
}

lfp_protocol* lfp_memfile_openwith(const unsigned char* p, std::size_t len) {
    try {
        return new lfp::memfile(p, len);
    } catch (...) {
        return nullptr;
    }
}
