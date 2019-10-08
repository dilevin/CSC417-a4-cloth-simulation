#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <EigenTypes.h>

#include <vector>

void fixed_point_constraints(Eigen::SparseMatrixd &P, unsigned int q_size, const std::vector<unsigned int> indices);