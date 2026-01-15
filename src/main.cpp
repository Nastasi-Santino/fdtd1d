#include "fdtd1d.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

void dumpCsv(const std::vector<double>& x,
             const std::vector<double>& E,
             const std::vector<double>& H,
             std::size_t step);

int main(){

    GridConfig config;
    config.N = 800;
    config.dx = 1e-3;
    config.S  = 0.99;
    config.eps = em::eps0;
    config.mu  = em::mu0;
    config.boundary = BoundaryType::Reflective;

    FDTD1D fdtd1d(config);

    std::vector<double> x(config.N);
    for (std::size_t i = 0; i < config.N; ++i){
        x[i] = i * config.dx;
    }

    std::size_t steps = 1200;

    const std::size_t dumpEvery = 5;
    for (std::size_t step = 0; step < steps; ++step) {
    fdtd1d.step();
    if (step % dumpEvery == 0)
        dumpCsv(x, fdtd1d.E(), fdtd1d.H(), step);
    }

    return 0;
}

void dumpCsv(const std::vector<double>& x,
             const std::vector<double>& E,
             const std::vector<double>& H,
             std::size_t step)
{
    // 1) Chequeos básicos (te evitan bugs silenciosos)
    if (x.size() != E.size()) {
        throw std::invalid_argument("dumpCsv: x and E must have the same size");
    }
    if (H.size() + 1 != E.size()) {
        throw std::invalid_argument("dumpCsv: H must have size E.size()-1");
    }

    // 2) Armar nombre de archivo: data/fields_000123.csv
    std::ostringstream filename;
    filename << "data/fields_"
             << std::setw(6) << std::setfill('0') << step
             << ".csv";

    // 3) Abrir archivo
    std::ofstream out(filename.str());
    if (!out) {
        throw std::runtime_error("dumpCsv: could not open output file");
    }

    // 4) Encabezado
    out << "x,E,H\n";

    // 5) Escribir filas
    out << std::setprecision(15);
    for (std::size_t i = 0; i < E.size(); ++i) {
        out << x[i] << "," << E[i] << ",";
        if (i < H.size()) out << H[i];   // H existe hasta N-2
        out << "\n";
    }
}