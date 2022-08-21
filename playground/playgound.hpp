#ifndef PLAYGROUND_HPP
#define PLAYGROUND_HPP

#include <Eigen/Eigen>

namespace playground {

using MatrixXf = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using Matrix4f = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;

} // namespace playground
#endif // PLAYGROUND_HPP
