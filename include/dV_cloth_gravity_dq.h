#include <Eigen/Dense>
#include <EigenTypes.h>

//compute gravity vector for entire mesh, all at once 
void dV_cloth_gravity_dq(Eigen::VectorXd &fg, Eigen::SparseMatrixd &M, Eigen::Ref<const Eigen::Vector3d> g);
