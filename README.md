# PacManGame

## Formatting and Linting

This project uses clang-format for code formatting and clang-tidy for static analysis.

### Config files

- `.clang-format` at the repo root defines the formatting rules.
- `.clang-tidy` at the repo root defines the lint rules.

Both IDEs (VS Code, Cursor) will respect these files automatically when configured to use clang-format.

### Install tools (macOS)

```bash
brew install clang-format clang-tidy
```

### CMake convenience targets

Reconfigure/build once:

```bash
cmake -S . -B build
cmake --build build
```

- Run formatter (in-place fix):

```bash
cmake --build build --target format
```

- Check formatting (fails on style drift):

```bash
cmake --build build --target format-check
```

- Run clang-tidy (uses compile commands from build/):

```bash
cmake --build build --target tidy
```

To run clang-tidy directly:

```bash
clang-tidy -p build src/*.cpp
```

### IDE integration

- VS Code: set `C_Cpp: Clang_format_style = file` and enable "Format on Save".
- Cursor: respects `.clang-format`; enable format on save in settings.

### Notes

- The formatter uses a Google-based style with 4-space indentation and 120 column limit.
