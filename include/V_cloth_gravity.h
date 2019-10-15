#include <Eigen/Dense>
#include <EigenTypes.h>

//  M - sparse mass matrix for the entire mesh
//  g - the acceleration due to gravity
//Output:
//  V - potential energy due to gravity for the entire mesh
void V_cloth_gravity(double &V, Eigen::SparseMatrixd &M, Eigen::Ref<const Eigen::Vector3d> g);