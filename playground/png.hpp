#ifndef PLAYGROUND_PNG_HPP
#define PLAYGROUND_PNG_HPP

#include <string>
#include <vector>

namespace png {

struct PngData {
    std::vector<uint8_t> pixels{};
    size_t width{};
    size_t height{};
    size_t bit_depth{};
    size_t color_type{};
    size_t channels{};
};

auto read_png(std::string const& filepath) -> PngData;

} // namespace png

#endif // PLAYGROUND_PNG_HPP
