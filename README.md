[![](https://github.com/dilevin/CSC2549-a4-cloth-simulation/workflows/Build-CSC2549-Assignment-Four/badge.svg)](https://github.com/dilevin/CSC2549-a4-cloth-simulation/actions)

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

<p align="center"><img src="/tex/1767c7f7922c97563d66b8446ba75840.svg?invert_in_darkmode&sanitize=true" align=middle width=147.58684215pt height=47.35857885pt/></p> 

where <img src="/tex/c83e439282bef5aadf55a91521506c1a.svg?invert_in_darkmode&sanitize=true" align=middle width=14.44544309999999pt height=22.831056599999986pt/> are the [barycentric coordinates](https://en.wikipedia.org/wiki/Barycentric_coordinate_system) for a 2D triangle and <img src="/tex/58c2313bc1ea66cc4d86d27f83c1941d.svg?invert_in_darkmode&sanitize=true" align=middle width=55.34000504999999pt height=26.76175259999998pt/> is the 2d coordinate in the undeformed space. 

Our goal is to be able to estimate the 3d world space position of any part of this cloth triangle (for any value of <img src="/tex/319d907db67f3000780e9b2d1a2816d9.svg?invert_in_darkmode&sanitize=true" align=middle width=14.764759349999988pt height=22.55708729999998pt/> in the triangle). Using our FEM basis, this becomes

<p align="center"><img src="/tex/e380f5951fb9f765922182550d7fa939.svg?invert_in_darkmode&sanitize=true" align=middle width=155.77825395pt height=47.35857885pt/></p>

where <img src="/tex/07fa8f51f4cd0e31f63743c0638a3388.svg?invert_in_darkmode&sanitize=true" align=middle width=56.34005519999999pt height=26.76175259999998pt/> are the 3d, per-vertex positions of the cloth mesh at time <img src="/tex/4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode&sanitize=true" align=middle width=5.936097749999991pt height=20.221802699999984pt/>. This gives a mapping from the 2d space of the undeformed cloth to the 3d world space.  As usual, we choose the **generalized coordinates** (<img src="/tex/84a62034abeb6bfffada277b45a096c7.svg?invert_in_darkmode&sanitize=true" align=middle width=50.55236834999999pt height=26.76175259999998pt/>) to be the stacked vector of vertex positions, which lets us rewrite the above expression as 

<p align="center"><img src="/tex/7f9c21b8882b50ee8b7a2ba3d6be1d09.svg?invert_in_darkmode&sanitize=true" align=middle width=332.70013754999997pt height=83.11467284999999pt/></p>
 
The velocity of the cloth, at any point <img src="/tex/319d907db67f3000780e9b2d1a2816d9.svg?invert_in_darkmode&sanitize=true" align=middle width=14.764759349999988pt height=22.55708729999998pt/> is then given by the total time derivative: 

<p align="center"><img src="/tex/d5b9b96cdc2cdabfd94eaaedae1a759d.svg?invert_in_darkmode&sanitize=true" align=middle width=332.96270534999996pt height=83.89091535pt/></p> 

which defines the **generalized velocities** as the stacked *9d* vector of per-vertex velocities. 

## Deformation Gradient 

The final necessary piece of the kinematic puzzle is the measure of deformation. You might be tempted to just compute <img src="/tex/247ed86bbeef7615018d6bc51947544a.svg?invert_in_darkmode&sanitize=true" align=middle width=57.78279374999998pt height=32.976995699999996pt/> but this is going to get you in trouble. The dimensions of this matrix (<img src="/tex/88787d1576b44fdadc099c2c2d5cd9f6.svg?invert_in_darkmode&sanitize=true" align=middle width=70.25574269999998pt height=26.76175259999998pt/>) make evaluating material models difficult since such models are *designed* to work for volumetric (re: square) deformation matrices. 

There are lots of ways to handle this problem in [literature](https://animation.rwth-aachen.de/media/papers/2013-CAG-AdaptiveCloth.pdf) and in this assignment we will rely on one which is both simple and effective. 

First let's remind ourselves that the functions which define barycentric coordinates require us to solve the linear system

<p align="center"><img src="/tex/ab23897fbb310593a19e9659aff87d65.svg?invert_in_darkmode&sanitize=true" align=middle width=327.7252077pt height=39.452455349999994pt/></p>

To "square" our deformation gradient, we are going to "lift" the undeformed space of the cloth to 3d. **NOTE:** we are still going to use 2D triangles but now the undeformed vertex positions of those triangles will be given in 3d. Let's call the 3d undeformed vertex positions of our triangle mesh <img src="/tex/9b01119ffd35fe6d8a8795a24fc11616.svg?invert_in_darkmode&sanitize=true" align=middle width=18.943064249999992pt height=22.55708729999998pt/>. So now, given any point in this weird 3d undeformed space, the second and third barycentric coordinates are given by 

<p align="center"><img src="/tex/5d9ea342efc9120401f89cbe3f833ad1.svg?invert_in_darkmode&sanitize=true" align=middle width=326.7800316pt height=53.80252679999999pt/></p>.

Ok, we've made everything worse. Now we can't even directly invert the right-hand side. But this is one of those cases wherein things had to get [worse before they get better](https://www.youtube.com/watch?v=uDIgS-Soo9Q). However, we can solve this system in a [least-squares](https://en.wikipedia.org/wiki/Least_squares) sense which gives us

<p align="center"><img src="/tex/79361f938b80de1c32bb7d7724ce3590.svg?invert_in_darkmode&sanitize=true" align=middle width=249.66638894999997pt height=39.452455349999994pt/></p>

which, when coupled with the fact that <img src="/tex/358bf7b214f6e3ed24776b7ec9a31866.svg?invert_in_darkmode&sanitize=true" align=middle width=121.00431914999999pt height=22.831056599999986pt/> gives us everything we need. 

This might seem like an esoteric, algebraic solution, but geometrically it is doing something really quite reasonable. Any 3d point <img src="/tex/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode&sanitize=true" align=middle width=14.29216634999999pt height=22.55708729999998pt/>, which is on a triangle yields the same barycentric coordinates as the 2d solution. That's a good feature to have since we always, always, always [only calculate the world space position of the cloth at points on the cloth](https://en.wikipedia.org/wiki/Truism). For 3d points <img src="/tex/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode&sanitize=true" align=middle width=14.29216634999999pt height=22.55708729999998pt/> that are off the cloth, this formulation projects them orthogonally (along the triangle normal) onto the cloth and returns the value for this projected cloth point. Because of this projection, the deformation of the cloth in the normal direction is zero, which makes sense, the discrete cloth has no thickness in this direction and cannot deform. However, because <img src="/tex/d05b996d2c08252f77613c25205a0f04.svg?invert_in_darkmode&sanitize=true" align=middle width=14.29216634999999pt height=22.55708729999998pt/> is now 3d, the derivative <img src="/tex/3ed8894ad18b8f13e6b16019cd987ca2.svg?invert_in_darkmode&sanitize=true" align=middle width=20.839257449999995pt height=30.648287999999997pt/> becomes <img src="/tex/9f2b6b0a7f3d99fd3f396a1515926eb3.svg?invert_in_darkmode&sanitize=true" align=middle width=36.52961069999999pt height=21.18721440000001pt/> which means <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/> also becomes a <img src="/tex/46e42d6ebfb1f8b50fe3a47153d01cd2.svg?invert_in_darkmode&sanitize=true" align=middle width=36.52961069999999pt height=21.18721440000001pt/>. This is much easier to deal with down the road. 

## Kinetic Energy

Armed with the generalized velocities, the formula for the per-triangle kinetic energy is eerily similar to that of assignment 3. It's an integral of the local kinetic energy over the entire triangle

<p align="center"><img src="/tex/e825c2910f10d1a0ef1babeb28698a51.svg?invert_in_darkmode&sanitize=true" align=middle width=335.72428064999997pt height=73.3976958pt/></p> 

and can be compute analytically using a symbolic math package. The per-element mass matrices for the every cloth triangle can then be *assembled* into the mass matrix for the entire mesh. 

## Potential Energy

For this assignment we will use a different type of material model to describe the elastic behaviour of the cloth. This is motivated by the fact that cloth is typically very resistant to stretching. For these materials, a linear stress-strain relationship is often desirable. Unfortunately, cloth triangles also rotate a lot (every time they fold-over for instance). Rotations are **NOT** linear and so a purely linear relationship will suffer from severe artifacts. To avoid this we will build a material model that only measures the in plane deformation of the cloth via its *principal stretches*. 

### Principal Stretches

Recall that in the previous assignment we used the right Cauchy strain tensor (<img src="/tex/07640d208ac0851eb169c73b736e6bb1.svg?invert_in_darkmode&sanitize=true" align=middle width=36.06344114999999pt height=27.6567522pt/>) to measure deformation and the rationale for using this was that it measures the squared deformed length of an arbitrary, infinitesimal line of material, <img src="/tex/fe4e62694c7a379a0b31c6aa14046dd0.svg?invert_in_darkmode&sanitize=true" align=middle width=24.79439324999999pt height=22.831056599999986pt/>. In other words, <img src="/tex/3b6459009b4b915041dfd6286e7e0c2b.svg?invert_in_darkmode&sanitize=true" align=middle width=175.45954964999999pt height=30.429319799999984pt/>.  Because <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/> is symmetric and positive definite, we can perform an [eigendecomposition](https://en.wikipedia.org/wiki/Eigendecomposition_of_a_matrix) such that <img src="/tex/357a3518901055dabec2b6272f82e942.svg?invert_in_darkmode&sanitize=true" align=middle width=105.41439149999998pt height=27.6567522pt/> where <img src="/tex/a9a3a4a202d80326bda413b5562d5cd1.svg?invert_in_darkmode&sanitize=true" align=middle width=13.242037049999992pt height=22.465723500000017pt/> is the orthogonal matrix of eigenvectors and <img src="/tex/b23332f99af850a48831f80dbf681ed6.svg?invert_in_darkmode&sanitize=true" align=middle width=11.41554479999999pt height=22.465723500000017pt/> is the diagonal matrix of eigenvalues. This means we can think of this squared length as <img src="/tex/1e910408a8c30e48164f77629c847ebe.svg?invert_in_darkmode&sanitize=true" align=middle width=130.26379905pt height=39.1051584pt/> where <img src="/tex/5cae4b0b649a9b8d2fe83816e5ea8db6.svg?invert_in_darkmode&sanitize=true" align=middle width=94.87794195pt height=31.50689519999998pt/>. In other words, if we transform <img src="/tex/fe4e62694c7a379a0b31c6aa14046dd0.svg?invert_in_darkmode&sanitize=true" align=middle width=24.79439324999999pt height=22.831056599999986pt/> just right, its deformation is completely characterized by <img src="/tex/b23332f99af850a48831f80dbf681ed6.svg?invert_in_darkmode&sanitize=true" align=middle width=11.41554479999999pt height=22.465723500000017pt/>. 

<img src="/tex/b23332f99af850a48831f80dbf681ed6.svg?invert_in_darkmode&sanitize=true" align=middle width=11.41554479999999pt height=22.465723500000017pt/> are the eigenvalues of <img src="/tex/07640d208ac0851eb169c73b736e6bb1.svg?invert_in_darkmode&sanitize=true" align=middle width=36.06344114999999pt height=27.6567522pt/> and also the squared [*singular values*](https://en.wikipedia.org/wiki/Singular_value_decomposition) of <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/>. We call these singular values of <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/> the [principal stretches](http://www.femdefo.org). They measure deformation independently of the orientation (or rotation/reflection) of the finite element. 

### Linear Elasticity without the Pesky Rotations

Now we can formulate a linear elastic model using the principal stretches which "filters out" any rotational components. Much like the Neohookean material model, this model will have one energy term which measures deformation and one energy term that tries to preserve volume (well area in the case of cloth). We already know we can measure deformation using the principal stretches. We also know that the determinant of <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/> measures the change in volume of a 3D object. In the volumetric case this determinant is just the product of the principal stretches. But first, we need to understand some specifics about the deformation gradient of cloth.

Recall that our deformation gradient for a cloth triangle is computed using a least squares projection. This is nice because we still have a <img src="/tex/9f2b6b0a7f3d99fd3f396a1515926eb3.svg?invert_in_darkmode&sanitize=true" align=middle width=36.52961069999999pt height=21.18721440000001pt/> matrix on which to perform singular value decomposition on (and later ... (*shudder*) take the derivative of). It's problematic because this deformation gradient has a nullspace normal to the triangle (all points normal to the triangle are mapped to the same barycentric coordinate).  This means that, forever and always, **one of the singular values of <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/> will be zero**. Now if we assume that our triangle is not deformed to be inside out or squished to a line or a point, then this zero singular value will always be the last singular value returned by any reasonable SVD code (Eigen is pretty reasonable). Thus rather than use all three principal values in our material model, we will only use the first 2. This gives us the following material model 

<p align="center"><img src="/tex/d4f90ae5560ad92e3d914767e406044c.svg?invert_in_darkmode&sanitize=true" align=middle width=321.24775485pt height=47.35857885pt/></p>

where <img src="/tex/fd8be73b54f5436a5cd2e73ba9b6bfa9.svg?invert_in_darkmode&sanitize=true" align=middle width=9.58908224999999pt height=22.831056599999986pt/> and <img src="/tex/07617f9d8fe48b4a7b3f523d6730eef0.svg?invert_in_darkmode&sanitize=true" align=middle width=9.90492359999999pt height=14.15524440000002pt/> are the material properties for the cloth. The first term in this model attempts to keep <img src="/tex/ac3148a5746b81298cb0c456b661f197.svg?invert_in_darkmode&sanitize=true" align=middle width=14.25802619999999pt height=14.15524440000002pt/> and <img src="/tex/286f7d4815c0996530bda7973b1ec5ea.svg?invert_in_darkmode&sanitize=true" align=middle width=14.25802619999999pt height=14.15524440000002pt/> close to one (limiting deformation) while the second term is attempting to preserve the area of the deformed triangle (it's a linearization of the determinant). This model is called  **co-rotational linear elasticity** because it is linear in the principal stretches but rotates *with* each finite element. When we use energy models to measure the in-plane stretching of the cloth (or membrane), we often refer to them as membrane energies.  

### The Gradient of Principal Stretch Models 

The strain energy density for principal stretch models, like the one above, are relatively easy to implement and understand. This is a big reason we like them in graphics. We'll also see that the gradient of this model (needed for force computation) is also pretty easy to compute.

Really, the derivative we need to understand how to compute is <img src="/tex/9a05c8a8b310cb0d9f34636450b3d442.svg?invert_in_darkmode&sanitize=true" align=middle width=17.83399365pt height=30.648287999999997pt/>. Once we have this we can use <img src="/tex/b38dda0a0c1ab15d9dae0b9b9e8aa1dc.svg?invert_in_darkmode&sanitize=true" align=middle width=17.83399365pt height=28.92634470000001pt/> to compute the gradient wrt to the generalized coordinates.  Conveniently, we have the following for principal stretch models.

<p align="center"><img src="/tex/57d6ce76b81f64d9de86d723aaf06fcf.svg?invert_in_darkmode&sanitize=true" align=middle width=215.32883955pt height=91.60788615pt/></p> 

where <img src="/tex/b93e9b71beaf724ff1d0f85841e9ae61.svg?invert_in_darkmode&sanitize=true" align=middle width=81.59062395pt height=27.6567522pt/> is the singular value decomposition.

### The Hessian of Principal Stretch Models

Unfortunately, the gradient of the principal stretch energy is not enough. That's because our favourite implicit integrators require second order information to provide the stability and performance we crave in computer graphics. This is where things get messy. The good news is that, if we can just compute <img src="/tex/5c09159163d9a8fe55c1a3016a3ad75a.svg?invert_in_darkmode&sanitize=true" align=middle width=35.667965849999995pt height=30.648287999999997pt/> then we can use <img src="/tex/b38dda0a0c1ab15d9dae0b9b9e8aa1dc.svg?invert_in_darkmode&sanitize=true" align=middle width=17.83399365pt height=28.92634470000001pt/>  to compute our Hessian wrt to the generalized coordinates (this follows from the linearity of the FEM formulation wrt to the generalized coordinates).  This formula is going to get ugly so, in an attempt to make it somewhat clear, we are going to consider derivatives wrt to single entries of <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/>, denoted <img src="/tex/a8c1f0c39cf12de6d64430cd9bc86046.svg?invert_in_darkmode&sanitize=true" align=middle width=21.32620544999999pt height=22.465723500000017pt/>. In this context we are trying to compute

<p align="center"><img src="/tex/52999a1482045a0142d3cbd60a381afe.svg?invert_in_darkmode&sanitize=true" align=middle width=393.63332085pt height=42.3143919pt/></p>

Here <img src="/tex/01f349799d08738b3ed7187a8adfa8b7.svg?invert_in_darkmode&sanitize=true" align=middle width=45.662167649999994pt height=24.65753399999998pt/> takes a <img src="/tex/6018bf12266e0674a19d0a622989ad88.svg?invert_in_darkmode&sanitize=true" align=middle width=36.52961069999999pt height=21.18721440000001pt/> vector as input and converts it into a diagonal matrix, with the entries of the matrix on the diagonal. In our case, we define <img src="/tex/0a36368a7b551a7c6aa055de5009bea3.svg?invert_in_darkmode&sanitize=true" align=middle width=17.958806249999988pt height=22.831056599999986pt/> as 

<p align="center"><img src="/tex/57354dd3723345bfc2d7b2a89dd465a7.svg?invert_in_darkmode&sanitize=true" align=middle width=301.91657265pt height=81.88967819999999pt/></p>

The first thing to keep in mind when looking at this formula is **DON'T PANIC**. It's a straight forward application of the chain rule, just a little nastier than usual. The second thing to keep in mind is that all derivatives wrt to <img src="/tex/97c7f491f7ac1623c0a86b1fb656029b.svg?invert_in_darkmode&sanitize=true" align=middle width=14.25802619999999pt height=14.15524440000002pt/> are zero (it never changes, it's always 0). The final thing to keep in mind is that **I am giving you the code to compute SVD derivatives in dsvd.h/dsvd.cpp**.  

If we define the svd of a matrix as <img src="/tex/b93e9b71beaf724ff1d0f85841e9ae61.svg?invert_in_darkmode&sanitize=true" align=middle width=81.59062395pt height=27.6567522pt/>, this code returns <img src="/tex/bfdc1036a225b1cba16ea97fa96d82de.svg?invert_in_darkmode&sanitize=true" align=middle width=110.94409754999998pt height=28.92634470000001pt/>, <img src="/tex/a0d1fd43ad34510636923359f83becfa.svg?invert_in_darkmode&sanitize=true" align=middle width=111.34893495pt height=28.92634470000001pt/> and <img src="/tex/c019432b974b538f20a9d3cb38176a2d.svg?invert_in_darkmode&sanitize=true" align=middle width=94.03493055pt height=28.92634470000001pt/>. Yes this code returns 3 and four dimensional tensors storing this quantities, yes I said never to do this in class, consider this the exception that makes the rule.  The latter two indices on each tensor are the <img src="/tex/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode&sanitize=true" align=middle width=5.663225699999989pt height=21.68300969999999pt/> and <img src="/tex/36b5afebdba34564d884d347484ac0c7.svg?invert_in_darkmode&sanitize=true" align=middle width=7.710416999999989pt height=21.68300969999999pt/> indices used in the formula above. 

The hardest part of implementing this gradient correctly is handling the SVD terms. These gradients have a different form based on whether your <img src="/tex/b8bc815b5e9d5177af01fd4d3d3c2f10.svg?invert_in_darkmode&sanitize=true" align=middle width=12.85392569999999pt height=22.465723500000017pt/> matrix is square or rectangular. This is one big reason that the <img src="/tex/9f2b6b0a7f3d99fd3f396a1515926eb3.svg?invert_in_darkmode&sanitize=true" align=middle width=36.52961069999999pt height=21.18721440000001pt/> deformation gradient we use in this assignment is desirable. It allows one to use the same singular value decomposition code for volumetric and cloth models. 

## Collision Detection with Spheres

To make this assignment a little more visually interesting, you will implement simple collision detection and resolution with an analytical sphere. **Collision Detection** is the process of detecting contact between two or more objects in the scene and **Collision Resolution** is the process of modifying the motion of the object in response to these detected collisions.  

For this assignment we will implement per-vertex collision detection with the sphere. This is as simple as detecting if the distance from the center of the sphere to any vertex in your mesh is less than the radius of the sphere. If you detect such a collision, you need to store an **index** to the colliding vertex, along with the outward facing **contact normal**(<img src="/tex/b56595d2a30a0af329086562ca12d521.svg?invert_in_darkmode&sanitize=true" align=middle width=10.502226899999991pt height=14.611878600000017pt/>). In this case, the outward facing contact normal is the sphere normal at the point of contact. 

## Collision Resolution 

The minimal goal of any collision resolution algorithm is to prevent collisions from getting worse locally. To do this we will implement a simple *velocity filter* approach. Velocity filters are so named because the "filter out" components of an objects velocity that will increase the severity of a collision. Given a vertex that is colliding with our sphere, the only way that the collision can get worse locally is if that vertex moves *into* the sphere. One way we can check if this is happening is to compute the projection of the vertex velocity onto the outward facing contact normal (<img src="/tex/82f2067d05c8a81659bc2b949d23b64a.svg?invert_in_darkmode&sanitize=true" align=middle width=35.48583554999999pt height=27.6567522pt/>, <img src="/tex/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode&sanitize=true" align=middle width=5.663225699999989pt height=21.68300969999999pt/> selects the <img src="/tex/c36ef0ba4721f49285945f33a25e7a45.svg?invert_in_darkmode&sanitize=true" align=middle width=20.92202969999999pt height=26.085962100000025pt/> contacting vertex). If this number is <img src="/tex/ea4bbf715156e61bd05c0ec553601019.svg?invert_in_darkmode&sanitize=true" align=middle width=25.570741349999988pt height=21.18721440000001pt/> we are OK, the vertex is moving away from the sphere. If this number is <img src="/tex/1757afe2b054e59c6d5c465cf82bd885.svg?invert_in_darkmode&sanitize=true" align=middle width=25.570741349999988pt height=21.18721440000001pt/> we better do something. 

The thing we will do is project out, or filter out, the component of the velocity moving in the negative, normal direction like so:

<p align="center"><img src="/tex/870c98e8c4b120f06c7a15af3b02d6e7.svg?invert_in_darkmode&sanitize=true" align=middle width=164.51989455pt height=22.2666345pt/></p>

This "fixes" the collision. This approach to collision resolution is fast but for more complicated scenes it is fraught with peril. For instance it doesn't take into account how it is deforming the simulated object which can cause big headaches when objects are stiff or rigid. We'll see a cleaner mathematical approach to content in the final assignment of the course.

## Assignment Implementation

### Implementation Notes

In this assignment you will reuse your linearly implicit integrator to time step the dynamic system. Also, we will eschew implementing the strain energy density function, quadrature rule and potential energies separately. Instead functions for potential energy and its derivatives should directly compute the integrated values for a triangular element.

### dphi_cloth_triangle_dX.cpp

Piecewise constant gradient matrix for linear shape functions. Row <img src="/tex/77a3b857d53fb44e33b53e4c8b68351a.svg?invert_in_darkmode&sanitize=true" align=middle width=5.663225699999989pt height=21.68300969999999pt/> of the returned matrix contains the gradient of the <img src="/tex/3def24cf259215eefdd43e76525fb473.svg?invert_in_darkmode&sanitize=true" align=middle width=18.32504519999999pt height=27.91243950000002pt/> shape function.

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


