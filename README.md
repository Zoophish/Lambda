# Lambda
A high performance physically-based renderer written in C++.

## Features
- Triangulated mesh geometry
- Graph based shading language
- Geometry instancing
- Many lights sampling
- PBRT-style unbiased spectral rendering
- Numerous BSDF models: Lambertian, Fresnel, specular, mix
- Microfacet BSDFs: Oren-Nayar, Beckmann-Spizzichino, Towbridge-Reitz/GGX
- Efficient rendering of participating media
- Path integrator with multiple importance sampling
- Pass Rendering: direct lighting, normals, depth, albedo, arbitrary textures
- Denoising via Intel Open Image Denoise
- Mesh lights/portals, infinite/environment lights, spotlights, point lights
- Pinhole, thin lens and spherical/equirectangular camera models
- Arbitrary camera aperture shapes
- Quasi-random sequence sampling with blue noise dithering
- Simple 3D asset importer via Assimp
- Texture encoding optimisation
- Multithreaded rendering
- Embree raytracing acceleration
- SSE support accross the program

![Transparency in materials (leaves).](https://github.com/Zoophish/Lambda/blob/master/repo_resources/lucyinnature.png)

![Denosing example](https://github.com/Zoophish/Lambda/blob/master/repo_resources/cornell_outside.jpg)

![Passes example](https://github.com/Zoophish/Lambda/blob/master/repo_resources/passes.jpg)

![Blackbody lamps and MixBSDF](https://github.com/Zoophish/Lambda/blob/master/repo_resources/lucy_blackbody.png)

![Volumetric Scattering](https://github.com/Zoophish/Lambda/blob/master/repo_resources/volumetric_dragon.png)

![Ocean render test @400spp](https://github.com/Zoophish/Lambda/blob/master/repo_resources/ocean_2.png)

## Third Party Dependencies
- Embree3 (https://www.embree.org/)
- Assimp (http://www.assimp.org/)
- Open Image Denoise (https://www.openimagedenoise.org/)

## Other Third Party Libraries
- stb_image & stb_image_write included in repository (https://github.com/nothings/stb)

 **Future goals**
 - Sobol' sampler with Owen scrambling
 - Path guiding
 - Packet-tracing on albedo and look-dev integrators
 - Tonemapping post-process
 - Utility integrators (depth, normal, albedo, etc)
 - Deep compositing AOVs
 - Bidirectional pathtracing integrator
 - Realistic camera model with lens and sensor profiles
 - Camera chromatic aberration
 - Parameterised Hair BSDF (Bitterli, Chiang - 2016)
 - Virtual-displacement mapping
 - Animated rendering
 - Massive-scene optimisation (occlusion of insignificant object groups, asset caching)
 - Light-pass rendering
 - Floating origin chunk instantiation for planetary scale rendering
 - Physical-atmosphere approximation with clouds