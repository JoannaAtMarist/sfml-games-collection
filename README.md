# SFML Games Collection  

A small collection of C++/SFML games built as learning projects based on John Horton’s *Beginning C++ Game Programming* (3rd ed.) and expanded with original features/assets.  

Author's code repository available at
https://github.com/PacktPublishing/Beginning-C-Game-Programming-Third-Edition  


## Games Included
- **Game 1 – Witch Cat**: A timed arcade game about collecting pumpkins while avoiding hazards.
- **Game 2 – Spooky Fetch**: A 2-player arcade sports game.
- **Game 3 – Bhost Guster**: A top-down survival shooter.
- **Game 4 – Billina Returns to Oz**: A side-scrolling platformer.


---

## Requirements
- **C++17** compiler
- **CMake 3.16+**
- **SFML 2.6.2** (graphics, window, system, audio)


---

## Build & Run (Command Line)

### 1) Clone
```bash
git clone <REPO_URL>
cd <your-repo-folder>
```

### 2) Install SFML

**Windows (recommended: vcpkg)**
1. Install vcpkg
2. Then:
```bash
vcpkg install sfml
```
3. Configure CMake with the vcpkg toolchain file (example):
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake
```

**macOS (Homebrew)**
```bash
brew install sfml cmake
```

**Ubuntu/Debian**
```bash
sudo apt update
sudo apt install cmake g++ libsfml-dev
```

### 3) Configure + Build
```bash
cmake -S . -B build
cmake --build build
```

Build a single game (optional):
```bash
cmake --build build --target Game1
cmake --build build --target Game2
cmake --build build --target Game3
cmake --build build --target Game4
```

### 4) Run
Your executables and copied assets live under the build output for each game.

**Single-config generators (Ninja/Makefiles):**
```bash
./build/Game1/Game1
./build/Game2/Game2
./build/Game3/Game3
./build/Game4/Game4
```

**Visual Studio / multi-config generators:**
```bash
./build/Game1/Debug/Game1.exe
# or
./build/Game1/Release/Game1.exe
```

---

## Build & Run (VS Code)
1. Install the **CMake Tools** extension.
2. Open the repo folder in VS Code.
3. Pick a compiler “kit” when prompted.
4. Configure + Build.
5. Run a target (Game1/Game2/Game3/Game4) from the CMake Tools UI.

>Note: If a new sound or texture file is added, the CMake configure/build needs to be re-run to ensure the new file is copied to the build folder.  

---

<br><br>  

## Original Asset Notice
- The background music and specific original sprites provided by **Matthew Picciano** (e.g., `matt-picciano-game2-music.ogg`) are proprietary.  

- The flower sprites (all assets in `Game4/graphics/flowers`) provided by **Viviana Moyano** are proprietary.  

- **Code:** Licensed for educational review.  
- **Media Assets:** All rights reserved by the original creator. They may **not** be reused, redistributed, or modified for other projects.    

All art/audio/fonts used in these projects are from their respective creators and are credited in each game’s README.   