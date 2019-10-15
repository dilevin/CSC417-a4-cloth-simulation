#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <EigenTypes.h>

//Input: 
//  q - generalized coordinates for the FEM system
//  V - the nx3 matrix of undeformed vertex positions
//  F - the mx3 matrix of triangle-vertex indices
//  density - the density of the cloth material
//  areas - the mx1 vector of undeformed triangle areas
//Output:
//  M - sparse mass matrix for the entire mesh
 void mass_matrix_mesh(Eigen::SparseMatrixd &M, Eigen::Ref<const Eigen::VectorXd> q, 
                         Eigen::Ref<const Eigen::MatrixXd> V, Eigen::Ref<const Eigen::MatrixXi> F,
                         double density, Eigen::Ref<const Eigen::VectorXd> areas);