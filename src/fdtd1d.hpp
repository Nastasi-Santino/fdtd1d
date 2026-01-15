#pragma once

#include <vector>
#include <cstddef>   // std::size_t

// --- Boundary types ---
enum class BoundaryType
{
    Reflective,  // PEC-like: full reflection (simple debug boundary)
    Mur1         // 1st-order Mur absorbing boundary (open-space approximation)
};

// --- Simulation/grid configuration (inputs) ---
struct GridConfig
{
    std::size_t N;     // Number of E nodes (E has size N, H has size N-1). Must be >= 3
    double dx;         // Spatial step [m]. Must be > 0
    double S;          // Courant number S = (c*dt)/dx. In 1D: 0 < S <= 1 for stability
    double eps;        // Permittivity [F/m]. Must be > 0 (use eps0 for vacuum)
    double mu;         // Permeability [H/m]. Must be > 0 (use mu0 for vacuum)
    BoundaryType boundary;

    // Optional: you can add a small validation helper later (implemented in .cpp)
    // void validate() const;
};

// --- physics constants ---
namespace em
{
    constexpr double eps0 = 8.854187817e-12;
    constexpr double mu0  = 1.25663706212e-6;
}

// --- 1D FDTD solver (fields + time stepping) ---
class FDTD1D
{
public:
    // Constructs solver, allocates fields based on cfg.N
    explicit FDTD1D(const GridConfig& cfg);

    // Advance simulation by one time step
    void step();

    // Read-only access to fields (avoid accidental modification)
    const std::vector<double>& E() const noexcept { return E_; } // size N
    const std::vector<double>& H() const noexcept { return H_; } // size N-1

    // Expose config read-only
    const GridConfig& config() const noexcept { return cfg_; }

private:
    // Break the algorithm into clear pieces
    void updateH_();
    void updateE_();
    void applyBoundary_();
    void applySource_();

private:
    GridConfig cfg_;

    // Field arrays:
    // E[i]   for i = 0..N-1
    // H[i]   for i = 0..N-2 (located at i+1/2 in space)
    std::vector<double> E_;
    std::vector<double> H_;

    // Derived parameters (computed in constructor):
    double dt_{};   // time step [s]
    double c_{};    // wave speed in medium [m/s] = 1/sqrt(eps*mu)
    double ce_{};   // dt/(eps*dx)
    double ch_{};   // dt/(mu*dx)

    // State needed for Mur 1st-order ABC (store previous boundary samples)
    double E_left_prev_{};   // previous-time E at left boundary
    double E_left_neighbor_prev_{};
    double E_right_prev_{};  // previous-time E at right boundary
    double E_right_neighbor_prev_{};
    double mur_k_{};         // (c*dt - dx)/(c*dt + dx)

    std::size_t n_ = 0;
};
