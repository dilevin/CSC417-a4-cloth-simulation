## Introduction

This assignment will give you the chance to implement a simple cloth simulation. We will leverage our new found expertise on [finite element methods](www.github.com/dilevin/CSC2549-a3-finite-elements-3d) to build a FEM cloth simulation. This simulation will use triangles, rather than tetrahedron as the finite elements and will use a principal stretch-based model for the cloth material. You will also implement your first contact response model, a simple velocity filter that can be bolted onto standard time integration schemes. 

### Prerequisite installation

On all platforms, we will assume you have installed cmake and a modern c++
compiler on Mac OS X[¹](#¹macusers), Linux[²](#²linuxusers), or
Windows[³](#³windowsusers).

We also assume that you have cloned this repository using the `--recursive`
flag (if not then issue `git submodule update --init --recursive`). 

**Note:** We only officially support these assignments on Ubuntu Linux 18.04 (the OS the teaching labs are running) and OSX 10.13 (the OS I use on my personal laptop). While they *should* work on other operating systems, we make no guarantees. 

**All grading of assignments is done on Linux 18.04**

### Layout

All assignments will have a similar directory and file layout:

    README.md
    CMakeLists.txt
    main.cpp
    assignment_setup.h
    include/
      function1.h
      function2.h
      ...
    src/
      function1.cpp
      function2.cpp
      ...
    data/
      ...
    ...

The `README.md` file will describe the background, contents and tasks of the
assignment.

The `CMakeLists.txt` file setups up the cmake build routine for this
assignment.

The `main.cpp` file will include the headers in the `include/` directory and
link to the functions compiled in the `src/` directory. This file contains the
`main` function that is executed when the program is run from the command line.

The `include/` directory contains one file for each function that you will
implement as part of the assignment.

The `src/` directory contains _empty implementations_ of the functions
specified in the `include/` directory. This is where you will implement the
parts of the assignment.

The `data/` directory contains _sample_ input data for your program. Keep in
mind you should create your own test data to verify your program as you write
it. It is not necessarily sufficient that your program _only_ works on the given
sample data.

## Compilation for Debugging

This and all following assignments will follow a typical cmake/make build
routine. Starting in this directory, issue:

    mkdir build
    cd build
    cmake ..

If you are using Mac or Linux, then issue:

    make

## Compilation for Testing

Compiling the code in the above manner will yield working, but very slow executables. To run the code at full speed, you should compile it in release mode. Starting in the **build directory**, do the following:

    cmake .. -DCMAKE_BUILD_TYPE=Release
    
Followed by:

    make 
  
Your code should now run significantly (sometimes as much as ten times) faster. 

If you are using Windows, then running `cmake ..` should have created a Visual Studio solution file
called `a4-cloth-simulation.sln` that you can open and build from there. Building the project will generate an .exe file.

Why don't you try this right now?

## Execution

Once built, you can execute the assignment from inside the `build/` using 

    ./## Introduction

The third assignment will introduce you to the most common numerical method for simulating, well, almost anything in Computer Graphics, the mighty Finite Element Method.  

### Prerequisite installation

On all platforms, we will assume you have installed cmake and a modern c++
compiler on Mac OS X[¹](#¹macusers), Linux[²](#²linuxusers), or
Windows[³](#³windowsusers).

We also assume that you have cloned this repository using the `--recursive`
flag (if not then issue `git submodule update --init --recursive`). 

**Note:** We only officially support these assignments on Ubuntu Linux 18.04 (the OS the teaching labs are running) and OSX 10.13 (the OS I use on my personal laptop). While they *should* work on other operating systems, we make no guarantees. 

**All grading of assignments is done on Linux 18.04**

### Layout

All assignments will have a similar directory and file layout:

    README.md
    CMakeLists.txt
    main.cpp
    assignment_setup.h
    include/
      function1.h
      function2.h
      ...
    src/
      function1.cpp
      function2.cpp
      ...
    data/
      ...
    ...

The `README.md` file will describe the background, contents and tasks of the
assignment.

The `CMakeLists.txt` file setups up the cmake build routine for this
assignment.

The `main.cpp` file will include the headers in the `include/` directory and
link to the functions compiled in the `src/` directory. This file contains the
`main` function that is executed when the program is run from the command line.

The `include/` directory contains one file for each function that you will
implement as part of the assignment.

The `src/` directory contains _empty implementations_ of the functions
specified in the `include/` directory. This is where you will implement the
parts of the assignment.

The `data/` directory contains _sample_ input data for your program. Keep in
mind you should create your own test data to verify your program as you write
it. It is not necessarily sufficient that your program _only_ works on the given
sample data.

## Compilation for Debugging

This and all following assignments will follow a typical cmake/make build
routine. Starting in this directory, issue:

    mkdir build
    cd build
    cmake ..

If you are using Mac or Linux, then issue:

    make

## Compilation for Testing

Compiling the code in the above manner will yield working, but very slow executables. To run the code at full speed, you should compile it in release mode. Starting in the **build directory**, do the following:

    cmake .. -DCMAKE_BUILD_TYPE=Release
    
Followed by:

    make 
  
Your code should now run significantly (sometimes as much as ten times) faster. 

If you are using Windows, then running `cmake ..` should have created a Visual Studio solution file
called `a4-cloth-simulation.sln` that you can open and build from there. Building the project will generate an .exe file.

Why don't you try this right now?

## Execution

Once built, you can execute the assignment from inside the `build/` using 

    ./a4-cloth-simulation

While running, you can activate or de-activate the collision sphere by pressing `c`. 

## Background 

In this assignment we will move from the simulation of volumetric objects to the simulation of thin sheets or thin shells. For thin objects, rather than discretize the volume with tetrahedra (as was done in [assignment 2](https://github.com/dilevin/CSC2549-a2-mass-spring-3d) and [assignment 3](https://github.com/dilevin/CSC2549-a3-finite-elements-3d)) we discretize the [medial surface](http://www.unchainedgeometry.com/medial.html)  of the cloth.  This surface takes the form of a two-dimensional (2d) [manifold](https://en.wikipedia.org/wiki/Manifold) embedded in (3d) space. While many of the concepts you have already learned will carry over to this assignment, the major confounding factor will be evaluating the material model for the cloth on this manifold. To make things more interesting, we will see our first material model expressed in terms of the "principal stretches" of the deformation. 

In order to allow for more interesting interactions with the cloth, we will also implement collision detection and resolution with an analytical sphere. We will implement a simple collision resolution scheme, via velocity filter. These algorithms try to prevent collisions by "filtering" a previously computed velocity to remove any components that might make collisions worse. While running the assignment code, you can activate or de-activate the collision sphere by pressing `c`. 

## Resources

Somewhat sadly, it is difficult to find a comprehensive resource for modern cloth simulation. However, there are a collection of seminal papers that are helpful to peruse. The first, [Large-Steps in Cloth Simulation](https://www.cs.cmu.edu/~baraff/papers/sig98.pdf) is widely recognized for introducing the linearly-implicit time integrator to graphics. While we won't deal with bending stiffness (think the difference between a Kleenex and a steel sheet), [Discrete quadratic curvature energies](https://www.sciencedirect.com/science/article/abs/pii/S0167839607000891) are still the state-of-the-art approach for triangle mesh cloth. Finally, modern cloth simulators rely on [aggressive remeshing schemes](https://dl.acm.org/citation.cfm?id=2366171) to capture detail while retaining performance. 

## Finite Elements on Manifolds 

## Triangular Finite Elements 

## Generalized Coordinates and Velocities 

## Kinetic Energy

## Potential Energy

### Principal Stretches

### Linear Elasticity

### Co-Rotational Linear Elasticity 

### Derivatives of Co-Rotational Models 

## Collision Detection with Spheres

## Collision Resolution 

## Assignment Implementation

### Implementation Notes

### dphi_cloth_triangle_dX.cpp

Piecewise constant gradient matrix for linear shape functions. Row <img src="/tex/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode&sanitize=true" align=middle width=5.663225699999989pt height=21.68300969999999pt/> of the returned matrix contains the gradient of the <img src="/tex/3def24cf259215eefdd43e76525fb473.svg?invert_in_darkmode&sanitize=true" align=middle width=18.32504519999999pt height=27.91243950000002pt/> shape function.

### T_cloth.cpp

The kinetic energy of the whole cost mesh.

###  V_cloth_gravity.cpp

The potential energy due to gravity acting on the cloth mesh. 

###  dV_cloth_gravity_dq.cpp

Gradient of potential energy due to gravity

###  V_membrane_corotational.cpp

Potential energy for the cloth stretching force

###  dV_membrane_corotational_dq.cpp

Gradient of the cloth stretching energy.

###  d2V_membrane_corotational_dq2.cpp

Hessian matrix of the cloth stretching energy

###  V_spring_particle_particle.cpp

**Use your code from the last assignment**

###  dV_spring_particle_particle_dq.cpp

**Use your code from the last assignment**

###  mass_matrix_mesh.cpp

Assemble the full mass matrix for the entire tetrahedral mesh.

###  assemble_forces.cpp

Assemble the global force vector for the finite element mesh.

###  assemble_stiffness.cpp

Assemble the global stiffness matrix for the finite element mesh.

###  linearly_implicit_euler.h

**Use your code from the last assignment**

###  fixed_point_constraints.cpp

**Use your code from the last assignment**

###  collision_detection_cloth_sphere.cpp

Detect if any mesh vertex falls inside a sphere centered at (0,0,0.4) with radius 0.22

###  velocity_filter_cloth_sphere.cpp

Project out components of the per-vertex velocities which are in the **positive** direction of the contact normal

###  pick_nearest_vertices.cpp

**Use your code from the last assignment**


