# PacManGame

A networked Pac-Man game implementation in C++.

## Running the Game

The main interface for building and running the game is the `rebuild_and_run.sh` script. This script automatically handles CMake configuration, building, and also runs `clang-format` and `clang-tidy` if they are installed.

```bash
# Run locally (starts both server and client automatically)
./rebuild_and_run.sh localhost

# Run server only
./rebuild_and_run.sh server [--port <num> --tick <hz>]

# Run client only
./rebuild_and_run.sh client [--ip <addr> --port <num>]
```

To host the server on an Ubuntu 24 VM, first install dependencies with `./scripts/install_deps_ubuntu.sh`, then start the server with `./rebuild_and_run.sh server` (or `./rebuild_and_run.sh server --port 54000` for an explicit port). From another machine, connect the client to the VM using its IP address, for example `./rebuild_and_run.sh client --ip <VM_IP> --port 54000`.

## Formatting and Linting

The project uses a Google-based style with 4-space indentation and 120 column limit. `clang-format` and `clang-tidy` rules are defined in `.clang-format` and `.clang-tidy` files at the root.

To install the tools on macOS:

```bash
brew install clang-format clang-tidy
```

Most IDEs will respect these files automatically when configured to use `clang-format` (enable "Format on Save").

*Note: If you need to run formatting or linting manually without running the game, you can run `cmake --build build --target format` or `cmake --build build --target tidy`.*

## Documentation

For detailed project documentation (including reports and requirements for the university course), please see [docs/README.md](docs/README.md).
