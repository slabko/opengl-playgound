#include <array>
#include <fstream>
#include <limits>
#include <numeric>
#include <stdexcept>

#include "vertex.hpp"

static std::pair<std::vector<glm::vec3>, std::vector<glm::uvec3>> parse_wavefront_file(std::string const& file_path);

static std::pair<glm::vec3, glm::vec3> elementwise_minmax(std::vector<glm::vec3> const& vertices);

static std::vector<glm::vec3> calculate_mean_normals(
  std::vector<glm::vec3> const& vertices,
  std::vector<glm::uvec3> const& indices);

VertexModel read_vertex_model(std::string const& file_path)
{
    auto [raw_vertices, raw_indices] = parse_wavefront_file(file_path);
    auto [min_p, max_p] = elementwise_minmax(raw_vertices);

    auto normals = calculate_mean_normals(raw_vertices, raw_indices);

    // make sure that the model touches zx-plane, that prevents the model from "flying"
    // we leave z and x unchanged, so if model is not centered, it will remain not centered
    auto offset = glm::vec3(0.0F, min_p.y, 0.0F);
    auto scale = 1 / max_p.y - offset.y;

    auto res = std::vector<Vertex>{};
    res.reserve(raw_indices.size() * 3);
    for (auto const& idx : raw_indices) {
        for (glm::length_t i{}; i < idx.length(); ++i) {
            auto v = raw_vertices[static_cast<size_t>(idx[i])];
            v -= offset;
            res.emplace_back(v * scale, normals[idx[i]]);
        }
    }

    return res;
}

/*******************************************************************************
 * parses a wavefront file and returns vertices and vectors of three elements
 * each representing a polygon of a model
 ******************************************************************************/
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
            raw_indices.emplace_back(a - 1, b - 1, c - 1);
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

/*******************************************************************************
 * Calculates elementwise min and max of a iterator of vectors.
 * Example: given {-1, 1, 0} and {0, -1, 1} returns {-1, -1, 0} and {0, 1, 1}
 ******************************************************************************/
std::pair<glm::vec3, glm::vec3> elementwise_minmax(std::vector<glm::vec3> const& vertices)
{
    auto elementwise_min = [](glm::vec3 a, glm::vec3 b) {
        return glm::min(a, b);
    };

    auto elementwise_max = [](glm::vec3 a, glm::vec3 b) {
        return glm::max(a, b);
    };

    auto max_v = std::reduce(
      vertices.cbegin(), vertices.cend(),
      glm::vec3{std::numeric_limits<float>::lowest()},
      elementwise_max);

    auto min_v = std::reduce(
      vertices.cbegin(), vertices.cend(),
      glm::vec3{std::numeric_limits<float>::max()},
      elementwise_min);

    return std::make_pair(min_v, max_v);
}

/*******************************************************************************
 * for each vertex the function takes all adjacent polygons,
 * calculates their normal vectors and assigns mean of these normal
 * to the given vertex
 ******************************************************************************/
std::vector<glm::vec3> calculate_mean_normals(
  std::vector<glm::vec3> const& vertices,
  std::vector<glm::uvec3> const& indices)
{
    std::vector<glm::vec3> res(vertices.size()); // rolling mean, {0,0,0} initially
    std::vector<size_t> counts(vertices.size()); // we need counts for the rolling mean

    for (auto const& polygon_indices : indices) {
        auto const& a = vertices[polygon_indices[0]];
        auto const& b = vertices[polygon_indices[1]];
        auto const& c = vertices[polygon_indices[2]];
        auto norm = glm::normalize(glm::cross(b - a, c - a));

        for (glm::length_t i{}; i < polygon_indices.length(); ++i) {
            auto idx = polygon_indices[i];
            auto n = static_cast<float>(++counts[idx]);
            auto mean_norm = res[idx];
            res[idx] = glm::normalize(mean_norm + (1.0F / n) * (norm - mean_norm));
        }
    }

    return res;
}