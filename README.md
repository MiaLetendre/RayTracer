# JSON-Driven C++ Ray Tracer

A raytracer that parses 3D scene environments entirely from JSON configurations, processes mathematical ray-surface geometry intersections using **Eigen**, implements a fully realized **Phong Shading model**, and exports rendered imagery directly into high-fidelity PPM formats.

*This repository expands upon foundational scene rendering structures originally detailed in the COMP371_all repository.*

---

## Key Features

* **Dynamic JSON Parser:** Fully automated layout configuration via `nlohmann/json`. Supports real-time validation flags (`radjson`, `centrejson`, `rectjson`, etc.) ensuring error-free data handling for complex environments.
* **Geometrical Intersections:** Core implementations for deterministic ray-sphere tracking (via the quadratic formula) and finite ray-rectangle tracking (via bounded edge-vector dot product boundaries).
* **Advanced Lighting Models:**
* **Hard Shadows & Point Lights:** Fast, analytical point-light source evaluations mapping direct ray obstructions.
* **Soft Shadows & Area Lights:** Stochastic stratified $8 \times 8$ area light jitter-sampling ($64$ shadow-ray passes per shading point) to simulate realistic penumbras.


* **Camera Pipeline:** Comprehensive coordinate framing mapping Camera space via `LookAt`, `Up`, and `Fov` into Viewport projection parameters.
* **Anti-Aliasing (AA):** Jittered sub-pixel multisampling (randomized offsets averaged across multiple passes) to eliminate harsh edge stepping.

---

## Mathematical & Shading Architecture

### Phong Shading Model

Surface coloration is computed via an empirical accumulation of ambient, diffuse, and specular components:

$$I = I_a k_a + \sum_{m \in \text{Lights}} \left[ I_{d,m} k_d (\vec{L}_m \cdot \vec{N}) + I_{s,m} k_s (\vec{R}_m \cdot \vec{V})^{p_c} \right]$$

Where:

* $\vec{L}_m$ = Unit vector pointing toward light source $m$.
* $\vec{N}$ = Surface normal vector at the intersection hitpoint.
* $\vec{V}$ = View direction pointing back toward the camera position.
* $\vec{R}_m$ = Perfect specular reflection direction vector ($2(\vec{L}_m \cdot \vec{N})\vec{N} - \vec{L}_m$).
* $p_c$ = Phong exponent coefficient governing highlight crispness.

### Geometry Intersection Logic

* **Sphere Intersection:** Solves the standard quadratic formulation: $at^2 + bt + c = 0$, mapping the ray equation $\vec{P}(t) = \vec{O} + t\vec{D}$ against surface thresholds.
* **Rectangle Intersection:** Evaluates the infinite coplanar projection value $t$:

$$t = \frac{(\vec{P}_1 - \vec{O}) \cdot \vec{N}}{\vec{D} \cdot \vec{N}}$$



It then executes a local coordinate boundary box validation using dot product projection limits over defined axes $\vec{Edge}_A$ and $\vec{Edge}_B$.

---

## Input Configuration Format (JSON)

The scene profile maps geometry, light arrays, and camera parameters inside a root file.

```json
{
  "geometry": [
    {
      "type": "sphere",
      "centre": [0.0, 0.0, -5.0],
      "radius": 1.5,
      "ac": [1.0, 0.0, 0.0],
      "dc": [1.0, 0.0, 0.0],
      "sc": [1.0, 1.0, 1.0],
      "ka": 0.2,
      "kd": 0.6,
      "ks": 0.4,
      "pc": 32.0,
      "visible": true
    },
    {
      "type": "rectangle",
      "p1": [-2.0, -1.0, -6.0],
      "p2": [2.0, -1.0, -6.0],
      "p3": [2.0, -1.0, -3.0],
      "p4": [-2.0, -1.0, -3.0],
      "ac": [0.5, 0.5, 0.5],
      "dc": [0.8, 0.8, 0.8],
      "sc": [0.0, 0.0, 0.0],
      "ka": 0.1,
      "kd": 0.7,
      "ks": 0.0,
      "pc": 1.0
    }
  ],
  "light": [
    {
      "type": "point",
      "centre": [2.0, 4.0, -2.0],
      "id": [1.0, 1.0, 1.0],
      "is": [1.0, 1.0, 1.0],
      "use": true
    }
  ],
  "output": [
    {
      "filename": "render.ppm",
      "size": [800, 600],
      "centre": [0.0, 0.0, 0.0],
      "lookat": [0.0, 0.0, -1.0],
      "up": [0.0, 1.0, 0.0],
      "fov": 45.0,
      "ai": [0.2, 0.2, 0.2],
      "bkc": [0.05, 0.05, 0.05],
      "antialiasing": true,
      "raysperpixel": [5.0]
    }
  ]
}

```

---

## Project Structure & Key Files

* `RayTracer.cpp` / `RayTracer.h`: Core execution pipeline managing scene loops, shadow testing, and Phong reflection functions.
* `../external/json.hpp`: Header-only JSON file processor utilities (`nlohmann`).
* `../external/simpleppm.h`: Streamlined context utility providing automated export channels to raw pixel maps (`.ppm`).

---

## Compilation and Execution

Ensure you have the **Eigen 3** linear algebra library installed on your system before compiling.

### Building via GCC/Clang:

```bash
g++ -std=c++17 -I /usr/include/eigen3 main.cpp RayTracer.cpp -o RayTracerOutput

```

### Running the Executable:

```bash
./RayTracerOutput scene.json

```

---

## Acknowledgments & Credits

* Base framework concepts adapted from the COMP371_all repository.
* JSON data extraction courtesy of [nlohmann/json](https://github.com/nlohmann/json).
* Matrix and vector manipulation framework powered by [Eigen](https://eigen.tuxfamily.org/).

# Works Cited
Tiperiu. COMP371_all. GitHub, github.com/tiperiu/COMP371_all. Accessed 18 May 2026.
