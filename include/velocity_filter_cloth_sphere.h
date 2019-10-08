#include <Eigen/Dense>
#include <EigenTypes.h>

void velocity_filter_cloth_sphere(Eigen::VectorXd &qdot, const std::vector<unsigned int> &index, 
                                  const std::vector<Eigen::Vector3d> &normals);