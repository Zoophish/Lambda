# Lambda
A high performance physically-based renderer written in C++ as a project in my spare time.

## Features
- Triangulated mesh geometry
- Graph based shading system
- Geometry instancing
- PBRT-style spectral rendering
- Fresnel, specular & mix BSDFs
- Microfacet BSDFs: Beckmann-Spizzichino, Towbridge-Reitz, GGX
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

![Microfacet (Beckmann) test @100spp](https://github.com/Zoophish/Lambda/blob/master/repo_resources/box_outdoors.png)

![Blackbody lamps and MixBSDF](https://github.com/Zoophish/Lambda/blob/master/repo_resources/lucy_blackbody.png)

![Ocean render test @400spp](https://github.com/Zoophish/Lambda/blob/master/repo_resources/ocean_2.png)

![Example Render of the Lucy statue using FresnelBSDF and Oren-NayarBRDF](https://github.com/Zoophish/Lambda/blob/master/repo_resources/glass_lucy_2.png)

## Third Party Dependencies
- Embree3 (https://www.embree.org/)
- Assimp (http://www.assimp.org/)

## Other Third Party Libraries
- stb_image & stb_image_write included in repository (https://github.com/nothings/stb)

 **Future goals**
 - Sobol' sampler
 - Light portals (projected)
 - Spatial importance sampling of lights
 - Packet-tracing
 - Tonemapping post-process
 - Utility integrators (depth, normal, albedo, etc)
 - Bidirectional path-integrator
 - Realistic camera model with lens and sensor profiles
 - Camera chromatic aberration
 - Infinite plane geommetry
 - Parameterised Hair BSDF (Bitterli, Chiang - 2016)
 - Virtual-displacement mapping
 - Animated rendering
 - Massive-scene optimisation (LOD, occlusion of insignificant object groups)
 - Light-pass rendering
 - Scene-marching, planetary scale rendering
 - Physical-atmosphere approximation with clouds