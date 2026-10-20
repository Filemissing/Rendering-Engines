# Raymarcher version benchmark

Every working raymarcher version is built into the engine and can be switched at runtime
(Ray-marcher window → **Version**). The **Version Benchmark** window runs the same benchmark against each one.

| Id | From commit | Technique |
|---|---|---|
| `v0-direct` | 0bdbf15 | No cache: evaluates every primitive per pixel per step |
| `v1-dense-raster` | 20aca78 | Dense 3D texture, baked slice by slice in a fragment shader |
| `v2-dense-jfa` | ef5bc68 | Dense 3D texture, baked with jump flooding in compute shaders |
| `v3-sparse` | current | Sparse brick pool |

Each old version's code and shaders are copies from its commit (`core/RaymarchVersions/v1`, `v2`,
`Assets/shaders/versions/`). Only include paths, namespaces and shader paths were changed, plus the
`Primitive::halfExtents` → `data` rename. Their bugs are deliberately kept.

## What is identical for every version
- Scene (reloaded fresh for each version), camera poses, render size, volume resolution and bounds,
  max steps / max distance / surface distance, warmup and measured frame counts, bake repeats.
- Measurement: GPU timestamps around the raymarch pass only, and around `Bake`. VRAM is the
  `GL_NVX_gpu_memory_info` used-memory delta against a reading taken with no raymarcher loaded.
  Only one version exists at a time, so their memory never overlaps.

## Running
- In the editor: open **Version Benchmark**, pick the scene and versions, press **Run comparison**.
- From the command line (working directory = `RawEngine_v2`):
  `RawEngine.exe --version-benchmark Benchmark_common` — add `--quick` for a short smoke test.
  The engine closes when the run ends.

Results go to `Assets/Data/comparison/<scene>_<date>/`: `results.csv` (same format as the Benchmark
Tool), `meta.json` (git hash, GPU, all settings, poses) and a `.ppm` screenshot per version and pose.

## Scenes and poses
- `Benchmark_common`: every primitive type except Plane and Terrain, all rotations 0. Versions before
  63bc651 read rotations in degrees as radians, so non-zero rotations would differ between versions.
- `Terrain_test`: v0 and v1 have no terrain and are skipped automatically.
- Poses live in `<scene>.poses.json` here. Use **Capture camera as pose** and **Save poses** to edit them.
  Without a poses file the scene's own camera transform is used.
