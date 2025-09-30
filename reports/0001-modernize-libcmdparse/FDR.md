feat: modernize libcmdparse for wide cpp adoption

## motivation
- Current build config relies on global flags (`set (CMAKE_CXX_FLAGS ...)`) and lacks exported targets, making consumption via `find_package` impossible (`CMakeLists.txt:5-21`).
- Only a shared library is built, and headers are installed individually, so downstream consumers cannot choose static builds or leverage transitive include paths (`lib/src/CMakeLists.txt:1-5`).
- Tests depend on a custom TAP shell wrapper, which limits integration with common tooling and IDEs (`t/CMakeLists.txt:30-43`).
- Docs expressly state Windows is unsupported and provide only manual build steps, blocking wider adoption (`README.md:156-175`).
- AGENTS guidelines emphasise maintainable structure and contributor clarity, yet there is no contributor roadmap for modernization (`AGENTS.md:3-31`).

## proposed_solution
1. **Rebuild the CMake surface**: adopt target-based commands, generate export sets, emit `cmdparseConfig.cmake`/`cmdparseConfigVersion.cmake`, and add options for static/shared builds and position-independent code. Provide `INSTALL_INTERFACE` include paths and optional `pkg-config` metadata.
2. **API refinements for modern C++**: introduce overloads using `std::span<std::string_view>` and iterators instead of raw `char**`, replace shared pointer factories with value semantics where possible, and add explicit error reporting (e.g. `std::expected` or `tl::expected`) without breaking existing API.
3. **Testing and CI refresh**: migrate the suite to Catch2 (or GoogleTest) with property-based coverage for DSL parsing, keep TAP scripts as legacy wrappers, and wire GitHub Actions workflows for Linux, macOS, and Windows builds with sanitizers.
4. **Cross-platform and packaging**: audit POSIX-only code paths (prefix parsing, filesystem usage) and adjust so the library compiles cleanly with MSVC. Publish example integrations (`examples/cli_calc`, `examples/subcommands`) and prepare manifests for vcpkg and Conan.
5. **Documentation and community assets**: rewrite the README quick-start with CMake integration samples, surface API reference in `doc/`, add CONTRIBUTING/CODE_OF_CONDUCT, and capture DSL grammar in a dedicated guide.

## technical_considerations
- Maintain baseline at C++17 per current headers; optionally gate C++20 features behind configuration to avoid alienating users on older toolchains.
- Ensure backward compatibility by keeping existing `Command::parse(char** argv, int argc)` and layering new interfaces on top; document deprecation timeline.
- Evaluate dependency footprint for new features (e.g. `tl::expected` vs rolling own) to preserve the “standard library only” promise from the README (`README.md:132-139`).
- Packaging artifacts must work on case-sensitive and case-insensitive filesystems; verify export names conform to CMake best practices.

## acceptance_criteria
- `find_package(cmdparse CONFIG)` works after `cmake --build build --target install`, yielding usable imported targets for both static and shared builds.
- CI workflows succeed on Linux (GCC/Clang), macOS (Clang), and Windows (MSVC) with unit tests and sanitizer jobs.
- Library offers `parse(std::span<std::string_view>)` (or equivalent) with tests proving interoperability while existing raw-pointer API remains functional.
- Documentation includes an updated README quick-start, DSL reference, and new CONTRIBUTING guide reviewed by stakeholders.
- vcpkg and Conan recipe PRs drafted with build logs confirming installation on at least one platform each.

## solution_model
| Phase | Focus | Key Deliverables |
| --- | --- | --- |
| 1 | Build & packaging | Target-based CMake, install exports, pkg-config file, CI job skeleton |
| 2 | API evolution | `std::span` parsing path, improved error handling, regression tests |
| 3 | Testing & CI | Catch2 suite, sanitizer runs, Windows build fixes |
| 4 | Distribution | Examples directory, README refresh, package manager manifests |
| 5 | Governance | CONTRIBUTING, CODE_OF_CONDUCT, roadmap updates |

## impacted_files
- `CMakeLists.txt`, `lib/src/CMakeLists.txt`, `t/CMakeLists.txt`
- `lib/include/*.h`, `lib/src/*.cpp`
- `doc/*`, `README.md`
- new `examples/`, `cmake/`, `packaging/` directories
- GitHub workflow files under `.github/workflows/`

## references
- `AGENTS.md`
- `CMakeLists.txt`
- `lib/src/CMakeLists.txt`
- `t/CMakeLists.txt`
- `README.md`
