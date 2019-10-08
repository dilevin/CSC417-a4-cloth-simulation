#include <Eigen/Dense>
#include <EigenTypes.h>

void dV_membrane_corotational_dq(Eigen::Vector9d &f, Eigen::Ref<const Eigen::VectorXd> q, Eigen::Ref<const Eigen::Matrix3d> dX, 
                          Eigen::Ref<const Eigen::MatrixXd> V, Eigen::Ref<const Eigen::RowVectorXi> element, double area, 
                          double mu, double lambda);