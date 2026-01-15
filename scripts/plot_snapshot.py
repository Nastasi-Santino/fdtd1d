import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

script_dir = Path(__file__).resolve().parent
project_root = script_dir.parent
data_dir = project_root / "data"

csv_files = sorted(data_dir.glob("fields_*.csv"))
if not csv_files:
    raise FileNotFoundError("No CSV files found in data/")

csv_file = csv_files[-1]
print("Plotting:", csv_file)

# Peek first line to guess delimiter
first_line = csv_file.read_text(encoding="utf-8", errors="replace").splitlines()[0]
delimiter = ";" if ";" in first_line else ","
print("Detected delimiter:", repr(delimiter))
print("Header line:", first_line)

data = np.genfromtxt(csv_file, delimiter=delimiter, names=True, dtype=float, encoding=None)

# If names got weird due to spaces, print them
print("Columns:", data.dtype.names)

x = data["x"]
E = data["E"]

plt.figure(figsize=(8, 4))
plt.plot(x, E, label="E(x)")
plt.xlabel("x [m]")
plt.ylabel("E [V/m]")
plt.title(f"Electric field snapshot\n{csv_file.name}")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
