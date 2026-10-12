# Verification

The macOS Apple Silicon release was built with AppleClang and JUCE 8.0.12. The portable CMake configuration was checked both with the full desktop build and with the engine-only configuration; both CTest runs passed. The running app was visually checked at 13×13, at full display width, and at a smaller window size. Checks covered setup and rules panels, expanded forecast layout, cell targeting, anchoring, undo, turn advancement, and sidebar scrolling.

Version 1.3 uses a 12px minimum for small labels and values, 11.5px bold section captions, and a 13px minimum for explanatory text. Secondary colours and strokes were strengthened. Dense prediction badges have more room, and cell coordinates appear in the footer.

The engine assertions cover seeded population, neighbour damage, negative resistance, anchor duration, transfer healing, severed edges, corrupted tools, permanent voids, geometry remapping, containment victory, collapse loss, and value/board invariants across all five sizes and three difficulties. Assertions remain enabled in Release builds.

A separate comparison evaluated the reference HTML's original engine in JavaScriptCore. Native and reference cell values and anchor counters matched at initialization and after every simulated turn across 15 size/difficulty scenarios using seed 129741 and both geometry options. This comparison exercised weather, satellites, and expansion/contraction without player actions; operations were checked by the C++ assertions.

To reproduce the included engine checks without JUCE:

```sh
cmake -S . -B build-tests -DZERO_BUILD_APP=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

The original HTML and the one-off JavaScriptCore comparison harness are not runtime dependencies and are not included in this repository.
