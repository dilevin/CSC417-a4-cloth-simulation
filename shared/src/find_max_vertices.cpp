#include <find_min_vertices.h>
#include <iostream>

void find_max_vertices(std::vector<unsigned int> &indices, Eigen::Ref<const Eigen::MatrixXd> V, double tol) {

    double max_vertex = V(0,1); 
    
    for(unsigned int vi=0; vi<V.rows(); ++vi) {
        max_vertex = (V(vi,1) > max_vertex ? V(vi,1) : max_vertex);
    }

    for(unsigned int vi=0; vi<V.rows(); ++vi) {

        if(std::abs(V(vi,1)-max_vertex) <= tol) {
            indices.push_back(vi);
        }
    }
}