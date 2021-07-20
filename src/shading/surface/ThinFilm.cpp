// cosI is the cosine of the incident angle, that is, cos0 = dot(view angle, normal) // lambda is the wavelength of the incident light (e.g. lambda = 510 for green)
//float ThinFilmReflectance(float cos0, float lambda) {
//	const float thickness; // the thin film thickness
//	const float n0, n1, n2; // the refractive indices // compute the phase change term (constant)
//	const float d10 = (n1 > n0) ? 0 : PI; const float d12 = (n1 > n2) ? 0 : PI; const float delta = d10 + d12; // now, compute cos1, the cosine of the reflected angle
//	float sin1 = pow(n0 / n1, 2) * (1 - pow(cos0, 2)); if (sin1 > 1) return 1.0f; // total internal reflection
//	float cos1 = sqrt(1 - sin1); // compute cos2, the cosine of the final transmitted angle, i.e. cos(theta_2) // we need this angle for the Fresnel terms at the bottom interface
//	float sin2 = pow(n0 / n2, 2) * (1 - pow(cos0, 2)); if (sin2 > 1) return 1.0f; // total internal reflection
//	float cos2 = sqrt(1 - sin2); // get the reflection transmission amplitude Fresnel coefficients
//	float alpha_s = rs(n1, n0, cos1, cos0) * rs(n1, n2, cos1, cos2); // rho_10 * rho_12 (s-polarized)
//	float alpha_p = rp(n1, n0, cos1, cos0) * rp(n1, n2, cos1, cos2); // rho_10 * rho_12 (p-polarized)
//	float beta_s = ts(n0, n1, cos0, cos1) * ts(n1, n2, cos1, cos2); // tau_01 * tau_12 (s-polarized)
//	float beta_p = tp(n0, n1, cos0, cos1) * tp(n1, n2, cos1, cos2); // tau_01 * tau_12 (p-polarized) // compute the phase term (phi)
//	float phi = (2 * PI / lambda) * (2 * n1 * thickness * cos1) + delta; // finally, evaluate the transmitted intensity for the two possible polarizations
//	float ts = pow(beta_s) / (pow(alpha_s, 2) - 2 * alpha_s * cos(phi) + 1);
//	float tp = pow(beta_p) / (pow(alpha_p, 2) - 2 * alpha_p * cos(phi) + 1); // we need to take into account conservation of energy for transmission 
//	float beamRatio = (n2 * cos2) / (n0 * cos0); // calculate the average transmitted intensity (if you know the polarization distribution of your // light source, you should specify it here. if you don't, a 50%/50% average is generally used)
//	float t = beamRatio * (ts + tp) / 2; // and finally, derive the reflected intensity
//	return 1 - t;
//}