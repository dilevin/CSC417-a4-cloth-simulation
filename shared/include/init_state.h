#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <EigenTypes.h>

void init_state(Eigen::VectorXd &q, Eigen::VectorXd &qdot, Eigen::Ref<const Eigen::MatrixXd> V);