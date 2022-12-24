#include "static_shape.hpp"

#include <fstream>
#include <iostream>
#include <limits>

#include <glm/gtx/io.hpp>
#include <spdlog/spdlog.h>

StaticShape::StaticShape(std::shared_ptr<VertexModel const> model) :
  model_{std::move(model)} {}

void StaticShape::update()
{
    vertices_.clear();
    vertices_.reserve(model_->size());
    std::copy(model_->cbegin(), model_->cend(), std::back_inserter(vertices_));
}
