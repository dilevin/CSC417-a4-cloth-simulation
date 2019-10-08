#include <find_min_vertices.h>
#include <iostream>

void find_min_vertices(std::vector<unsigned int> &indices, Eigen::Ref<const Eigen::MatrixXd> V, double tol) {

    double min_vertex = V(0,1); 
    
    for(unsigned int vi=0; vi<V.rows(); ++vi) {
        min_vertex = (V(vi,1) < min_vertex ? V(vi,1) : min_vertex);
    }

    for(unsigned int vi=0; vi<V.rows(); ++vi) {

        if(std::abs(V(vi,1)-min_vertex) <= tol) {
            indices.push_back(vi);
        }
    }
}