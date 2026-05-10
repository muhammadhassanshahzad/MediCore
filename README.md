# MediCore Hospital Management System

## How to Compile and Run

### Prerequisites
- CMake 3.16+
- g++ with C++17 support (MinGW on Windows or MSVC)

### Steps (VS Code Terminal)
```bash
cd "C:/FAST/Spring 2026/OOP/Project"
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
cd ..
./build/MediCore.exe
```

### Notes
- All data files must exist in the `data/` folder before running.
- The executable must be run from the project root so relative paths to `data/` resolve correctly.
