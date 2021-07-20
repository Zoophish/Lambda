# Lambda
A high performance physically-based renderer.

## Overview
**Geometry**  
    Triangle primitives and meshes are currently supported, however the primitive options are extensible via Embree.
    Instance proxies and objects allow scenes to contain large amounts of geometry with little memory usage.

**Materials & Shading**  
    Materials are simple objects containing surface and volumetric properties that are driven by node networks. These are executed in a fast virtual node machine and do not need compiling, meaning edits take place immediately in real time.    
    - Standard BSDFs: Lambertian, transparent/translucent, specular reflectionand transmission.    
    - PBR BSDFs: Fresnel, GGX, Oren-Nayar, Beckmann-Spizzichino, etc.    
    - PBR volumetric shading models.    
    - Standard scalar and vector mathematical operators are supported.     
    - Image textures    
    - Procedural textures: Perlin, value, Voronoi, etc noise types - 2D & 3Dsupport and octave stacking.         

**Rendering**  
    Rendering uses an unbiased Monte Carlo path-tracing implementation:  
        - PBRT-style spectral path-tracing  
        - Multiple importance sampling  
        - Equiangular medium sampling  
        - Light tree sampling for better efficiency and many lights support  
    Pass rendering is supported for albedo, direct lighting, normals, depth etc. Custom pass rendering is also supported.    
    - Environment/infinite lights   
    - Point lights  
    - Spotlights    
    - Mesh lights 

**Camera**  
    - Thin lens camera: DOF with custom bokeh/aperture shape.   
    - Spherical camera    

**Concurrency**   
    Multithreaded rendering.    
    Progressive rendering.
    Performance-critical code is optimised with SIMD vectorization.    

![Glass rendering](https://github.com/Zoophish/Lambda/blob/master/repo_resources/glass.jpg)

![Glass rendering](https://github.com/Zoophish/Lambda/blob/master/repo_resources/glass2.jpg)

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

## Third Party Libraries
- stb_image & stb_image_write included in repository (https://github.com/nothings/stb)

 **Future goals**  
 - Globe rendering & precision techniques for planetary scale rendering
 - Sobol' sampler with Owen scrambling
 - Path guiding
 - Fast raytraced look-dev rendering
 - Better post processing options
 - Deep compositing AOVs
 - Bidirectional pathtracing integrator
 - Realistic camera model with lens and sensor profiles
 - Parameterised Hair BSDF (Bitterli, Chiang - 2016)
 - Virtual-displacement mapping
 - Animated transforms & deformables
 - Light-pass rendering
 - Physical-atmosphere with clouds