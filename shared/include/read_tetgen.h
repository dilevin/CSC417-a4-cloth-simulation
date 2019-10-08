#include <Eigen/Dense>
#include <EigenTypes.h>
#include <iostream>
#include <fstream>

 void read_tetgen(Eigen::MatrixXd &V, Eigen::MatrixXi &F, const std::string nodeFile, const std::string eleFile);