# MapEngine

A real-time geospatial rendering engine built in C++20 with OpenGL. Renders OpenStreetMap raster tiles, roads, and buildings with interactive pan/zoom, coordinate transforms, and geometry processing — built as a take-home engineering task.

![MapEngine Demo](Assets/demo.png)

---

## Features

### Core
- **Raster Tile Rendering** — Fetches and renders OSM tiles with a dynamic tile grid across zoom levels 1–19
- **Smooth Zoom Transitions** — Tiles fade in/out with alpha blending on zoom change, eliminating hard pops
- **Road Rendering** — Polylines rendered with correct screen-space width via quad expansion in the vertex shader; road types (motorway, primary, residential, etc.) rendered with distinct widths
- **Douglas-Peucker Simplification** — Geometry simplified at lower zoom levels to reduce vertex count and improve performance
- **Building Triangulation** — Polygon triangulation using [earcut.hpp](https://github.com/mapbox/earcut.hpp) for building footprints fetched from the Overpass API; typed by land use (commercial, residential, industrial)
- **Web Mercator Transforms** — Full EPSG:3857 / Web Mercator pipeline: lat/lng ↔ world pixel ↔ screen pixel with double-precision arithmetic to avoid coordinate drift at high zoom
- **Pan & Zoom** — Mouse drag panning and scroll-wheel zoom with cursor-anchored zoom behavior
- **Distance Measurement** — Click two points on the map to calculate the Haversine distance, displayed in the ImGui overlay
- **Compass Indicator** — On-screen compass showing cardinal North, rendered via ImGui DrawList
- **Frame Time Display** — Real-time ms/frame and FPS counter in the ImGui overlay

### Performance
- VAOs/VBOs with dynamic buffer management; geometry uploaded once per fetch, re-used across frames
- Camera pan handled by a per-frame shader uniform delta (no re-upload on pan)
- Debounced Overpass fetches — roads and buildings only re-fetched after the camera has stopped moving for 2 seconds
- Overpass requests run on background threads; main render loop never blocks
- Zoom-level guards prevent fetching geometry at inappropriate scales

---

## Architecture

```
MapEngine/
├── Main.cpp                    # Entry point, window + render loop
├── AnubisMath/                 # Vector math, coordinate transforms
│   ├── Vector2D, Vector3D, Vector4D
│   └── Anubis::LatLngToPixel, PixelToLatLng, HaversineDistanceKm
├── AnubisRenderer/             # Low-level OpenGL abstractions
│   ├── BufferGroup             # VAO/VBO/EBO management
│   ├── ShadersManager          # Shader compilation and caching
│   └── StateManager            # OpenGL state machine wrapper
├── CoreEngine/                 # Window, input, and engine lifecycle
│   └── CWindowManager          # GLFW window + ImGui host
├── MapLayer/                   # All map-domain logic
│   ├── CMapManager             # Orchestrates tile grid, roads, buildings
│   ├── CMapCamera              # Pan/zoom, world pixel origin
│   ├── CTileGrid               # Tile slot management and LRU eviction
│   ├── CTileGridRenderer       # Tile texture download and GL rendering
│   ├── CPolyLineRenderer       # Road polyline rendering with DP simplification
│   ├── CPolyTriangulationRenderer  # Building polygon triangulation + rendering
│   └── CCompassRenderer        # Compass overlay (ImGui DrawList)
├── LibImageUI/                 # STB image + ImGui integration
└── Extern/                     # Third-party headers and prebuilt libs
    ├── glfw3                   # Window and input
    ├── glad                    # OpenGL function loader
    ├── glm                     # Math (matrices, vectors)
    ├── imgui                   # Immediate mode UI overlay
    ├── nlohmann/json           # JSON parsing (Overpass responses)
    ├── httplib                 # HTTP client (tile + Overpass fetches)
    ├── mapbox/earcut.hpp       # Polygon triangulation
    └── stb_image               # PNG tile decoding
```

### Key Design Decisions

**Double-precision world pixels** — At zoom 16+, world pixel coordinates exceed 9,800,000. Storing these as `float` introduces ~1 pixel of positional error per building vertex. All lat/lng → world pixel math is done in `double`; the origin is subtracted before casting to `float` for the GPU, keeping vertex values small (~screen-space) where float precision is sufficient.

**Upload-time origin baking** — Building and road vertices are baked into screen space relative to the camera origin at upload time. Panning is handled by passing a `u_CameraOrigin` delta uniform each frame — no re-upload on pan. Re-upload only occurs on zoom change or when new geometry arrives from Overpass.

**Threaded Overpass fetches** — Roads and buildings are fetched on detached background threads. Results are written to pending buffers and swapped onto the main thread at the start of each `Update()` frame via a mutex-protected ready flag. In-progress flags prevent duplicate concurrent fetches; a 30-second watchdog resets stuck flags.

**Overpass rate limiting** — A global request timestamp enforces a minimum 1.5-second gap between any two Overpass requests. HTTP 429/504 responses trigger exponential backoff retry (up to 3 attempts: 3s, 6s, 9s).

---

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| [GLFW](https://www.glfw.org/) | 3.3+ | Window creation and input |
| [GLAD](https://glad.dav1d.de/) | OpenGL 4.5 | OpenGL function loader |
| [GLM](https://github.com/g-truc/glm) | 0.9.9+ | Math (matrices, projection) |
| [Dear ImGui](https://github.com/ocornut/imgui) | 1.90+ | UI overlay (stats, compass, distance) |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.x | Overpass API JSON parsing |
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | 0.14+ | HTTP/HTTPS tile and Overpass requests |
| [earcut.hpp](https://github.com/mapbox/earcut.hpp) | 2.x | Polygon triangulation for buildings |
| [stb_image](https://github.com/nothings/stb) | 2.x | PNG tile decoding |
| [OpenSSL](https://www.openssl.org/) | 3.x | HTTPS support for httplib |

All third-party headers are vendored under `Extern/include`. Prebuilt GLFW `.lib`/`.so` files are under `Extern/lib`.

---

## Build Instructions

### Prerequisites

- **Windows**: Visual Studio 2022 (MSVC v143), CMake 3.23+, [OpenSSL for Windows](https://slproweb.com/products/Win32OpenSSL.html) installed to `C:/Program Files/OpenSSL-Win64`
- **Linux**: GCC 12+ or Clang 15+, CMake 3.23+, OpenSSL dev package (`libssl-dev`), OpenGL dev package (`libgl-dev`)

### Windows (Visual Studio)

```bat
git clone https://github.com/El-sharqawy/MapEngine.git
cd MapEngine
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

Then run:
```bat
build\Debug\MapEngine.exe
```

### Linux

```bash
git clone https://github.com/El-sharqawy/MapEngine.git
cd MapEngine
sudo apt install libssl-dev libgl-dev   # Ubuntu/Debian
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
./build/MapEngine
```

> **Note**: On Linux, copy `Extern/lib/Linux/libglfw.so` next to the executable or set `LD_LIBRARY_PATH=./Extern/lib/Linux` before running.

---

## How to Run

Launch the executable. The engine defaults to Cairo, Egypt at zoom 12.

| Action | Control |
|--------|---------|
| Pan map | Click and drag |
| Zoom in/out | Mouse scroll wheel |
| Measure distance | Right-click two points on the map |
| Toggle wireframe | `Q` key |

The ImGui overlay (top-left) shows:
- Frame time (ms) and FPS
- Current zoom level
- Measured distance between two picked points
- Compass with cardinal North indicator

Roads and buildings load automatically when you pan to a new area. They appear within 2–5 seconds depending on Overpass API response time.

---

## What I'd Improve With More Time

- **Tile caching to disk** — Currently tiles are re-fetched every session. Persisting to a local SQLite MBTiles store would eliminate redundant network requests.
- **Vector tiles (MVT)** — Switching from raster tiles + separate Overpass queries to Mapbox Vector Tiles would give sub-100ms geometry load times and resolution-independent rendering.
- **GPU instancing for POI markers** — Point-of-interest symbols (restaurants, transit stops) are natural candidates for instanced rendering with a single draw call.
- **End-to-end double-precision pipeline** — The current approach subtracts the camera origin on CPU before uploading to the GPU. A cleaner solution would be a 64-bit world space with relative-to-eye (RTE) rendering to avoid the conversion step entirely.
- **Label rendering** — Road and building name labels require signed distance field (SDF) font rendering, which would significantly improve map readability.
- **Android/JNI wrapper** — The rendering core is platform-agnostic C++; wrapping it with a JNI surface and exposing a `MapView` Kotlin API would be straightforward given the existing separation of concerns.

---

## AI Tools Used

[Perplexity AI](https://www.perplexity.ai) was used as a pair-programming assistant throughout this project. Specific areas where it provided assistance:

- **Debugging coordinate precision** — Identifying that float world-pixel values lose sub-pixel precision at zoom 16+ (Cairo coordinates ~9,845,000px) and designing the origin-subtraction fix
- **Earcut winding order** — Diagnosing that OSM building polygons occasionally arrive in clockwise winding and adding the signed-area winding check before triangulation
- **Overpass query structure** — Structuring the OverpassQL bbox queries for roads and buildings with appropriate zoom-level guards
- **Camera pan delta architecture** — Designing the upload-origin vs. render-time delta approach that avoids per-frame re-uploads on pan

All architectural decisions, system design, module boundaries, shader code, and debugging approach were my own. AI was used as a sounding board and research accelerator — not as a code generator.

---

## License

MIT — see [LICENSE.txt](LICENSE.txt)
