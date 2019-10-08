#include <Eigen/Dense>

void V_spring_particle_particle(double &V, Eigen::Ref<const Eigen::Vector3d> q0,  Eigen::Ref<const Eigen::Vector3d> q1, double l0, double stiffness);