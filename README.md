# Lambda
A high performance physically-based renderer written in C++ as a project in my spare time.

![Microfacet (Beckmann) test @100spp](https://github.com/Zoophish/Lambda/blob/master/repo_resources/box_outdoors.png)

![Example Render of the Lucy statue using FresnelBSDF and Oren-NayarBRDF](https://github.com/Zoophish/Lambda/blob/master/repo_resources/glass_lucy_2.png)

![Ocean render test @400spp](https://github.com/Zoophish/Lambda/blob/master/repo_resources/ocean_2.png)

## Dependencies
- Embree3 (https://www.embree.org/) (Requires SSE2)
- Assimp (http://www.assimp.org/)

## Roadmap of Features
*2019 Q4*
**Priority**
 - [x] Path-integrator with multiple importance sampling
 - [x] Improved offline render-coordinator (with multithreading)
 - [ ] Adaptive pixel sampling
 - [ ] Interactive render application (for debugging)
 - [ ] Denoiser
 - [x] Thin lens camera model
 - [x] Spherical camera model
 - [x] Importance-sampled image/polygon aperture
 - [ ] Improved import system and scene structure
 - [x] Triangulated mesh geometry
 - [x] Instanced geometry
 - [x] Transform
 - [ ] Hair geometry
 - [x] Sample scrambler with dithering (blue-noise)
 - [x] Mesh lights
 - [x] Environment lights
 - [x] Light portals (mesh-based)
 - [ ] Microfacet BSDFs
 - [x] Oren-Nayar BRDF
 - [x] Specular BSDF
 - [x] Fresnel BSDF
 - [ ] Mix BSDF
 - [ ] Participating Media (homogenous + heterogenous)

**Non-Priority**
 - [ ] Sobol' sampler
 - [ ] Light portals (projected)
 - [ ] Spatial importance sampling of lights
 - [ ] Packet-tracing
 - [ ] Tonemapping post-processor
 - [ ] Utility integrators (depth, normal, albedo, etc)
 - [ ] Bidirectional path-integrator
 - [ ] Realistic camera model with lens and sensor profiles
 - [ ] Camera chromatic aberration
 - [ ] Infinite plane geommetry
 - [ ] Parameterised Hair BSDF (Bitterli, Chiang - 2016)

 **Concept**
 - Virtual-displacement mapping
 - Animated rendering
 - Massive-scene optimisation (LOD, occlusion of insignificant object groups)
 - Light-pass rendering
 - Scene-marching, planetary scale rendering
 - Physical-atmosphere approximation with clouds