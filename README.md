# OPTKIT-GUI
GUI Interface for the OPTKIT library.

## Building

No system-wide `premake5` installation is required. The build script bootstraps
premake5 directly from the OPTKIT submodule (`lib/OPTKIT/lib/premake5`).

```bash
# Generate GNU Makefiles (default) and build
./build.sh          # generates Makefiles via premake5 gmake2
make                # build Debug (default)
make config=release # build Release
```

Other premake5 actions are forwarded as the first argument:

```bash
./build.sh vs2022   # generate Visual Studio 2022 solution (Windows)
./build.sh xcode4   # generate Xcode project (macOS)
```

