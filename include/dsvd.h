#include <Eigen/Dense>
#include <EigenTypes.h>

//computes derivative of SVD and stores in tensor variables 
//matrix valued [r][s] tensor entry is the derivative of that quantity with respect to F(r,s)
void dsvd(Eigen::Tensor3333d &dU, Eigen::Tensor333d  &dS, Eigen::Tensor3333d &dV, Eigen::Ref<const Eigen::Matrix3d> F);
