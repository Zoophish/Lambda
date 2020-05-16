# Lambda
A high performance physically-based renderer written in C++ as a project in my spare time.

## Features
- Triangulated mesh geometry
- Graph based shading system
- Geometry instancing
- Many lights sampling
- PBRT-style unbiased spectral rendering
- Fresnel, specular & mix BSDFs
- Microfacet BSDFs: Beckmann-Spizzichino, Towbridge-Reitz/GGX
- Volumetric rendering
- Path integrator with multiple importance sampling
- Utlilty integrators: direct lighting, normals, depth
- Mesh lights, infinite/environment lights, mesh portals
- Pinhole, thin lens and spherical/equirectangular camera models
- Arbitrary camera aperture shapes
- Quasi-random sequence sampling with blue noise dithering
- Simple 3D asset importer
- Scanline, Hilbert and Morton texture encoding
- Multithreaded rendering
- SSE support accross the program

![Transparency in materials (leaves).](https://github.com/Zoophish/Lambda/blob/master/repo_resources/lucyinnature.png)

![Blackbody lamps and MixBSDF](https://github.com/Zoophish/Lambda/blob/master/repo_resources/lucy_blackbody.png)

![Volumetric Scattering](https://github.com/Zoophish/Lambda/blob/master/repo_resources/volumetric_dragon.png)

![Ocean render test @400spp](https://github.com/Zoophish/Lambda/blob/master/repo_resources/ocean_2.png)

## Third Party Dependencies
- Embree3 (https://www.embree.org/)
- Assimp (http://www.assimp.org/)

## Other Third Party Libraries
- stb_image & stb_image_write included in repository (https://github.com/nothings/stb)

 **Future goals**
 - Sobol' sampler
 - Light portals (projected)
 - Spatial importance sampling of lights
 - Packet-tracing on albedo and look-dev integrators
 - Tonemapping post-process
 - Utility integrators (depth, normal, albedo, etc)
 - Bidirectional path-integrator
 - Realistic camera model with lens and sensor profiles
 - Camera chromatic aberration
 - Parameterised Hair BSDF (Bitterli, Chiang - 2016)
 - Virtual-displacement mapping
 - Animated rendering
 - Massive-scene optimisation (LOD, occlusion of insignificant object groups)
 - Light-pass rendering
 - Floating origin chunk instantiation for planetary scale rendering
 - Physical-atmosphere approximation with clouds