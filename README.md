# Zero Is Infectious

A native C++17 strategy game about containing infectious zeros in a numerical world. Built from scratch using JUCE, with a warm Swiss-style interface and an independent game engine. There is no HTML, JavaScript, browser, or web view in the app.

## Download

[Download the macOS app — Apple Silicon](https://github.com/maetyu-d/zero_is_infectious/raw/refs/heads/main/dist/Zero-Is-Infectious-1.3-macOS-arm64.zip). Extract the ZIP and open **Zero Is Infectious.app**.

The included app is version 1.3 and runs natively on Apple Silicon Macs. The source can be built for other platforms supported by JUCE; those builds have not been verified here.

## Build

Requires CMake 3.22 or later and a C++17 compiler. On macOS, install Apple's command-line developer tools. The default configuration downloads JUCE 8.0.12, pinned to an upstream commit, during configuration.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j 6
ctest --test-dir build -C Release --output-on-failure
```

The macOS app is produced at `build/ZeroIsInfectious_artefacts/Release/Zero Is Infectious.app`.

To use an existing JUCE source checkout instead of downloading it:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DJUCE_DIR=/absolute/path/to/JUCE
```

The engine can be tested without JUCE or network access:

```sh
cmake -S . -B build-tests -DZERO_BUILD_APP=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

## Play

Choose a board size (5, 7, 9, 11, or 13 cells per side), difficulty, and optional geometry modes in Setup. Keep enough nonzero cells alive and preserve exactly one zero on the same cell during the final three turns.

| Key | Operation | Behaviour |
| --- | --- | --- |
| 1 | Transfer | Move magnitude within two cells; a zero recipient can be healed. |
| 2 | Exchange | Swap neighbouring cells. |
| 3 | Invert | Reverse the sign of a nonzero cell. |
| 4 | Sever | Block an adjacent edge temporarily. |
| 5 | Sacrifice | Remove a cell permanently; costs two actions. |
| 6 | Anchor | Temporarily stop a zero spreading. |

Transfer, Exchange, and Sever use two cell selections. Enter ends the turn; Z undoes the last operation; Escape clears a selection; ? opens the rules. Arrow keys move the highlighted cell and Space operates on it.

Unanchored zeros spread simultaneously. Lines and 2×2 colonies attack farther; negative values resist overlapping attacks. Arithmetic weather, corrupted tools, satellite zeros, and optional expansion/contraction make later turns harder. Preview badges show the next pulse, while **After this turn** includes geometry changes and survival checks.

## Interface

The app fills the display work area. Board cells adapt to window proportions while their number discs stay circular. Native controls provide keyboard focus and hover feedback; the sidebar scrolls when space is limited. Version 1.3 increases small-text size and weight, strengthens contrast and rules, and improves dense-board preview readability. The game is silent.

## Source and verification

- `Main.cpp`: native JUCE interface and application entry point.
- `Engine.h`: seeded world generation, operations, infection, weather, geometry, and win/loss rules.
- `Tests.cpp`: deterministic engine checks.
- [Verification](Verification.md): build, reference comparison, and visual checks.

JUCE is an external dependency. Its upstream license notice is included in [JUCE-LICENSE.md](JUCE-LICENSE.md).
