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

Our major challenge in this assignment arises from the difference in the dimensionality of the cloth material and the world (deformed) space (**NOTE:** I use the terms world and deformed space interchangeably). Cloth is locally two-dimensional (*2d*) while the world  is [*3d*](https://www.quora.com/What-are-all-of-Calvins-alter-egos).  What will be comforting is that, a relatively straight-forward application of the finite-element-method (FEM) will allow us to build a passable dynamic cloth simulator. 

## Triangular Finite Elements 

The previous assignment applied FEM to *volumetric* simulation -- the simulation of objects with geometry of dimension equal to that of the world space (i.e our bunny and armadillo were 3d as was the world). In this case our finite elements were also volumetric ... they were tetrahedra in the undeformed space of the simulated object. 

In the case of cloth, the underformed  geometry is of different dimension (2d) than the world space (3d). Because our finite elements divide up the undeformed space, they also need to be 2d. As such we will use [triangles](https://en.wikipedia.org/wiki/Triangle), not tetrahedra as our elements.

## Generalized Coordinates and Velocities 

Just as in the [previous assignment](https://github.com/dilevin/CSC2549-a3-finite-elements-3d), we need to choose basis, or shape functions with which to approximate functions on our, now triangular, mesh. A triangle as 3 nodes our approximations become

<p align="center"><img src="/tex/1767c7f7922c97563d66b8446ba75840.svg?invert_in_darkmode&sanitize=true" align=middle width=147.58684215pt height=47.35857885pt/></p> 

where <img src="/tex/c83e439282bef5aadf55a91521506c1a.svg?invert_in_darkmode&sanitize=true" align=middle width=14.44544309999999pt height=22.831056599999986pt/> are the [barycentric coordinates](https://en.wikipedia.org/wiki/Barycentric_coordinate_system) for a 2D triangle and <img src="/tex/58c2313bc1ea66cc4d86d27f83c1941d.svg?invert_in_darkmode&sanitize=true" align=middle width=55.34000504999999pt height=26.76175259999998pt/> is the 2d coordinate in the undeformed space. 

Our goal is to be able to estimate the 3d world space position of any part of this cloth triangle (for any value of <img src="/tex/319d907db67f3000780e9b2d1a2816d9.svg?invert_in_darkmode&sanitize=true" align=middle width=14.764759349999988pt height=22.55708729999998pt/> in the triangle). Using our FEM basis, this becomes

<p align="center"><img src="/tex/e380f5951fb9f765922182550d7fa939.svg?invert_in_darkmode&sanitize=true" align=middle width=155.77825395pt height=47.35857885pt/></p>

where <img src="/tex/07fa8f51f4cd0e31f63743c0638a3388.svg?invert_in_darkmode&sanitize=true" align=middle width=56.34005519999999pt height=26.76175259999998pt/> are the 3d, per-vertex positions of the cloth mesh at time <img src="/tex/4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode&sanitize=true" align=middle width=5.936097749999991pt height=20.221802699999984pt/>. This gives a mapping from the 2d space of the undeformed cloth to the 3d world space.  As usual, we choose the **generalized coordinates** (<img src="/tex/84a62034abeb6bfffada277b45a096c7.svg?invert_in_darkmode&sanitize=true" align=middle width=50.55236834999999pt height=26.76175259999998pt/>) to be the stacked vector of vertex positions, which lets us rewrite the above expression as 

<p align="center"><img src="/tex/99876db7d3f4ab121c85347e0275a67f.svg?invert_in_darkmode&sanitize=true" align=middle width=332.70013754999997pt height=102.84090135pt/></p>
 
The velocity of the cloth, at any point <img src="/tex/319d907db67f3000780e9b2d1a2816d9.svg?invert_in_darkmode&sanitize=true" align=middle width=14.764759349999988pt height=22.55708729999998pt/> is then given by the total time derivative: 

<p align="center"><img src="/tex/873246b3685f69b2a152de8f0199546c.svg?invert_in_darkmode&sanitize=true" align=middle width=332.96270534999996pt height=103.61714219999999pt/></p> 

which defines the **generalized velocities** as the stacked *9d* vector of per-vertex velocities. 

## Deformation Gradient 

The final necessary piece of the kinematic puzzle is the measure of deformation. You might be tempted to just compute <img src="/tex/247ed86bbeef7615018d6bc51947544a.svg?invert_in_darkmode&sanitize=true" align=middle width=57.78279374999998pt height=32.976995699999996pt/> but this is going to get you in trouble. The dimensions of this matrix (<img src="/tex/88787d1576b44fdadc099c2c2d5cd9f6.svg?invert_in_darkmode&sanitize=true" align=middle width=70.25574269999998pt height=26.76175259999998pt/>) make evaluating material models difficult since such models are *designed* to work for volumetric (re: square) deformation matrices. 

There are lots of ways to handle this problem in [literature](https://animation.rwth-aachen.de/media/papers/2013-CAG-AdaptiveCloth.pdf) and in this assignment we will rely on one which is both simple and effective. 

First let's remind ourselves that the functions which define barycentric coordinates require us to solve the linear system

<p align="center"><img src="/tex/7d6ab38d6d05964698ae8a092d1948fc.svg?invert_in_darkmode&sanitize=true" align=middle width=327.7252077pt height=39.452455349999994pt/></p>

To "square" our deformation gradient, we are going to "lift" the undeformed space of the cloth to 3d. **NOTE:** we are still going to use 2D triangles but now the undeformed vertex positions of those triangles will be given in 3d. Let's call the 3d undeformed vertex positions of our triangle mesh <img src="/tex/9b01119ffd35fe6d8a8795a24fc11616.svg?invert_in_darkmode&sanitize=true" align=middle width=18.943064249999992pt height=22.55708729999998pt/>. So now, given any point in this weird 3d undeformed space, the second and third barycentric coordinates are given by 

<p align="center"><img src="/tex/e1df9ab5f065f666cdd061d6ef23f8f4.svg?invert_in_darkmode&sanitize=true" align=middle width=326.7800316pt height=53.80252679999999pt/></p>.

Ok, we've made everything worse. Now we can't even directly invert the right-hand side. But this is one of those cases wherein things had to get [worse before they get better](https://www.youtube.com/watch?v=uDIgS-Soo9Q). However, we can solve this system in a [least-squares](https://en.wikipedia.org/wiki/Least_squares) sense which gives us

<p align="center"><img src="/tex/79361f938b80de1c32bb7d7724ce3590.svg?invert_in_darkmode&sanitize=true" align=middle width=249.66638894999997pt height=39.452455349999994pt/></p>

which, when coupled with the fact that <img src="/tex/358bf7b214f6e3ed24776b7ec9a31866.svg?invert_in_darkmode&sanitize=true" align=middle width=121.00431914999999pt height=22.831056599999986pt/> gives us everything we need. 

This might seem like an esoteric, algebraic solution, but geometrically it is doing something really quite reasonable. 




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


