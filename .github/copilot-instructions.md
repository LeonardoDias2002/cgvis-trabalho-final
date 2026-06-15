<!-- Copilot / AI agent instructions for the cgvis final project -->
# Quick orientation for AI coding agents

This repository is a small OpenGL/GLFW-based game demo used in the INF01047 course. The goal of these instructions is to surface project-specific facts and workflows so an AI agent can be immediately productive.

- **Big picture**: The app is a single native C++ executable that creates an OpenGL context (GLFW + GLAD), loads OBJ models and textures (tinyobjloader, stb_image), builds triangle meshes for a simple scene, then runs a game loop in `src/main.cpp`.

- **Where runtime assets live**: game data (models, textures, `.mtl`) are under the `data/` directory. Source code loads them using relative paths (e.g. `"../../data/..."` from `src/main.cpp`). The binary is expected to run from `bin/Linux` (CMake `run` target chdirs into output dir), so preserve relative paths or update the working directory when running locally.

- **Build system and run commands**:
  - Preferred (Linux): use CMake preset as documented in `COMPILACAO.md`:
    - `cmake --workflow --preset configure-build-run` (configures, builds, runs)
    - Equivalent manual sequence: `cmake -B build -S . && cmake --build build && cmake --build build -- run`
  - Alternative: `make` / `make run` (Makefile present).
  - VSCode: uses the CMake Tools extension; use the Play button in the status bar.

- **Key files / dirs to inspect**:
  - `CMakeLists.txt` — controls compilation, adds `SOURCES` list. When adding/removing `.cpp` files, update the `SOURCES` list.
  - `COMPILACAO.md` — canonical build/run instructions (copy-pasteable commands).
  - `src/main.cpp` — large single-file program with most logic (initialization, scene construction, game loop, input callbacks).
  - `include/` — shared headers (notably `globals.h` which exposes many `g_` globals used across files).
  - `lib-linux/` — prebuilt static GLFW used on Linux; other platform libs live in `lib-*` dirs.
  - `shader_*` and `src/*.glsl` — GPU programs are loaded from `src/shader_vertex.glsl` and `src/shader_fragment.glsl`.

- **Conventions & important patterns**:
  - Global state: the project relies heavily on globals (`globals.h`, `g_` prefix). Expect changes to affect many translation units.
  - Scene construction: models are loaded with `ObjModel` and converted into GPU triangles via `BuildTrianglesAndAddToVirtualScene()` (see `src/main.cpp` and `src/tiny_obj_loader.cpp`).
  - Data loading assumes the binary working directory contains `../../data/` relative to `src/` when run from build output — prefer using the CMake `run` target or run the binary from `bin/Linux` to avoid broken paths.
  - CMake file existence check: `CMakeLists.txt` verifies listed source files exist and will `FATAL_ERROR` if they don't. When moving/renaming files, update `SOURCES` first.

- **Dependencies to be aware of**:
  - GLAD, GLFW (static lib in `lib-linux/`), GLM (headers provided under `include/glm/`), tinyobjloader (`src/tiny_obj_loader.cpp`), `stb_image`.
  - `CMake` >= 3.10 is used; the project targets C++11.

- **When making edits** (practical rules for an agent):
  - If adding a `.cpp` file, add it to the `SOURCES` list in `CMakeLists.txt` and ensure the path exists — otherwise CMake will abort.
  - Avoid refactoring all globals at once: prefer local, incremental changes and run to verify behavior.
  - Preserve relative data paths or change the `run` target/workdir consistently.
  - Prefer static analysis or compilation checks before proposing runtime changes; compile with `cmake --build build`.

- **Debugging & running tips**:
  - Use `cmake --build build -- run` to run with correct working dir (the `run` target changes to `CMAKE_RUNTIME_OUTPUT_DIRECTORY`).
  - To reproduce OpenGL/driver issues, print GPU info from `main.cpp` — the program already logs vendor/renderer/version at startup.

- **No test/CI framework**: there are no unit tests or CI configs. Validate by building and running the binary.

If anything here is unclear or you'd like more detail (e.g., call-graph of `main.cpp`, specific globals to avoid touching, or a proposed small refactor), tell me which area to expand. 
