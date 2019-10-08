#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <EigenTypes.h> 

void T_cloth(double &T, Eigen::Ref<const Eigen::VectorXd> qdot, Eigen::Ref<const Eigen::MatrixXd> V, Eigen::Ref<const Eigen::MatrixXi> F, Eigen::SparseMatrixd &M);