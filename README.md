# Lambda
A high performance physically-based renderer written in C++ as a project in my spare time.
![Example Render of the Lucy statue using FresnelBSDF and Oren-NayarBRDF](https://github.com/Zoophish/Lambda/blob/master/repo_resources/glass_lucy.png)

## Roadmap of Features
*2019 Q4*
**Priority**
 - [x] Path-integrator with multiple importance sampling
 - [x] Offline render-coordinator (with multithreading)
 - [ ] Interactive render application (for debugging)
 - [x] Thin lens camera model
 - [x] Spherical camera model
 - [x] Importance-sampled image/polygon aperture
 - [ ] Improved import system and scene structure
 - [ ] Tonemapping post-processor
 - [x] Triangulated mesh geometry
 - [ ] Instanced geometry
 - [ ] Transform
 - [ ] Hair geometry
 - [x] Mesh lights
 - [x] Environment lights
 - [x] Light portals (mesh-based)
 - [x] Microfacet BSDFs
 - [x] Oren-Nayar BRDF
 - [x] Specular BSDF
 - [x] Fresnel BSDF
 - [ ] Mix BSDF
 - [ ] Participating Media (homogenous + heterogenous)
 - [ ] Virtual-displacement mapping

**Non-Priority**
 - [ ] Light portals (projected)
 - [ ] Packet-tracing
 - [ ] Bidirectional path-integrator
 - [ ] Realistic camera model with lens and sensor profiles
 - [ ] Camera chromatic aberration
