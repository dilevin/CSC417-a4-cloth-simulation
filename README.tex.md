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

    ./a4-cloth-simulation

While running, you can activate or de-activate the collision sphere by pressing `c`. 

## Background 

In this assignment we will move from the simulation of volumetric objects to the simulation of thin sheets or thin shells. For thin objects, rather than discretize the volume with tetrahedra (as was done in [assignment 2](https://github.com/dilevin/CSC2549-a2-mass-spring-3d) and [assignment 3](https://github.com/dilevin/CSC2549-a3-finite-elements-3d)) we discretize the [medial surface](http://www.unchainedgeometry.com/medial.html)  of the cloth.  This surface takes the form of a two-dimensional (2d) [manifold](https://en.wikipedia.org/wiki/Manifold) embedded in (3d) space. While many of the concepts you have already learned will carry over to this assignment, the major confounding factor will be evaluating the material model for the cloth on this manifold. To make things more interesting, we will see our first material model expressed in terms of the "principal stretches" of the deformation. 

In order to allow for more interesting interactions with the cloth, we will also implement collision detection and resolution with an analytical sphere. We will implement a simple collision resolution scheme, via velocity filter. These algorithms try to prevent collisions by "filtering" a previously computed velocity to remove any components that might make collisions worse. While running the assignment code, you can activate or de-activate the collision sphere by pressing `c`. 

![Cloth simulation!](images/cloth.gif)

## Resources

Somewhat sadly, it is difficult to find a comprehensive resource for modern cloth simulation. However, there are a collection of seminal papers that are helpful to peruse. The first, [Large-Steps in Cloth Simulation](https://www.cs.cmu.edu/~baraff/papers/sig98.pdf) is widely recognized for introducing the linearly-implicit time integrator to graphics. While we won't deal with bending stiffness (think the difference between a Kleenex and a steel sheet), [Discrete quadratic curvature energies](https://www.sciencedirect.com/science/article/abs/pii/S0167839607000891) are still the state-of-the-art approach for triangle mesh cloth. Finally, modern cloth simulators rely on [aggressive remeshing schemes](https://dl.acm.org/citation.cfm?id=2366171) to capture detail while retaining performance. 

## Finite Elements on Manifolds 

Our major challenge in this assignment arises from the difference in the dimensionality of the cloth material and the world (deformed) space (**NOTE:** I use the terms world and deformed space interchangeably). Cloth is locally two-dimensional (*2d*) while the world  is [*3d*](https://www.quora.com/What-are-all-of-Calvins-alter-egos).  What will be comforting is that, a relatively straight-forward application of the finite-element-method (FEM) will allow us to build a passable dynamic cloth simulator. 

## Triangular Finite Elements 

The previous assignment applied FEM to *volumetric* simulation -- the simulation of objects with geometry of dimension equal to that of the world space (i.e our bunny and armadillo were 3d as was the world). In this case our finite elements were also volumetric ... they were tetrahedra in the undeformed space of the simulated object. 

In the case of cloth, the underformed  geometry is of different dimension (2d) than the world space (3d). Because our finite elements divide up the undeformed space, they also need to be 2d. As such we will use [triangles](https://en.wikipedia.org/wiki/Triangle), not tetrahedra as our elements.

## Generalized Coordinates and Velocities 

Just as in the [previous assignment](https://github.com/dilevin/CSC2549-a3-finite-elements-3d), we need to choose basis, or shape functions with which to approximate functions on our, now triangular, mesh. A triangle as 3 nodes our approximations become

$$ f\left(\mathbf{Y}\right)=\sum_{i=0}^{2}f_i\phi_i\left(\mathbf{Y}\right) $$ 

where $\phi_i$ are the [barycentric coordinates](https://en.wikipedia.org/wiki/Barycentric_coordinate_system) for a 2D triangle and $\mathbf{Y} \in \mathcal{R}^2$ is the 2d coordinate in the undeformed space. 

Our goal is to be able to estimate the 3d world space position of any part of this cloth triangle (for any value of $\mathbf{Y}$ in the triangle). Using our FEM basis, this becomes

$$ \mathbf{x}^t\left(\mathbf{Y}\right)=\sum_{i=0}^{2}\mathbf{x}^t_i\phi_i\left(\mathbf{Y}\right) $$

where $\mathbf{x}^t_i \in \mathcal{R}^3$ are the 3d, per-vertex positions of the cloth mesh at time $t$. This gives a mapping from the 2d space of the undeformed cloth to the 3d world space.  As usual, we choose the **generalized coordinates** ($\mathbf{q} \in \mathcal{R}^9$) to be the stacked vector of vertex positions, which lets us rewrite the above expression as 

$$\mathbf{x}^t\left(\mathbf{Y}\right) = \underbrace{\begin{bmatrix} \phi_0\left(\mathbf{Y}\right)I & \phi_1\left(\mathbf{Y}\right)I& \phi_2\left(\mathbf{Y}\right)I \end{bmatrix}}_{N} \underbrace{\begin{bmatrix} \mathbf{x}^t_0 \\ \mathbf{x}^t_1 \\ \mathbf{x}^t_2 \end{bmatrix}}_{\mathbf{q}}$$
 
The velocity of the cloth, at any point $\mathbf{Y}$ is then given by the total time derivative: 

$$\mathbf{v}^t\left(\mathbf{Y}\right) = \underbrace{\begin{bmatrix} \phi_0\left(\mathbf{Y}\right)I & \phi_1\left(\mathbf{Y}\right)I& \phi_2\left(\mathbf{Y}\right)I \end{bmatrix}}_{N} \underbrace{\begin{bmatrix} \dot{\mathbf{x}}^t_0 \\ \dot{\mathbf{x}}^t_1 \\ \dot{\mathbf{x}}^t_2 \end{bmatrix}}_{\dot{\mathbf{q}}}$$ 

which defines the **generalized velocities** as the stacked *9d* vector of per-vertex velocities. 

## Deformation Gradient 

The final necessary piece of the kinematic puzzle is the measure of deformation. You might be tempted to just compute $F = \frac{\partial \mathbf{x}^t}{\partial \mathbf{Y}}$ but this is going to get you in trouble. The dimensions of this matrix ($F \in \mathcal{R}^{3\times 2}$) make evaluating material models difficult since such models are *designed* to work for volumetric (re: square) deformation matrices. 

There are lots of ways to handle this problem in [literature](https://animation.rwth-aachen.de/media/papers/2013-CAG-AdaptiveCloth.pdf) and in this assignment we will rely on one which is both simple and effective. 

First let's remind ourselves that the functions which define barycentric coordinates require us to solve the linear system

$$\begin{bmatrix}\left(\mathbf{Y}_1- \mathbf{Y}_0\right) & \left(\mathbf{Y}_2- \mathbf{Y}_0\right)\end{bmatrix}\begin{bmatrix}\phi_1\left(\mathbf{Y}\right)\\\phi_2\left(\mathbf{Y}\right)\end{bmatrix} = \mathbf{Y} - \mathbf{Y}_0$$

To "square" our deformation gradient, we are going to "lift" the undeformed space of the cloth to 3d. **NOTE:** we are still going to use 2D triangles but now the undeformed vertex positions of those triangles will be given in 3d. Let's call the 3d undeformed vertex positions of our triangle mesh $\mathbf{X}_i$. So now, given any point in this weird 3d undeformed space, the second and third barycentric coordinates are given by 

$$\underbrace{\begin{bmatrix}\left(\mathbf{X}_1- \mathbf{X}_0\right) & \left(\mathbf{X}_2- \mathbf{X}_0\right)\end{bmatrix}}_{T \in \mathcal{R}^{3 \times 2}}\begin{bmatrix}\phi_1\left(\mathbf{X}\right)\\\phi_2\left(\mathbf{X}\right)\end{bmatrix} = \mathbf{X} - \mathbf{X}_0$$.

Ok, we've made everything worse. Now we can't even directly invert the right-hand side. But this is one of those cases wherein things had to get [worse before they get better](https://www.youtube.com/watch?v=uDIgS-Soo9Q). However, we can solve this system in a [least-squares](https://en.wikipedia.org/wiki/Least_squares) sense which gives us

$$\begin{bmatrix}\phi_1\left(\mathbf{X}\right)\\\phi_2\left(\mathbf{X}\right)\end{bmatrix} = \left(T^{T}T\right)^{-1}T^T\left(\mathbf{X} - \mathbf{X}_0\right)$$

which, when coupled with the fact that $\phi_0 = 1-\phi_1-\phi_2$ gives us everything we need. 

This might seem like an esoteric, algebraic solution, but geometrically it is doing something really quite reasonable. Any 3d point $\mathbf{X}$, which is on a triangle yields the same barycentric coordinates as the 2d solution. That's a good feature to have since we always, always, always [only calculate the world space position of the cloth at points on the cloth](https://en.wikipedia.org/wiki/Truism). For 3d points $\mathbf{X}$ that are off the cloth, this formulation projects them orthogonally (along the triangle normal) onto the cloth and returns the value for this projected cloth point. Because of this projection, the deformation of the cloth in the normal direction is zero, which makes sense, the discrete cloth has no thickness in this direction and cannot deform. However, because $\mathbf{X}$ is now 3d, the derivative $\frac{\partial \phi_i}{\partial \mathbf{X}}$ becomes $3 \times 3$ which means $F$ also becomes a $3\times 3$. This is much easier to deal with down the road. 

## Kinetic Energy

Armed with the generalized velocities, the formula for the per-triangle kinetic energy is eerily similar to that of assignment 3. It's an integral of the local kinetic energy over the entire triangle

$$ T_{triangle} = \dot{\mathbf{q}}\underbrace{\left(\int_{\mbox{triangle}}\rho N\left(\mathbf{X}\right)^T N\left(\mathbf{X}\right) dV\right)}_{M_e}\dot{\mathbf{q}} $$ 

and can be compute analytically using a symbolic math package. The per-element mass matrices for the every cloth triangle can then be *assembled* into the mass matrix for the entire mesh. 

## Potential Energy

For this assignment we will use a different type of material model to describe the elastic behaviour of the cloth. This is motivated by the fact that cloth is typically very resistant to stretching. For these materials, a linear stress-strain relationship is often desirable. Unfortunately, cloth triangles also rotate a lot (every time they fold-over for instance). Rotations are **NOT** linear and so a purely linear relationship will suffer from severe artifacts. To avoid this we will build a material model that only measures the in plane deformation of the cloth via its *principal stretches*. 

### Principal Stretches

Recall that in the previous assignment we used the right Cauchy strain tensor ($F^T F$) to measure deformation and the rationale for using this was that it measures the squared deformed length of an arbitrary, infinitesimal line of material, $\mathbf{dX}$. In other words, $|\mathbf{dx}|^2 = \mathbf{dX}^T \left(F^T F\right)\mathbf{dX}$.  Because $F$ is symmetric and positive definite, we can perform an [eigendecomposition](https://en.wikipedia.org/wiki/Eigendecomposition_of_a_matrix) such that $F^T F = U \Lambda U^T$ where $V$ is the orthogonal matrix of eigenvectors and $\Lambda$ is the diagonal matrix of eigenvalues. This means we can think of this squared length as $|\mathbf{dx}|^2 = \hat{\mathbf{dX}}^T \Lambda \hat{\mathbf{dX}}$ where $\hat{\mathbf{dX}}=U^T\mathbf{dX}$. In other words, if we transform $\mathbf{dX}$ just right, its deformation is completely characterized by $\Lambda$. 

$\Lambda$ are the eigenvalues of $F^T F$ and also the squared [*singular values*](https://en.wikipedia.org/wiki/Singular_value_decomposition) of $F$. We call these singular values of $F$ the [principal stretches](http://www.femdefo.org). They measure deformation independently of the orientation (or rotation/reflection) of the finite element. These rigid motions are encapsulated by $U$ which we will ignore during the construction of our material model. 

### Linear Elasticity without the Pesky Rotations

Now we can formulate a linear elastic model using the principal stretches which "filters out" any rotational components. Much like the Neohookean material model, this model will have one energy term which measures deformation and one energy term that tries to preserve volume (well area in the case of cloth). We already know we can measure deformation using the principal stretches. We also know that the determinant of $F$ measures the change in volume of a 3D object. In the volumetric case this determinant is just the product of the principal stretches. But first, we need to understand some specifics about the deformation gradient of cloth.

Recall that our deformation gradient for a cloth triangle is computed using a least squares projection. This is nice because we still have a $3 \times 3$ matrix on which to perform singular value decomposition on (and later ... (*shudder*) take the derivative of). It's problematic because this deformation gradient has a nullspace normal to the triangle (all points normal to the triangle are mapped to the same barycentric coordinate).  This means that, forever and always, **one of the singular values of $F$ will be zero**. Now if we assume that our triangle is not deformed to be inside out or squished to a line or a point, then this zero singular value will always be the last singular value returned by any reasonable SVD code (Eigen is pretty reasonable). Thus rather than use all three principal values in our material model, we will only use the first 2. This gives us the following material model 

$$\psi\left(s_0, s_1\right) = \mu\sum_{i=0}^1\left(s_i-1\right)^2 + \frac{\lambda}{2}\left(s_0 +  s_1 -2\right)^2 $$

where $\lambda$ and $\mu$ are the material properties for the cloth. The first term in this model attempts to keep $s_0$ and $s_1$ close to one (limiting deformation) while the second term is attempting to preserve the area of the deformed triangle (it's a linearization of the determinant). This model is called  **co-rotational linear elasticity** because it is linear in the principal stretches but rotates *with* each finite element. When we use energy models to measure the in-plane stretching of the cloth (or membrane), we often refer to them as membrane energies.  

### The Gradient of Principal Stretch Models 

The strain energy density for principal stretch models, like the one above, are relatively easy to implement and understand. This is a big reason we like them in graphics. We'll also see that the gradient of this model (needed for force computation) is also pretty easy to compute.

Really, the derivative we need to understand how to compute is $frac{\partial \psi}{\partial F}$. Once we have this we can use $\frac{\partial F}{\partial \mathbf{q}}$ to compute the gradient wrt to the generalized coordinates.  Conveniently, we have the following for principal stretch models.

$$\frac{\partial \psi}{\partial F} = U\underbrace{\begin{bmatrix}\frac{\partial \psi}{\partial s_0} & 0 & 0 \\ 0 & \frac{\partial \psi}{\partial s_1} & 0 \\ 0 & 0 & \frac{\partial \psi}{\partial s_2}\end{bmatrix}}_{dS}V^T $$ 

where $F = USV^T$ is the singular value decomposition.

### The Hessian of Principal Stretch Models

Unfortunately, the gradient of the principal stretch energy is not enough. That's because our favourite implicit integrators require second order information to provide the stability and performance we crave in computer graphics. This is where things get messy. The good news is that, if we can just compute $\frac{\partial \psi}{\partial F \partial F}$ then we can use $\frac{\partial F}{\partial \mathbf{q}}$  to compute our Hessian wrt to the generalized coordinates (this follows from the linearity of the FEM formulation wrt to the generalized coordinates).  This formula is going to get ugly so, in an attempt to make it somewhat clear, we are going to consider derivatives wrt to single entries of $F$, denoted $F_{ij}$. In this context we are trying to compute

$$\frac{\partial  }{\partial F_{ij}}\frac{\partial \psi}{\partial F}  = \frac{\partial U}{\partial F_{ij}}dS V^T + U\mbox{diag}\left(\mathbf{ds}_{ij}\right)V^T + UdS \frac{\partial V}{\partial F_{ij}}^T$$

Here $\mbox{diag}\left(\right)$ takes a $3\times 1$ vector as input and converts it into a diagonal matrix, with the entries of the matrix on the diagonal. In our case, we define $\mathbf{ds}$ as 

$$ \mathbf{ds}_{ij} = \begin{bmatrix}\frac{\partial^2 \psi}{\partial s_0^2} & \frac{\partial^2 \psi}{\partial s_0\partial s_1} & \frac{\partial^2 \psi}{\partial s_0\partial s_2} \\ \frac{\partial^2 \psi}{\partial s_0\partial s_1} & \frac{\partial^2 \psi}{\partial s_1^2} & \frac{\partial^2 \psi}{\partial s_1\partial s_2} \\ \frac{\partial^2 \psi}{\partial s_0\partial s_2} & \frac{\partial^2 \psi}{\partial s_1\partial s_12} & \frac{\partial^2 \psi}{\partial s_2^2}\end{bmatrix}\begin{bmatrix} \frac{\partial s_0}{\partial F_{ij}} \\ \frac{\partial s_1}{\partial F_{ij}} \\ \frac{\partial s_2}{\partial F_{ij}}\end{bmatrix}$$

The first thing to keep in mind when looking at this formula is **DON'T PANIC**. It's a straight forward application of the chain rule, just a little nastier than usual. The second thing to keep in mind is that all derivatives wrt to $s_2$ are zero (it never changes, it's always 0). The final thing to keep in mind is that **I am giving you the code to compute SVD derivatives in dsvd.h/dsvd.cpp**.  

If we define the svd of a matrix as $F = USV^T$, this code returns $\frac{\partial U}{\partial F}\in\mathcal{R}^{3\times 3 \times 3 \times 3}$, $\frac{\partial V}{\partial F}\in\mathcal{R}^{3\times 3 \times 3 \times 3}$ and $\frac{\partial S}{\partial F}\in\mathcal{R}^{3\times 3 \times 3}$. Yes this code returns 3 and four dimensional tensors storing this quantities, yes I said never to do this in class, consider this the exception that makes the rule.  The latter two indices on each tensor are the $i$ and $j$ indices used in the formula above. 

The hardest part of implementing this gradient correctly is handling the SVD terms. These gradients have a different form based on whether your $F$ matrix is square or rectangular. This is one big reason that the $3 \times 3$ deformation gradient we use in this assignment is desirable. It allows one to use the same singular value decomposition code for volumetric and cloth models. 

## Collision Detection with Spheres

To make this assignment a little more visually interesting, you will implement simple collision detection and resolution with an analytical sphere. **Collision Detection** is the process of detecting contact between two or more objects in the scene and **Collision Resolution** is the process of modifying the motion of the object in response to these detected collisions.  

For this assignment we will implement per-vertex collision detection with the sphere. This is as simple as detecting if the distance from the center of the sphere to any vertex in your mesh is less than the radius of the sphere. If you detect such a collision, you need to store an **index** to the colliding vertex, along with the outward facing **contact normal**($\mathbf{n}$). In this case, the outward facing contact normal is the sphere normal at the point of contact. 

## Collision Resolution 

The minimal goal of any collision resolution algorithm is to prevent collisions from getting worse locally. To do this we will implement a simple *velocity filter* approach. Velocity filters are so named because the "filter out" components of an objects velocity that will increase the severity of a collision. Given a vertex that is colliding with our sphere, the only way that the collision can get worse locally is if that vertex moves *into* the sphere. One way we can check if this is happening is to compute the projection of the vertex velocity onto the outward facing contact normal ($\mathbf{n}^T\dot{\mathbf{q}}_i$, $i$ selects the $i^th$ contacting vertex). If this number is $>0$ we are OK, the vertex is moving away from the sphere. If this number is $<0$ we better do something. 

The thing we will do is project out, or filter out, the component of the velocity moving in the negative, normal direction like so:

$$\dot{\mathbf{q}}^{\mbox{filtered}}_i = \dot{\mathbf{q}}_i - \mathbf{n}\mathbf{n}^T\dot{\mathbf{q}}_i$$

This "fixes" the collision. This approach to collision resolution is fast but for more complicated scenes it is fraught with peril. For instance it doesn't take into account how it is deforming the simulated object which can cause big headaches when objects are stiff or rigid. We'll see a cleaner mathematical approach to content in the final assignment of the course.

## Assignment Implementation

### Implementation Notes

In this assignment you will reuse your linearly implicit integrator to time step the dynamic system. Also, we will eschew implementing the strain energy density function, quadrature rule and potential energies separately. Instead functions for potential energy and its derivatives should directly compute the integrated values for a triangular element.

### dphi_cloth_triangle_dX.cpp

Piecewise constant gradient matrix for linear shape functions. Row $i$ of the returned matrix contains the gradient of the $i^{th}$ shape function.

### T_cloth.cpp

The kinetic energy of the whole cost mesh.

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


