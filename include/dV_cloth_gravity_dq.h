#include <Eigen/Dense>
#include <EigenTypes.h>

//  M - sparse mass matrix for the entire mesh
//  g - the acceleration due to gravity
//Output:
//  fg - the gradient of the gravitational potential for the entire mesh
void dV_cloth_gravity_dq(Eigen::VectorXd &fg, Eigen::SparseMatrixd &M, Eigen::Ref<const Eigen::Vector3d> g);
