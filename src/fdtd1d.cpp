#include "fdtd1d.hpp"
#include <stdexcept>
#include <cmath>

FDTD1D::FDTD1D(const GridConfig& cfg)
  : cfg_(cfg)
  , E_()
  , H_()
{

    if(cfg_.N < 3){
        throw std::invalid_argument("N must be greater than 2");
    }

    if(cfg_.dx <= 0){
        throw std::invalid_argument("dx must be greater than 0");
    }

    if(cfg_.eps <= 0){
        throw std::invalid_argument("Epsilon must be greater than 0");
    }

    if(cfg_.mu <= 0){
        throw std::invalid_argument("Mu must be greater than 0");
    }

    if(cfg_.S <= 0 || cfg_.S > 1){
        throw std::invalid_argument("S must satisfy 0 < S <= 1");
    }

    E_.assign(cfg_.N, 0.0);
    H_.assign(cfg_.N - 1, 0.0);

    c_ = 1.0 / std::sqrt(cfg_.eps * cfg_.mu);
    dt_ = cfg_.S * cfg_.dx / c_;
    ce_ = dt_ / (cfg_.eps * cfg_.dx);
    ch_ = dt_ / (cfg_.mu * cfg_.dx);
    if(cfg_.boundary == BoundaryType::Mur1){
        mur_k_ = (c_*dt_ - cfg_.dx) / (c_*dt_ + cfg_.dx);
        E_left_prev_ = 0.0;
        E_left_neighbor_prev_ = 0.0;
        E_right_neighbor_prev_ = 0.0;
        E_right_prev_ = 0.0;
    }

}

void FDTD1D::updateH_(){
    for (std::size_t i = 0; i < cfg_.N-1; i++){
        H_[i] += ch_ * (E_[i+1] - E_[i]);
    }
}

void FDTD1D::updateE_(){
    for (std::size_t i = 1; i < cfg_.N-1; i++){
        E_[i] += ce_ * (H_[i] - H_[i-1]);
    }
}

void FDTD1D::applyBoundary_(){

    double E0_old = 0.0;
    double E1_old = 0.0;
    double EN_old = 0.0;
    double ENm1_old = 0.0;

    switch (cfg_.boundary)
    {
    case BoundaryType::Reflective:
        E_.front() = 0;
        E_.back() = 0;
        break;
    
    case BoundaryType::Mur1:
        E0_old = E_.front();
        E1_old = E_[1];
        EN_old = E_.back();
        ENm1_old = E_[cfg_.N-2];

        E_.front() = E_left_neighbor_prev_ + mur_k_ * (E_[1] - E_left_prev_);
        E_.back() = E_right_neighbor_prev_ + mur_k_ * (E_[cfg_.N - 2] - E_right_prev_);

        E_left_prev_ = E0_old;
        E_left_neighbor_prev_ =E1_old;
        E_right_prev_ = EN_old;
        E_right_neighbor_prev_ = ENm1_old;
        break;
    
    default:
        throw std::logic_error("Unknown boundary type");
        break;
    }
}

void FDTD1D::applySource_()
{
    const std::size_t src = cfg_.N / 4;

    const double A = 1.0;
    const double n0 = 60.0;
    const double spread = 18.0;

    const double t = static_cast<double>(n_);
    const double arg = (t - n0) / spread;
    const double pulse = A * std::exp(-(arg * arg));

    E_[src] += pulse;  // soft source
}

void FDTD1D::step()
{
    updateH_();
    updateE_();
    applySource_();
    applyBoundary_();
    ++n_;
}

