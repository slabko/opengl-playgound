#ifndef PLAYGROUND_PNG_HPP
#define PLAYGROUND_PNG_HPP

#include <string>
#include <vector>

#include <Eigen/Eigen>

namespace png {

template<class PixelType>
using Pixels = Eigen::Matrix<PixelType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

struct RedPixel {
    uint8_t r;
};

struct RgbPixel {
    uint8_t r, g, b;
};

struct RgbaPixel {
    uint8_t r, g, b, a;
};

// NOLINTBEGIN(readability-identifier-naming)
template<class PixelType>
struct total_channels {
    static const size_t value = 0;
};

template<>
struct total_channels<RedPixel> {
    static const size_t value = 1;
};

template<>
struct total_channels<RgbPixel> {
    static const size_t value = 3;
};

template<>
struct total_channels<RgbaPixel> {
    static const size_t value = 4;
};
// NOLINTEND(readability-identifier-naming)

template<class PixelType>
struct PngData {
    Pixels<PixelType> pixels{};
    size_t width{};
    size_t height{};
    size_t bit_depth{};
    size_t color_type{};
    size_t channels{};
};

template<class PixelType>
auto read_png(std::string const& filepath) -> PngData<PixelType>;

} // namespace png

#endif // PLAYGROUND_PNG_HPP
