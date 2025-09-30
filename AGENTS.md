# Repository Guidelines

## Project Structure & Module Organization
`lib/include/` hosts the public headers for the `cli` namespace, while `lib/src/` contains the C++17 implementations mirrored by filename. The bundled TAP harness lives under `lib/tap/` and is only pulled into builds through CMake. Tests sit in `t/`, numbered (`NN-description.cpp`) to reflect execution order; update `t/CMakeLists.txt` when adding new files. Generated documentation (HTML and man pages) resides in `doc/`, and workflow playbooks for agents live in `workflows/`.

## Build, Test, and Development Commands
Configure once per build directory:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```
Incremental builds:
```bash
cmake --build build
```
Run the TAP-based suite (mirrors `make test`):
```bash
ctest --test-dir build --output-on-failure
```
For ad-hoc debugging of parser behavior, create small drivers in `t/` and reuse `cmake --build build --target tests` to compile them.

## Coding Style & Naming Conventions
Production code targets modern C++17 with the standard library only. Follow the existing two-space indentation inside namespaces and keep braces on the same line as declarations. Internal helpers use lowercase snake_case (`is_prefix_char`), while public API sticks to PascalCase (`Command`, `Info`). Inputs and handles mirror the DSL syntax shown in `README.md`; keep new specs lowercase unless they wrap user-facing literals. No project-wide formatter is enforced, so run `clang-format` manually when touching large blocks to keep spacing consistent.

## Testing Guidelines
Extend coverage by cloning the numeric naming pattern in `t/`. Each scenario should focus on one parser feature and arrange/act/assert using libtap++ macros already included. Add new sources to `t/CMakeLists.txt` and rerun `ctest --output-on-failure` before submitting. When reproducing bugs, commit the failing test first, then the fix, to document intent. While no formal coverage gate exists, aim to exercise both success and error paths, including multi-option edge cases.

## Commit & Pull Request Guidelines
Commits trending in history are short, imperative, and lowercase (`fix bugs`, `update readme`). Follow that style and keep changesets focused. Pull requests should describe the problem, outline the solution, and link related issues. Attach `ctest` results and note any API or DSL additions. If changes impact documentation, update `doc/` or `README.md` alongside the code so reviewers can validate behavior quickly.

## Documentation & Reference
Consult `README.md` for API background and option syntax; sync any behavioral changes there. For deeper details, rebuild the docs in `doc/` when interfaces move. Keep agent notes in `workflows/` aligned with this guide so automations offer consistent instructions.
