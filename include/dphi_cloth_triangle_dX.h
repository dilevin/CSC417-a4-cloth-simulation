#include <Eigen/Dense>
#include <EigenTypes.h>

//Input:
//  V - the nx3 matrix of undeformed vertex positions. Each row is a single undeformed vertex position.
//  element - the 1x3 vertex indices for this tetrahedron
//  X - the 3D position in the underformed space at which to compute the gradient
//Output:
//  dphi - the 3x3 gradient of the the basis functions wrt to X. The i'th row stores d phi_i/dX
void dphi_cloth_triangle_dX(Eigen::Matrix3d &dphi, Eigen::Ref<const Eigen::MatrixXd> V, Eigen::Ref<const Eigen::RowVectorXi> element, Eigen::Ref<const Eigen::Vector3d> X);
