# 1D FDTD Electromagnetic Wave Simulator

## Overview
This project implements a **one-dimensional Finite-Difference Time-Domain (FDTD)** solver for electromagnetic wave propagation.  
The solver numerically integrates Maxwell’s equations in the time domain and allows visualization of both the **electric field E(x,t)** and the **magnetic field H(x,t)**.

The main goal of the project is educational and exploratory: to build a **clean, physically consistent, and extensible** FDTD code from scratch, following practices commonly used in computational electromagnetics.

---

## Physical Model

The simulator solves the 1D Maxwell curl equations in a homogeneous, isotropic, lossless medium:

\[
\frac{\partial E}{\partial t} = \frac{1}{\varepsilon} \frac{\partial H}{\partial x}, \qquad
\frac{\partial H}{\partial t} = \frac{1}{\mu} \frac{\partial E}{\partial x}
\]

using a **leapfrog FDTD scheme**:
- Electric field **E** sampled at integer spatial nodes  
- Magnetic field **H** sampled at staggered positions  
- Time staggering between E and H updates  

---

## Numerical Scheme

- Spatial discretization: uniform grid with spacing `dx`
- Time step:
  \[
  \Delta t = S \frac{\Delta x}{c}, \quad c = \frac{1}{\sqrt{\varepsilon \mu}}
  \]
- Courant number `S ≤ 1` ensures numerical stability

The update equations are implemented explicitly following the standard Yee scheme in one dimension.

---

## Boundary Conditions

The solver supports multiple boundary conditions:

### Reflective Boundary (PEC)
- Enforces `E = 0` at the domain boundaries  
- Produces total reflection of the incident wave  

### Mur First-Order Absorbing Boundary
- Implements **Mur’s first-order absorbing boundary condition**  
- Approximates an open domain  
- Significantly reduces boundary reflections  
- Requires storing field values from the previous time step  

---

## Source Excitation

A **soft Gaussian pulse source** is used to excite the domain:

\[
s(n) = A \exp\left[-\left(\frac{n - n_0}{\text{spread}}\right)^2\right]
\]

The source is injected additively into the electric field, avoiding hard discontinuities and providing broadband excitation suitable for transient analysis.

---

## Code Structure

```
fdtd1d/
├── src/
│   ├── main.cpp
│   ├── fdtd1d.cpp
│   └── fdtd1d.hpp
├── data/
├── scripts/
│   ├── plot_snapshot.py
│   └── animate_field.py
└── plots/
```

The `FDTD1D` class encapsulates grid configuration, field storage, time stepping, boundary handling, and source injection.  
Simulation results are written in CSV format for flexible post-processing.

---

## Visualization and Post-Processing

Two Python scripts are provided:

### plot_snapshot.py
- Loads a single CSV snapshot  
- Plots **E(x)** and optionally **H(x)**  
- Automatically locates the `data/` directory  
- Useful for validation and debugging  

### animate_field.py
- Animates **E(x,t)** and **H(x,t)**  
- Automatically detects CSV format and delimiters  
- Uses publication-style plotting  
- Supports export to GIF or MP4  

---

## Typical Simulation Parameters

- Grid points: `N = 400–800`
- Spatial step: `dx = 1e-3 m`
- Courant number: `S ≈ 0.99`
- Medium: free space (`ε₀`, `μ₀`)

These values provide stable simulations with smooth wave propagation and limited numerical dispersion.

---

## Build and Run (Windows / MSVC)

From a **Visual Studio x64 Native Tools Command Prompt**:

```bat
mkdir build
cl /std:c++20 /EHsc /Zi /W4 /Od src\main.cpp src\fdtd1d.cpp /Fe:build\fdtd.exe
build\fdtd.exe
```

The simulation generates CSV files in the `data/` directory.

---

## Possible Extensions

- Inhomogeneous or lossy materials  
- Sinusoidal steady-state sources  
- Frequency-domain analysis (FFT)  
- Higher-order absorbing boundaries (PML)  
- Extension to 2D or 3D FDTD  

---

## Motivation

This project was developed to deepen understanding of:
- Computational electromagnetics  
- Time-domain numerical methods  
- Stability criteria  
- Boundary conditions  

It is intended as a foundation for more advanced EM simulations, including antennas, waveguides, and microwave structures.

---

## Author
Electronic Engineering student with a strong interest in  
**electromagnetics, numerical simulation, and scientific computing**.
