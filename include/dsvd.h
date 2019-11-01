#include <Eigen/Dense>
#include <EigenTypes.h>

//NOTE: Then tensor data structure stores arrays of matrix/vectors.
//      For a 3D tensor, such as dS, the entry dS[i][j] = dS/dF_ij, which is a vector
//      For a 4D tensors, such as dV, the entry dV[i][j] = dV/dF_ij which is a matrix
//Input:
//  F - this function computes the gradient of svd(F), where F is a 3x3 matrix. 
//Output (for the definition of the tensorial types, see EigenTypes.h):
//  dU - the 3x3x3x3 derivative of U wrt to F. dU[x][y][i][j] contains the derivative of U[x][y] wrt to F[i][j]
//  dV - the 3x3x3x3 derivative of U wrt to F. dV[x][y][i][j] contains the derivative of V[x][y] wrt to F[i][j]
//  dS - the 3x3x3 derivative of the singular values wrt to F. dS[x][i][j] contains the derivative of the x^{th} singlar value wrt to F[i][j]
void dsvd(Eigen::Tensor3333d &dU, Eigen::Tensor333d  &dS, Eigen::Tensor3333d &dV, Eigen::Ref<const Eigen::Matrix3d> F);
