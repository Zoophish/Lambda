#pragma once
#include <lighting/MeshLight.h>
#include "ManyLightSampler.h"

LAMBDA_BEGIN

static inline bool HasInfiniteBounds(const Bounds &_bounds) {
	const bool c1 = std::isinf(_bounds.min.x) || std::isinf(_bounds.min.y) || std::isinf(_bounds.min.z);
	const bool c2 = std::isinf(_bounds.max.x) || std::isinf(_bounds.max.y) || std::isinf(_bounds.max.z);
	return c1 || c2;
}

ManyLightSampler::ManyLightSampler(const Real _threshold) : threshold(_threshold) {}

ManyLightSampler::ManyLightSampler(const Scene &_scene, const Real _threshold) : LightSampler(&_scene) {
	InitLights(_scene.lights);
	threshold = _threshold;
}

void ManyLightSampler::Commit() {
	std::cout << std::endl << "Building light tree...";
	RecursiveBuild(root);
	std::cout << std::endl << "Done.";
}

Light *ManyLightSampler::Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const {
	return PickLight(_event, _sampler.Get1D(), root, _pdf);
}

ManyLightSampler::OrientationCone ManyLightSampler::OrientationCone::Union(const OrientationCone &_a, const OrientationCone &_b) {
	if(_b.thetaO > _a.thetaO) std::swap(*const_cast<OrientationCone*>(&_a), *const_cast<OrientationCone *>(&_b));
	const Real thetaE = std::max(_a.thetaE, _b.thetaE);
	const Real thetaD = std::acos(maths::Dot(_a.axis, _b.axis));	//Angle between two axes
	if (std::min(thetaD + _b.thetaO, (Real)PI) <= _a.thetaO)
		return { _a.axis, _a.thetaO, thetaE };	//_a already conatains _b
	else {
		const Real thetaO = (_a.thetaO + thetaD + _b.thetaO) * (Real).5;	//New cone over _a and _b; we divide by 2 because the axis sits in the middle of the arc
		if (thetaO >= PI)
			return { _a.axis, PI, thetaE };	//We don't need a new axis since the bounds cover 180° (which is the maximum)
		const Real thetaR = thetaO - _a.thetaO;	//Rotate _a towards _b by the difference in angle
		const Vec3 axis = maths::Rotate(_a.axis, maths::Cross(_a.axis, _b.axis), thetaR);	//Rotate _a towards _b along orthogonal axis using Rodrigues' formula
		return { axis, thetaO, thetaE };
	}
}

ManyLightSampler::LightNode::~LightNode() {
	if (children[0]) delete children[0];
	if (children[1]) delete children[1];
}

Real ManyLightSampler::MOmega(const OrientationCone &_cone) {
	const Real cosThetaO = std::cos(_cone.thetaO);
	const Real sinThetaO = std::sqrt(1 - cosThetaO * cosThetaO);
	const Real a = PI2 * ((Real)1 - cosThetaO);
	const Real thetaW2 = std::min(_cone.thetaO + _cone.thetaE, (Real)PI) * 2;
	const Real b = thetaW2 * sinThetaO - std::cos(_cone.thetaO - thetaW2) - 2 * _cone.thetaO * sinThetaO + cosThetaO;
	return a + PI * .5 * b;
}

/*
	Cost of proposed split into two sub-nodes based on surface area of AABB and the size of the orientation bounds.
*/
Real ManyLightSampler::SAOH(LightNode &_P, const Bucket &_R, const Bucket &_L, const unsigned _axis) {
	const Vec3 bLen = _P.bounds.max - _P.bounds.min;
	const Real lenMax = std::max(std::max(bLen.x, bLen.y), bLen.z);
	const Real Kr = lenMax / bLen[_axis];
	const Real m = _P.bounds.SurfaceArea() * MOmega(_P.orientationCone);
	const Real mL = _L.totalPower * _L.bounds.SurfaceArea() * MOmega(_L.cone);
	const Real mR = _R.totalPower * _R.bounds.SurfaceArea() * MOmega(_R.cone);
	return Kr * ((mL + mR) / m);
}

Real ManyLightSampler::PowerVariance(LightNode *_node) const {
	std::unique_ptr<Real[]> powers(new Real[_node->numLights]);
	Real mean = 0;
	for (unsigned i = 0; i < _node->numLights; ++i) {
		powers[i] = lights[i + _node->firstLightIndex]->Power();
		mean += powers[i];
	}
	mean /= (Real)_node->numLights;
	Real sum = 0;
	for (unsigned i = 0; i < _node->numLights; ++i) {
		const Real term = powers[i] - mean;
		sum += term * term;
	}
	return sum / (Real)(_node->numLights - 1);
}

Real ManyLightSampler::GeometricVariance(LightNode *_node) const {

}

/*
	Recursive split. Inital _node must contain all lights.
*/
void ManyLightSampler::RecursiveBuild(LightNode *_node) {
	constexpr unsigned numBuckets = 12;

	//Determine split axis and important lengths
	const Vec3 axes = _node->bounds.max - _node->bounds.min;
	const unsigned splitAxis = axes.x > axes.y && axes.x > axes.z ? 0 : (axes.y > axes.x && axes.y > axes.z) ? 1 : 2;

	//Initialise buckets
	Bucket buckets[numBuckets];
	for (unsigned i = _node->firstLightIndex; i < _node->firstLightIndex + _node->numLights; ++i) {
		const Light *light = lights[i];
		unsigned b = numBuckets * _node->bounds.Offset(light->GetBounds().Center())[splitAxis];
		if (b == numBuckets) b--;
		buckets[b].totalPower += light->Power();
		buckets[b].cone = OrientationCone::Union(buckets[b].cone, { light->GetDirection(), 0, PI });
		buckets[b].bounds = maths::Union(buckets[b].bounds, light->GetBounds());
	}
	
	//Compute split costs and find minimum cost split
	constexpr unsigned numCosts = numBuckets - 1;
	Real minCost = INFINITY;
	unsigned minCostBucket;
	Bucket leftBucket, rightBucket;
	for (unsigned i = 0; i < numCosts; ++i) {
		Bucket bL, bR;
		for (unsigned j = 0; j <= i; ++j) {	//Left bucket proposal
			bL.bounds = maths::Union(bL.bounds, buckets[j].bounds);
			bL.cone = OrientationCone::Union(bL.cone, buckets[j].cone);
			bL.totalPower += buckets[j].totalPower;
		}
		for (unsigned j = i + 1; j < numBuckets; ++j) {	//Right bucket proposal
			bR.bounds = maths::Union(bR.bounds, buckets[j].bounds);
			bR.cone = OrientationCone::Union(bR.cone, buckets[j].cone);
			bR.totalPower += buckets[j].totalPower;
		}
		const Real cost = SAOH(*_node, bR, bL, splitAxis);
		if (cost < minCost) {	//Set minimum cost 
			minCost = cost;
			leftBucket = bL;	//Store best buckets
			rightBucket = bR;
			minCostBucket = i;
		}
	}

	//Create leaf or split node
	if (_node->numLights > 1 || minCost < _node->totalPower) {	//Split
		LightNode *left = new LightNode;
		LightNode *right = new LightNode;
		_node->children[0] = left;
		_node->children[1] = right;
		left->bounds = leftBucket.bounds;
		left->orientationCone = leftBucket.cone;
		left->totalPower = leftBucket.totalPower;
		right->bounds = rightBucket.bounds;
		right->orientationCone = rightBucket.cone;
		right->totalPower = rightBucket.totalPower;
		unsigned pivot = 0;
		for (unsigned i = _node->firstLightIndex; i < _node->firstLightIndex + _node->numLights; ++i) {	//Determine what children are in left or right
			const Real pos = lights[i]->GetBounds().Center()[splitAxis];	//Light position on split axis
			auto pred = [&](const Light *l) {
				unsigned b = _node->bounds.Offset(l->GetBounds().Center())[splitAxis] * numBuckets;
				if (b == numBuckets) b--;
				const bool t = b <= minCostBucket;
				if (t) pivot++;
				return t;
			};
			std::partition(lights.begin() + _node->firstLightIndex, lights.begin() + _node->firstLightIndex + _node->numLights - 1, pred);
		}
		//Set the light 
		left->numLights = pivot;
		left->firstLightIndex = _node->firstLightIndex;
		right->numLights = _node->numLights - pivot - 1;
		right->firstLightIndex = pivot + 1;

		//Split left and right since they are not leaves yet
		RecursiveBuild(left);
		RecursiveBuild(right);

		_node->powerVariance = PowerVariance(_node);	//Calculate power variance
	}
	else {	//Leaf light distribution
		std::unique_ptr<Real[]> d(new Real[_node->numLights]);
		for (unsigned i = 0; i < _node->numLights; ++i) {
			d[i] = lights[_node->firstLightIndex + i]->Power();
		}
		leafDistributions.insert({ _node->firstLightIndex, Distribution::Piecewise1D(d.get(), _node->numLights) });
	}
}

void ManyLightSampler::InitLights(std::vector<Light *> _lights) {
	//Filter out infinite lights
	for (auto l = _lights.begin(); l != _lights.end(); ++l) {
		if (HasInfiniteBounds((*l)->GetBounds())) {
			infiniteLights.push_back(*l);
			_lights.erase(l);
		}
	}

	//Filter out mesh lights and convert to triangle lights
	for (auto l = _lights.begin(); l != _lights.end(); ++l) {
		MeshLight *meshLight = dynamic_cast<MeshLight *>(*l);
		if (meshLight) {
			const TriangleMesh *mesh = &meshLight->GetMesh();
			triangleLights.reserve(triangleLights.size() + mesh->trianglesSize);
			_lights.erase(l);
			for (size_t i = 0; i < mesh->trianglesSize; ++i) {
				TriangleLight triLight(meshLight, i);
				/*
				Needs review:
				Some triangles might have have zero power coming out of them so inclusion in the light tree is redundant
				Emission could be animated? rely on artist to correctly organise this?
				*/
				//if (triLight.Power() > 0) {
					triangleLights.push_back(triLight);
					lights.push_back(&triLight);
				//}
			}
		}
	}

	//Add remaining from _lights to lights vector
	for (auto l : _lights) lights.push_back(l);

	//Initialise root
	root = new LightNode;
	root->numLights = lights.size();
	root->firstLightIndex = 0;
	root->children[0] = root->children[1] = nullptr;
	for (unsigned i = 0; i < root->numLights; ++i) {
		root->bounds = maths::Union(root->bounds, lights[i]->GetBounds());
		root->orientationCone = OrientationCone::Union(root->orientationCone, { lights[i]->GetDirection(), 0, PI });
		root->totalPower += lights[i]->Power();
	}
}

Real ManyLightSampler::ImportanceMeasure(const ScatterEvent &_event, LightNode *_node) {	//Optimise
	const Real E = _node->totalPower;
	const Vec3 delta = _node->bounds.Center() - _event.hit->point;
	const Real d2 = maths::Dot(delta, delta);
	const Real bsdfApprox = maths::Dot(delta, _event.hit->normalS);	//Does not approx the exact bsdf, which could be very useful!
	const Real theta = std::acos(maths::Dot(delta, _node->orientationCone.axis));
	const Vec3 box = _node->bounds.max - _node->bounds.min;
	const Real thetaU = std::atan((box.Magnitude() * .5) / std::sqrt(d2));
	const Real thetaDash = std::max(theta - _node->orientationCone.thetaO - thetaU, (Real)0);
	return (bsdfApprox * E) / d2 * (thetaDash < _node->orientationCone.thetaE ? std::cos(thetaDash) : 0);
}

Real ManyLightSampler::SplitMeasure(LightNode *_node) const {
	
}

Light *ManyLightSampler::PickLight(const ScatterEvent &_event, Real _epsilon, LightNode *_node, Real *_pdf) const {
	if (_node->IsLeaf()) {
		const unsigned i = leafDistributions.at(_node->firstLightIndex).SampleDiscrete(_epsilon, _pdf);	//Sample light distribution of cluster
		return lights[_node->firstLightIndex + i];
	}
	else {
		const Real IL = ImportanceMeasure(_event, _node->children[0]);
		const Real IR = ImportanceMeasure(_event, _node->children[1]);
		if (_epsilon < IL / (IL + IR)) {
			_epsilon = _epsilon * (IL + IR) / IL;
			return PickLight(_event, _epsilon, _node->children[0], _pdf);
		}
		else {
			_epsilon = (_epsilon * (IL + IR) - IL) / IR;
			return PickLight(_event, _epsilon, _node->children[1], _pdf);
		}
	}
}

LAMBDA_END