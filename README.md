# 💧 Water Quality Checker

A Windows-based C application for analyzing water quality using **Electrical Conductivity (EC)** and **Total Dissolved Solids (TDS)**.

The project provides a simple graphical interface where the user enters the electrical measurements and water temperature, and the program calculates conductance, temperature-corrected EC, TDS, and a water-quality classification.

## ✨ Features

- 🖥️ Simple Windows graphical interface
- ⚡ Electrical conductance calculation
- 🧪 Electrical Conductivity (EC) calculation
- 🌡️ Temperature compensation to 25 °C
- 💧 TDS estimation
- 📊 Automatic water-quality classification
- 🧹 Clear button for quickly resetting the inputs

## 🧮 Calculations

The program uses the following calculations:

### Conductance

```text
G = (Current / 1,000,000) / Voltage
```

### Electrical Conductivity

```text
EC = G × K × 1,000,000
```

where:

- `G` = conductance
- `K` = cell constant
- `EC` = electrical conductivity

### Temperature-compensated EC

```text
EC25 = EC / (1 + a × (Temperature - 25))
```

The program uses:

```text
a = 0.02
```

### TDS

```text
TDS = EC25 × 0.5
```

## 📊 Water Quality Classification

The application classifies water according to the calculated TDS:

| TDS | Quality |
|---:|---|
| `< 300` | Excellent |
| `300 – < 600` | Good |
| `600 – < 900` | Fair |
| `900 – < 1200` | Poor |
| `≥ 1200` | Unacceptable |

## 🖥️ Inputs

The graphical interface accepts:

- **Supply Voltage (V)**
- **Cell Constant K (1/cm)**
- **Meter Current (µA)**
- **Water Temperature (°C)**

The output displays:

- Conductance
- EC at 25 °C
- TDS
- Water Quality

## 📁 Project Structure

```text
Water-Quality-Checker/
│
├── water_quality_gui.c       # Main graphical application
├── water_quality_gui.exe    # Compiled Windows application
├── water2.c                  # Original console version
├── water2.exe                # Compiled console version
└── water2_frontpage.c        # Earlier front-page version
```

## ▶️ How to Run

### Option 1 — Run the Windows application

Download:

```text
water_quality_gui.exe
```

and run it on a Windows computer.

### Option 2 — Compile from source

Make sure GCC/MinGW is installed, then open the project folder in VS Code and run:

```bash
gcc water_quality_gui.c -o water_quality_gui.exe -mwindows
```

Then run:

```powershell
.\water_quality_gui.exe
```

## 🛠️ Technologies Used

- **C**
- **Win32 API**
- **GCC / MinGW**
- **Visual Studio Code**

## 🎓 Project Purpose

This project demonstrates how electrical measurements can be processed in C to estimate water conductivity and TDS, while providing a user-friendly graphical interface for entering measurements and viewing the calculated results.

## 👨‍💻 Author

**PRUTHVIRAJROM**

---

💧 *Water Quality Checker — Electrical Conductivity & TDS Analysis*
