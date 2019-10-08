#include <dsvd.h>

void dsvd(Eigen::Tensor3333d &dU, Eigen::Tensor333d  &dS, Eigen::Tensor3333d &dV, Eigen::Ref<const Eigen::Matrix3d> Fin) {

    Eigen::Matrix3d UVT, tmp, U,V;
    Eigen::Matrix3d lambda;
    Eigen::Matrix3d F;
    Eigen::Vector3d S; 
    //get the SVD 
    F = Fin;
    Eigen::JacobiSVD<Eigen::Matrix3d> svd(F, Eigen::ComputeFullU | Eigen::ComputeFullV);
    U = svd.matrixU();
    V = svd.matrixV();
    S = svd.singularValues();
    
    //crappy hack for now
    double tol = 1e-5;

     if(std::fabs(S[0] - S[1]) < tol || std::fabs(S[1] - S[2]) < tol || std::fabs(S[0] - S[2]) < tol) {
        F += Eigen::Matrix3d::Random()*tol;
        Eigen::JacobiSVD<Eigen::Matrix3d> svd2(F, Eigen::ComputeFullU | Eigen::ComputeFullV);
        U = svd2.matrixU();
        V = svd2.matrixV();
        S = svd2.singularValues();
    }

    double w01, w02, w12;
    double d01, d02, d12;
    
    d01 = S(1)*S(1)-S(0)*S(0);
    d02 = S(2)*S(2)-S(0)*S(0);
    d12 = S(2)*S(2)-S(1)*S(1);
    
    //corresponds to conservative solution --- if singularity is detected no angular velocity
    d01 = 1.0/(std::abs(d01) < tol ? std::numeric_limits<double>::infinity() : d01);
    d02 = 1.0/(std::abs(d02) < tol ? std::numeric_limits<double>::infinity() : d02);
    d12 = 1.0/(std::abs(d12) < tol ? std::numeric_limits<double>::infinity() : d12);
    
    for(unsigned int r=0; r<3; ++r) {
        for(unsigned int s =0; s <3; ++s) {
            
            UVT = U.row(r).transpose()*V.row(s);
            
            //Compute dS
            dS[r][s] = UVT.diagonal();
            
            UVT -= dS[r][s].asDiagonal();
            
            tmp  = S.asDiagonal()*UVT + UVT.transpose()*S.asDiagonal();
            w01 = tmp(0,1)*d01;
            w02 = tmp(0,2)*d02;
            w12 = tmp(1,2)*d12;
            tmp << 0, w01, w02,
                    -w01, 0, w12,
                    -w02, -w12, 0;
            
            dV[r][s] = V*tmp;
            
            tmp = UVT*S.asDiagonal() + S.asDiagonal()*UVT.transpose();
            w01 = tmp(0,1)*d01;
            w02 = tmp(0,2)*d02;
            w12 = tmp(1,2)*d12;
            tmp << 0, w01, w02,
            -w01, 0, w12,
            -w02, -w12, 0;
            
            dU[r][s] = U*tmp;
                        
        }
    }
}
