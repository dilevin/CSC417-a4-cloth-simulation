#include <init_state.h>

//void init_state(Eigen::Ref<Eigen::VectorXd> q, Eigen::Ref<Eigen::VectorXd> qdot, Eigen::Ref<const Eigen::MatrixXd> V)
void init_state(Eigen::VectorXd &q, Eigen::VectorXd &qdot, Eigen::Ref<const Eigen::MatrixXd> V) {

    q.resize(V.rows()*V.cols());
    qdot.resize(V.rows()*V.cols());

    Eigen::MatrixXd Vt = V.transpose();
    q = Eigen::Map<Eigen::VectorXd>(Vt.data(), Vt.rows()*Vt.cols());
    qdot.setZero();
}