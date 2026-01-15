import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from pathlib import Path
import matplotlib as mpl

mpl.rcParams.update({
    "figure.figsize": (10, 4),
    "figure.dpi": 100,
    "font.family": "serif",
    "font.serif": ["Times New Roman", "Times", "Computer Modern Roman"],
    "mathtext.fontset": "cm",
    "axes.labelsize": 12,
    "axes.titlesize": 12,
    "xtick.labelsize": 10,
    "ytick.labelsize": 10,
    "legend.fontsize": 10,
    "lines.linewidth": 1.5,
    "axes.grid": True,
    "grid.linestyle": "--",
    "grid.linewidth": 0.5,
    "grid.alpha": 0.6,
})

# ----------------------------
# Locate project folders
# ----------------------------
script_dir = Path(__file__).resolve().parent
project_root = script_dir.parent
data_dir = project_root / "data"
plots_dir = project_root / "plots"

if not data_dir.exists():
    raise FileNotFoundError(f"data/ directory not found at: {data_dir}")

plots_dir.mkdir(exist_ok=True)

# ----------------------------
# Gather CSV files
# ----------------------------
csv_files = sorted(data_dir.glob("fields_*.csv"))
if not csv_files:
    raise FileNotFoundError("No files matching fields_*.csv found in data/")

print(f"Found {len(csv_files)} snapshots in {data_dir}")

# ----------------------------
# Detect delimiter from first file header
# ----------------------------
first_line = csv_files[0].read_text(encoding="utf-8", errors="replace").splitlines()[0]
delimiter = ";" if ";" in first_line else ","
print("Detected delimiter:", repr(delimiter))
print("Header:", first_line)

# ----------------------------
# Frame loader
# ----------------------------
def load_frame(file_path: Path):
    data = np.genfromtxt(file_path, delimiter=delimiter, names=True, dtype=float, encoding=None)
    x = data["x"]
    E = data["E"]
    H = data["H"]

    # Some dumps may leave last H empty -> becomes nan; fix for plotting
    H = np.nan_to_num(H, nan=0.0)
    return x, E, H

x0, E0, H0 = load_frame(csv_files[0])

# ----------------------------
# Precompute y-limits (sample for speed)
# ----------------------------
Emax = 0.0
Hmax = 0.0
stride = max(1, len(csv_files) // 60)  # sample up to ~60 files
for f in csv_files[::stride]:
    _, Ei, Hi = load_frame(f)
    Emax = max(Emax, float(np.nanmax(np.abs(Ei))))
    Hmax = max(Hmax, float(np.nanmax(np.abs(Hi))))

if Emax == 0.0: Emax = 1.0
if Hmax == 0.0: Hmax = 1.0

# ----------------------------
# Setup plot with twin y-axis
# ----------------------------
fig, axE = plt.subplots(figsize=(10, 4))
axH = axE.twinx()

(lineE,) = axE.plot(x0, E0, color="black", label=r"$E(x)$")
(lineH,) = axH.plot(x0, H0, color="tab:blue", linestyle="--", label=r"$H(x)$")

axE.set_xlabel(r"$x\ \mathrm{[m]}$")
axE.set_ylabel(r"$E\ \mathrm{[V/m]}$")
axH.set_ylabel(r"$H\ \mathrm{[A/m]}$")

axE.grid(True)

axE.set_ylim(-1.1 * Emax, 1.1 * Emax)
axH.set_ylim(-1.1 * Hmax, 1.1 * Hmax)

# Combine legends from both axes
lines = [lineE, lineH]
labels = [l.get_label() for l in lines]
axE.legend(lines, labels, loc="upper right", frameon=False)

axE.set_title(r"1D FDTD electric and magnetic fields")

# Optional: mark source position if you know it's at N/4
# x_src = x0[len(x0)//4]
# axE.axvline(x_src, linestyle="--", linewidth=1.0)

# ----------------------------
# Animation update
# ----------------------------
def update(frame_idx: int):
    file_path = csv_files[frame_idx]
    x, E, H = load_frame(file_path)

    lineE.set_data(x, E)
    lineH.set_data(x, H)

    axE.set_title(r"1D FDTD electric and magnetic fields")
    return (lineE, lineH)

fps = 30
anim = FuncAnimation(fig, update, frames=len(csv_files), interval=1000/fps, blit=True)

plt.tight_layout()
plt.show()

# ----------------------------
# Optional save
# ----------------------------
anim.save(plots_dir / "EH_fields.gif", writer="pillow", fps=fps)
# anim.save(plots_dir / "EH_fields.mp4", writer="ffmpeg", fps=fps)
