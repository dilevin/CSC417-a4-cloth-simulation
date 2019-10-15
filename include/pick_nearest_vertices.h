#include <vector>

#include <Eigen/Dense>
#include <EigenTypes.h>

//NOTE: You can use the libigl igl::unproject function to implement this method

//Input:
//  win - window coordinate of mouse click (x_window, y_window, 0)
//  view - view transformation matrix
//  proj - projection matrix
//  viewport - viewport coordinates .
//  V - 3xn dense matrix of mesh vertices, each row of the matrix is a single vertex.
//  radius - selection radius for vertex picking
//Output:
//  verts - vertex ids (rows in V) of selected vertices
bool pick_nearest_vertices(std::vector<unsigned int> &verts, Eigen::Ref<const Eigen::Vector3d> win, 
                           Eigen::Ref<const Eigen::Matrix44f> view, Eigen::Ref<const Eigen::Matrix44f> proj, Eigen::Vector4f viewport,
                           Eigen::Ref<const Eigen::MatrixXd> V, Eigen::Ref<const Eigen::MatrixXi> F, double radius);