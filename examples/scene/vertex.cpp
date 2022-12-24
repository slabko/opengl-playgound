#include <fstream>
#include <stdexcept>
#include <fmt/core.h>

#include "vertex.hpp"

VertexModel read_vertex_model(std::string const& file_path)
{
    std::vector<glm::vec3> raw_vertices{};
    std::vector<glm::ivec3> raw_indices{};

    std::ifstream content_file(file_path);
    if (!content_file) {
        throw std::runtime_error("could not open the file");
    }

    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::min();

    char head{};
    while (content_file >> head) {
        switch (head) {
        case '#':
            content_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        case 'v':
            float x, y, z; // NOLINT(cppcoreguidelines-init-variables)
            content_file >> x >> y >> z;
            raw_vertices.emplace_back(x, y, z);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
            content_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        case 'f':
            int a, b, c; // NOLINT(cppcoreguidelines-init-variables)
            content_file >> a >> b >> c;
            raw_indices.emplace_back(a, b, c);
            content_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        default:
            throw std::runtime_error("unexpected entry");
        }

        if (!content_file.good()) {
            throw std::runtime_error("could not parse data");
        }
    }

    content_file.close();

    max_y -= min_y;

    auto res = std::vector<Vertex>{};
    res.reserve(raw_indices.size() * 3);
    for (auto idx : raw_indices) {
        auto a = raw_vertices[static_cast<size_t>(idx[0]) - 1];
        auto b = raw_vertices[static_cast<size_t>(idx[1]) - 1];
        auto c = raw_vertices[static_cast<size_t>(idx[2]) - 1];

        a.y -= min_y;
        b.y -= min_y;
        c.y -= min_y;

        auto norm = glm::normalize(glm::cross(b - a, c - a));

        res.emplace_back(a / max_y, norm);
        res.emplace_back(b / max_y, norm);
        res.emplace_back(c / max_y, norm);
    }

    return res;
}
