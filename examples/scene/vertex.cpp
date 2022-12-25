#include <fmt/core.h>
#include <fstream>
#include <stdexcept>

#include "vertex.hpp"

inline static std::vector<glm::vec3> calculate_mean_normals(std::vector<glm::vec3> const& vertices, std::vector<glm::uvec3> const& indices)
{
    // for each vertex the function takes all adjacent polygons, calculates vectors normal to the polygons
    // and assigns mean of all vertex's adjacent polygons to the vertex.
    // the function uses rolling mean to avoid storing all adjacent polygons' res

    std::vector<glm::vec3> res(vertices.size()); // rolling mean, {0,0,0} initially
    std::vector<size_t> counts(vertices.size()); // we need counts for the rolling mean
    for (auto const& polygon_idxs : indices) {
        auto const& a = vertices[static_cast<size_t>(polygon_idxs[0]) - 1];
        auto const& b = vertices[static_cast<size_t>(polygon_idxs[1]) - 1];
        auto const& c = vertices[static_cast<size_t>(polygon_idxs[2]) - 1];
        auto norm = glm::normalize(glm::cross(b - a, c - a));

        for (glm::length_t i{}; i < polygon_idxs.length(); ++i) {
            auto idx = polygon_idxs[i] - 1;
            auto n = ++counts[idx];
            auto mean_norm = res[idx];
            res[idx] = mean_norm + (1.0F / static_cast<float>(n)) * (norm - mean_norm);
        }
    }

    for (auto& norm : res) {
        norm = glm::normalize(norm);
    }

    return res;
}

std::pair<std::vector<glm::vec3>, std::vector<glm::uvec3>> parse_wavefront_file(std::string const& file_path)
{
    auto res = std::make_pair(std::vector<glm::vec3>{}, std::vector<glm::uvec3>{});
    auto& [raw_vertices, raw_indices] = res;

    std::ifstream content_file(file_path);
    if (!content_file) {
        throw std::runtime_error("could not open the file");
    }

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

    return res;
}

VertexModel read_vertex_model(std::string const& file_path)
{
    auto [raw_vertices, raw_indices] = parse_wavefront_file(file_path);

    // calculate boundaries
    auto max_p = glm::vec3(std::numeric_limits<float>::lowest());
    auto min_p = glm::vec3(std::numeric_limits<float>::max());
    for (auto v : raw_vertices) {
        max_p = glm::max(max_p, v);
        min_p = glm::min(min_p, v);
    }

    auto normals = calculate_mean_normals(raw_vertices, raw_indices);

    auto res = std::vector<Vertex>{};
    res.reserve(raw_indices.size() * 3);
    for (auto const& idx : raw_indices) {
        auto a = raw_vertices[static_cast<size_t>(idx[0]) - 1];
        auto b = raw_vertices[static_cast<size_t>(idx[1]) - 1];
        auto c = raw_vertices[static_cast<size_t>(idx[2]) - 1];

        // make sure that the model touches zx-plane, that prevents the model from "flying"
        // we leave z and x unchanged, so if model is not centered, it will remain not centered
        a.y -= min_p.y;
        b.y -= min_p.y;
        c.y -= min_p.y;
        max_p.y -= min_p.y;
        min_p.y -= min_p.y;

        res.emplace_back(a / max_p.y, normals[idx[0] - 1]);
        res.emplace_back(b / max_p.y, normals[idx[1] - 1]);
        res.emplace_back(c / max_p.y, normals[idx[2] - 1]);
    }

    return res;
}
