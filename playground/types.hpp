#ifndef PLAYGROUND_HPP
#define PLAYGROUND_HPP

#include <Eigen/Eigen>
#include <glad/glad.h>

namespace playground {

using RowMajorMatrixXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMajorMatrixXui = Eigen::Matrix<GLuint, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;


} // namespace playground
#endif // PLAYGROUND_HPP
