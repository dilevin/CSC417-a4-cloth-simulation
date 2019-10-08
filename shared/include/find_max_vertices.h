#include <Eigen/Dense>
#include <vector>

void find_max_vertices(std::vector<unsigned int> &indices, Eigen::Ref<const Eigen::MatrixXd> V, double tol = 1e-3);