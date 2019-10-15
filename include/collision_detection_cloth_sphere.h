#include <Eigen/Dense>
#include <EigenTypes.h>

//  q - generalized coordinates for the FEM system
//  center - the position of the sphere center in the world space
//  radius - the radius of the sphere in the world space 
//Output:
//  cloth_index - the indices of all vertices currently in contact with the sphere
//  normals - the outward facing contact normals for each contacting vertex. 
void collision_detection_cloth_sphere(std::vector<unsigned int> &cloth_index, std::vector<Eigen::Vector3d> &normals, Eigen::Ref<const Eigen::VectorXd> q,  Eigen::Ref<const Eigen::Vector3d> center, double radius);