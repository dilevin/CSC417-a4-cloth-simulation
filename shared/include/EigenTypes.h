#ifndef EIGENTYPES_H
#define EIGENTYPES_H

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <array>

namespace Eigen {

    //dense types
    using Vector4d = Eigen::Matrix<double,4,1>;
    using Vector6d = Eigen::Matrix<double, 6,1>;
    using Vector9d = Eigen::Matrix<double, 9, 1>;
    using Vector12d = Eigen::Matrix<double, 12,1>;
    
    using Matrix32d = Eigen::Matrix<double, 3,2>;
    using Matrix34d = Eigen::Matrix<double, 3,4>;
    using Matrix36d = Eigen::Matrix<double, 3,6>;
    using Matrix39d = Eigen::Matrix<double, 3, 9>;
    using Matrix43d = Eigen::Matrix<double, 4,3>;
    using Matrix66d  = Eigen::Matrix<double, 6,6>;
    using Matrix99d = Eigen::Matrix<double, 9, 9>;
    using Matrix93d = Eigen::Matrix<double, 9, 3>;
    using Matrix1212d = Eigen::Matrix<double, 12,12>;
    using Matrix44f = Eigen::Matrix<float, 4,4>;
    
    //sparse types
    using SparseMatrixd = Eigen::SparseMatrix<double>;

    //tensor types (which are just arrays of matrices)
    using Tensor333d = std::array<std::array<Eigen::Vector3d,3>, 3>;
    using Tensor3333d = std::array<std::array<Eigen::Matrix3d,3>, 3>;

}

inline double stablePow(double a, double b) {        
    return static_cast<double> (std::pow(std::cbrt(static_cast<double>(a)),static_cast<double>(b)));
}

inline void inverse33(Eigen::Ref<Eigen::Matrix3d> result, Eigen::Ref<const Eigen::Matrix3d> A) {
    double determinant =    +A(0,0)*(A(1,1)*A(2,2)-A(2,1)*A(1,2))
                            -A(0,1)*(A(1,0)*A(2,2)-A(1,2)*A(2,0))
                            +A(0,2)*(A(1,0)*A(2,1)-A(1,1)*A(2,0));
    double invdet = 1/determinant;
    result(0,0) =  (A(1,1)*A(2,2)-A(2,1)*A(1,2))*invdet;
    result(1,0) = -(A(0,1)*A(2,2)-A(0,2)*A(2,1))*invdet;
    result(2,0) =  (A(0,1)*A(1,2)-A(0,2)*A(1,1))*invdet;
    result(0,1) = -(A(1,0)*A(2,2)-A(1,2)*A(2,0))*invdet;
    result(1,1) =  (A(0,0)*A(2,2)-A(0,2)*A(2,0))*invdet;
    result(2,1) = -(A(0,0)*A(1,2)-A(1,0)*A(0,2))*invdet;
    result(0,2) =  (A(1,0)*A(2,1)-A(2,0)*A(1,1))*invdet;
    result(1,2) = -(A(0,0)*A(2,1)-A(2,0)*A(0,1))*invdet;
    result(2,2) =  (A(0,0)*A(1,1)-A(1,0)*A(0,1))*invdet;
    result.transposeInPlace();
}



#endif 
