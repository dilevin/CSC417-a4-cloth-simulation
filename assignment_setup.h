#ifndef ASSIGNMENT_SETUP_H
#define ASSIGNMENT_SETUP_H

//Assignment 4 code 
#include <igl/readMESH.h>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/readOFF.h>
#include <read_tetgen.h>
#include <igl/boundary_facets.h>
#include <igl/volume.h>

//assignment files for implementing simulation and interaction
#include <visualization.h>
#include <init_state.h>
#include <find_max_vertices.h>
#include <fixed_point_constraints.h>

#include <mass_matrix_mesh.h>
#include <linearly_implicit_euler.h>
#include <dsvd.h>

#include <dphi_cloth_triangle_dX.h>
#include <T_cloth.h>
#include <V_membrane_corotational.h>
#include <dV_membrane_corotational_dq.h>
#include <d2V_membrane_corotational_dq2.h>
#include <dV_cloth_gravity_dq.h>
#include <V_spring_particle_particle.h>
#include <dV_spring_particle_particle_dq.h>
#include <assemble_forces.h>
#include <assemble_stiffness.h>

//collision detection stuff
#include <collision_detection_cloth_sphere.h>
#include <velocity_filter_cloth_sphere.h>

//Variable for geometry
Eigen::MatrixXd V, V_skin; //vertices of simulation mesh //this will hold all individual pieces of cloth, I'll load some offsets
Eigen::MatrixXi F, F_skin; //faces of simulation mesh

Eigen::MatrixXd V_sphere, V_sphere_skin; //vertices of simulation mesh //this will hold all individual pieces of cloth, I'll load some offsets
Eigen::MatrixXi F_sphere, F_sphere_skin; //faces of simulation mesh

Eigen::SparseMatrixd N;

//material parameters
double density = 1;
double YM = 1e5; //young's modulus
double mu = 0.4; //poissons ratio
double C = (YM*mu)/((1.0+mu)*(1.0-2.0*mu));
double D = YM/(2.0*(1.0+mu));

//BC
std::vector<unsigned int> fixed_point_indices;
Eigen::SparseMatrixd P;
Eigen::VectorXd x0; 

//mass matrix
Eigen::SparseMatrixd M; //mass matrix
Eigen::VectorXd a0; //areas
Eigen::MatrixXd dX; //dX matrices for computing deformation gradients

//scratch memory for assembly
Eigen::VectorXd tmp_qdot;
Eigen::VectorXd tmp_force;
Eigen::VectorXd gravity;
Eigen::VectorXd qtmp;
Eigen::SparseMatrixd tmp_stiffness;

std::vector<std::pair<Eigen::Vector3d, unsigned int>> spring_points; //need this for interaction 

//collision detection stuff
bool collision_detection_on = false; 
std::vector<unsigned int> collision_indices;
std::vector<Eigen::Vector3d> collision_normals;

bool fully_implicit = false;

//selection spring
double k_selected = 1e5;

inline void simulate(Eigen::VectorXd &q, Eigen::VectorXd &qdot, double dt, double t) {  

    double V_ele, T_ele, KE,PE;

    spring_points.clear();

    //Interaction spring
    Eigen::Vector3d mouse;
    Eigen::Vector6d dV_mouse;
    double k_selected_now = (Visualize::is_mouse_dragging() ? k_selected : 0.);
    
    for(unsigned int pickedi = 0; pickedi < Visualize::picked_vertices().size(); pickedi++) {   
        spring_points.push_back(std::make_pair((P.transpose()*q+x0).segment<3>(3*Visualize::picked_vertices()[pickedi]) + Visualize::mouse_drag_world() + Eigen::Vector3d::Constant(1e-6),3*Visualize::picked_vertices()[pickedi]));
    }

    
    KE = PE = 0.;

    auto energy = [&](Eigen::Ref<const Eigen::VectorXd> qdot_1)->double {
        double E = 0;
        Eigen::VectorXd newq = P.transpose()*(q+dt*qdot_1)+x0;

        for(unsigned int ei=0; ei<F.rows(); ++ei) {
            
            V_membrane_corotational(V_ele,newq , Eigen::Map<Eigen::Matrix3d>(dX.row(ei).data()), V, F.row(ei), a0(ei), C, D);
            E += V_ele;
        }

        for(unsigned int pickedi = 0; pickedi < spring_points.size(); pickedi++) {   
            V_spring_particle_particle(V_ele, spring_points[pickedi].first, newq.segment<3>(spring_points[pickedi].second), 0.0, k_selected_now);
            E += V_ele;
        }

        E += 0.5*(qdot_1 - qdot).transpose()*M*(qdot_1 - qdot);

        return E;
    };

    auto force = [&](Eigen::VectorXd &f, Eigen::Ref<const Eigen::VectorXd> q2, Eigen::Ref<const Eigen::VectorXd> qdot2) { 
        
            assemble_forces(f, P.transpose()*q2+x0, P.transpose()*qdot2, dX, V, F, a0, C,D);
            f -= gravity;
        
            for(unsigned int pickedi = 0; pickedi < spring_points.size(); pickedi++) {
                dV_spring_particle_particle_dq(dV_mouse, spring_points[pickedi].first, (P.transpose()*q2+x0).segment<3>(spring_points[pickedi].second), 0.0, k_selected_now);
                f.segment<3>(3*Visualize::picked_vertices()[pickedi]) -= dV_mouse.segment<3>(3);
            }

        
            f = P*f;
            //std::cout<<"Force: "<<f.transpose()<<"\n";
        };

    //assemble stiffness matrix,
    auto stiffness = [&](Eigen::SparseMatrixd &K, Eigen::Ref<const Eigen::VectorXd> q2, Eigen::Ref<const Eigen::VectorXd> qdot2) { 
        assemble_stiffness(K, P.transpose()*q2+x0, P.transpose()*qdot2, dX, V, F, a0, C, D);
        K = P*K*P.transpose();
    };

    //run collision detector
    if(collision_detection_on) {
        collision_detection_cloth_sphere(collision_indices, collision_normals, q, Eigen::Vector3d(0.0, 0.0, 0.4), 0.22);
    }

    qtmp = q; 
    //unconstrained velocity
    linearly_implicit_euler(q, qdot, dt, M, force, stiffness, tmp_force, tmp_stiffness);
    
    //velocity filter 
    if(collision_detection_on) {
        velocity_filter_cloth_sphere(qdot, collision_indices, collision_normals);
    }

    q = qtmp + dt*qdot;
    //std::cout<<q.transpose()<<"\n";
}

inline void draw(Eigen::Ref<const Eigen::VectorXd> q, Eigen::Ref<const Eigen::VectorXd> qdot, double t) {

    //update vertex positions using simulation
    Visualize::update_vertex_positions(0, P.transpose()*q + x0);

}

bool key_down_callback(igl::opengl::glfw::Viewer &viewer, unsigned char key, int modifiers) {

    if(key =='N') {
        std::cout<<"toggle integrators \n";
        fully_implicit = !fully_implicit;
    } 
    if(key == 'C') {
        collision_detection_on = !collision_detection_on;
        Visualize::set_visible(1, collision_detection_on);
    }

    return false;
}
inline void assignment_setup(int argc, char **argv, Eigen::VectorXd &q, Eigen::VectorXd &qdot) {

    //load geometric data 
    igl::readOBJ("../data/square_cloth.obj", V, F);
    igl::readOBJ("../data/sphere.obj", V_sphere, F_sphere);

    //setup simulation 
    init_state(q,qdot,V);

    //add geometry to scene
    V_skin = V;
    F_skin = F;
    N.resize(V.rows(), V.rows());
    N.setIdentity();

    Visualize::add_object_to_scene(V,F, V_skin, F_skin, N, Eigen::RowVector3d(244,165,130)/255.);

    //add collision sphere to scene 
    V_sphere_skin = V_sphere;
    F_sphere_skin = F_sphere;
    N.resize(V.rows(), V.rows());
    N.setIdentity();

    Visualize::add_object_to_scene(V_sphere,F_sphere, V_sphere_skin, F_sphere_skin, N, Eigen::RowVector3d(244,165,130)/255.);
    Visualize::set_visible(1, collision_detection_on);

    //compute dX and area of each face;
    dX.resize(F.rows(), 9);
    a0.resize(F.rows(),1);
    for(unsigned int ii=0; ii<F.rows(); ++ii) {
        Eigen::Matrix3d dX_tmp;
        //Eigen::Matrix<double, 1,9> tmp_row;
        dphi_cloth_triangle_dX(dX_tmp, V, F.row(ii), Eigen::Vector3d(0.,0.,0.)); //X doesn't matter since dphi is constant per element.
        dX.row(ii) = Eigen::Map<Eigen::Matrix<double, 1, 9> >(dX_tmp.data());

        //std::cout<<"dX_tmp:\n "<<dX_tmp<<"\n";
        //std::cout<<"dX row: "<<dX.row(ii)<<"\n";
        
        //tmp_row = dX.row(ii);
        //std::cout<<"Retrieval: \n"<<Eigen::Map<const Eigen::Matrix3d>(tmp_row.data())<<"\n";
        //Heron's formula for area
        double side_a = (V.row(F(ii,1)) - V.row(F(ii,0))).norm();
        double side_b = (V.row(F(ii,2)) - V.row(F(ii,1))).norm();
        double side_c = (V.row(F(ii,0)) - V.row(F(ii,2))).norm();
        double s = (side_a+side_b+side_c)/2.;
        a0(ii) = sqrt(s*(s-side_a)*(s-side_b)*(s-side_c));
     }
    
    //Mass Matrix
    mass_matrix_mesh(M, q,  V, F, density, a0);

    if(M.rows() == 0) {
        std::cout<<"Mass matrix not implemented, exiting.\n";
        exit(1);
    }
    
    //should be max verts for cloth simulation
    find_max_vertices(fixed_point_indices, V, 0.001);
    
    P.resize(q.rows(),q.rows());
    P.setIdentity();
    fixed_point_constraints(P, q.rows(), fixed_point_indices);
    
    x0 = q - P.transpose()*P*q; //vector x0 contains position of all fixed nodes, zero for everything else    
    
    //constant gravity vector
    gravity.resize(q.rows(),1);
    dV_cloth_gravity_dq(gravity, M, Eigen::Vector3d(0,-1800.8,0));

    //std::cout<<"Gravity "<<gravity.transpose()<<"\n";
    
    //correct M, q and qdot so they are the right size
    q = P*q;
    qdot = P*qdot;
    M = P*M*P.transpose();
    
    Visualize::viewer().callback_key_down = key_down_callback;

}

#endif

