#pragma once
#include <lighting/MeshLight.h>
#include "ManyLightSampler.h"

LAMBDA_BEGIN

static inline bool HasInfiniteBounds(const Bounds &_bounds) {
	const bool c1 = std::isinf(_bounds.min.x) || std::isinf(_bounds.min.y) || std::isinf(_bounds.min.z);
	const bool c2 = std::isinf(_bounds.max.x) || std::isinf(_bounds.max.y) || std::isinf(_bounds.max.z);
	return c1 || c2;
}

ManyLightSampler::ManyLightSampler(const Real _threshold) : threshold(_threshold), root(nullptr) {}

ManyLightSampler::ManyLightSampler(const Scene &_scene, const Real _threshold) : LightSampler(&_scene) {
	threshold = _threshold;
}

void ManyLightSampler::Commit() {
	std::cout << std::endl << "Building light tree...";
	InitLights(scene->lights);
	RecursiveBuild(root.get());
	std::cout << std::endl << "Done.";
}

Light *ManyLightSampler::Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const {
	return PickLight(_event, _sampler.Get1D(), root.get(), _pdf);
}

ManyLightSampler::OrientationCone ManyLightSampler::OrientationCone::MakeCone(const Vec3 &_axis, const Real _thetaO, const Real thetaE) {
	return { _axis, 0, PI / (Real)2 };
}

ManyLightSampler::OrientationCone ManyLightSampler::OrientationCone::Union(const OrientationCone &_a, const OrientationCone &_b) {
	if(_b.thetaO > _a.thetaO) std::swap(*const_cast<OrientationCone*>(&_a), *const_cast<OrientationCone *>(&_b));
	const Real thetaE = std::max(_a.thetaE, _b.thetaE);
	const Real thetaD = std::acos(maths::Clamp(maths::Dot(_a.axis, _b.axis), (Real)0, (Real)1));	//Angle between two axes
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
	const Real thetaW2 = std::min(_cone.thetaO + _cone.thetaE, (Real)PI) * (Real)2;
	const Real b = thetaW2 * sinThetaO - std::cos(_cone.thetaO - thetaW2) - (Real)2 * _cone.thetaO * sinThetaO + cosThetaO;
	return a + PI * (Real).5 * b;
}

/*
	Cost of proposed split into two sub-nodes based on surface area of AABB and the size of the orientation bounds.
*/
Real ManyLightSampler::SAOH(LightNode &_P, const Bucket &_R, const Bucket &_L, const unsigned _axis) {
	const Vec3 box = _P.bounds.max - _P.bounds.min;
	const Real lenMax = std::max(std::max(box.x, box.y), box.z);
	const Real Kr = lenMax / box[_axis];	//Regularization factor
	const Real m = _P.bounds.SurfaceArea() * MOmega(_P.orientationCone);
	const Real mL = _L.totalPower * _L.bounds.SurfaceArea() * MOmega(_L.cone);
	const Real mR = _R.totalPower * _R.bounds.SurfaceArea() * MOmega(_R.cone);
	return Kr * (mL + mR) / m;
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
	return 0;
}

void ManyLightSampler::InitLeaf(LightNode *_node) {
	std::unique_ptr<Real[]> d(new Real[_node->numLights]);
	for (unsigned i = 0; i < _node->numLights; ++i) {
		d[i] = lights[_node->firstLightIndex + i]->Power();
	}
	leafDistributions.insert({ _node->firstLightIndex, Distribution::Piecewise1D(&d[0], _node->numLights) });
	_node->children[0] = _node->children[1] = nullptr;
}

/*
	Recursive SAOH split. Inital _node must contain all lights.
*/
void ManyLightSampler::RecursiveBuild(LightNode *_node) {

	_node->bounds = lights[_node->firstLightIndex]->GetBounds();
	_node->orientationCone = OrientationCone::MakeCone(lights[_node->firstLightIndex]->GetDirection());
	for (unsigned i = 0; i < _node->numLights; ++i) {
		_node->bounds = maths::Union(_node->bounds, lights[i + _node->firstLightIndex]->GetBounds());
		_node->orientationCone = OrientationCone::Union(_node->orientationCone, OrientationCone::MakeCone(lights[i + _node->firstLightIndex]->GetDirection()));
	}
	
	constexpr unsigned numBuckets = 12;

	//Determine split axis and important lengths
	const Vec3 axes = _node->bounds.max - _node->bounds.min;
	const unsigned splitAxis = axes.x > axes.y && axes.x > axes.z ? 0 : (axes.y > axes.x && axes.y > axes.z) ? 1 : 2;

	//Initialise buckets
	std::unique_ptr<Bucket[]> buckets(new Bucket[numBuckets]);
	std::unique_ptr<bool[]> bucketInits(new bool[numBuckets]);
	std::fill(&bucketInits[0], &bucketInits[numBuckets], false);
	for (unsigned i = _node->firstLightIndex; i < _node->firstLightIndex + _node->numLights; ++i) {
		const Light *light = lights[i];
		unsigned b = (Real)numBuckets * _node->bounds.Offset(light->GetBounds().Center())[splitAxis];
		if (b == numBuckets) b = numBuckets - 1;
		if (!bucketInits[b]) {
			buckets[b].totalPower = light->Power();
			buckets[b].bounds = light->GetBounds();
			buckets[b].cone = OrientationCone::MakeCone(light->GetDirection());
			bucketInits[b] = true;
		}
		else {
			buckets[b].totalPower += light->Power();
			buckets[b].bounds = maths::Union(buckets[b].bounds, light->GetBounds());
			buckets[b].cone = OrientationCone::Union(buckets[b].cone, OrientationCone::MakeCone(light->GetDirection()));
		}
	}
	
	//Compute split costs and find minimum cost split
	constexpr unsigned numCosts = numBuckets - 1;
	Real minCost = INFINITY;
	unsigned minCostBucket;
	Bucket leftBucket, rightBucket;
	for (unsigned i = 0; i < numCosts; ++i) {
		Bucket bL = { 0, buckets[0].bounds, buckets[0].cone };
		Bucket bR = { 0, buckets[i + 1].bounds, buckets[i + 1].cone };
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
	if (_node->numLights > 1 && minCost < _node->totalPower) {	//Split

		auto pred = [&](const Light *l) {
			unsigned b = _node->bounds.Offset(l->GetBounds().Center())[splitAxis] * (Real)numBuckets;
			if (b == numBuckets) b--;
			return b <= minCostBucket;
		};

		Light **it = std::partition(&lights[_node->firstLightIndex], &lights[_node->firstLightIndex + _node->numLights - 1] + 1, pred);	//Range not inclusive of last, hence +1
		const unsigned pivot = it - &lights[0];
		//Set the light primitive range
		const unsigned leftNumLights = pivot - _node->firstLightIndex;
		const unsigned rightNumLights = _node->numLights - leftNumLights;

		if (leftNumLights != 0 && rightNumLights != 0) {
			_node->children[0] = new LightNode {	//Left
				{nullptr, nullptr},
				_node->firstLightIndex,
				leftNumLights,
				leftBucket.bounds,
				leftBucket.cone,
				leftBucket.totalPower,
				0	//Variance (to do)
			};
			_node->children[1] = new LightNode {	//Right
				{nullptr, nullptr},
				pivot,
				rightNumLights,
				rightBucket.bounds,
				rightBucket.cone,
				rightBucket.totalPower,
				0	//Variance (to do)
			};
			RecursiveBuild(_node->children[0]);
			RecursiveBuild(_node->children[1]);
		}
		else InitLeaf(_node);
	}
	else InitLeaf(_node);	//Leaf light distribution and terminate recursion (TODO add variance)
}

void ManyLightSampler::InitLights(const std::vector<Light *> &_lights) {
	std::list<Light *> lightList;
	std::copy(_lights.begin(), _lights.end(), std::back_inserter(lightList));
	//Filter out infinite lights
	auto l = lightList.begin();
	while (l != lightList.end()) {
		if (HasInfiniteBounds((*l)->GetBounds())) {
			infiniteLights.push_back(*l);
			l = lightList.erase(l);
		}
		else ++l;
	}

	//Filter out mesh lights and convert to triangle lights
	l = lightList.begin();
	while (l != lightList.end()) {
		MeshLight *meshLight = dynamic_cast<MeshLight *>(*l);
		if (meshLight) {
			const TriangleMesh *mesh = &meshLight->GetMesh();
			//triangleLights.reserve(triangleLights.size() + mesh->trianglesSize);
			for (size_t i = 0; i < mesh->trianglesSize; ++i) {
				triangleLights.push_back(TriangleLight(meshLight, i));
			}
			lightList.erase(l++);
		}
		else l++;
	}

	//Add the triangle lights ptrs to lights
	for (unsigned i = 0; i < triangleLights.size(); ++i) lights.push_back(&triangleLights[i]);

	//Add remaining from _lights to lights vector
	//for (auto l : lightList) lights.push_back(l);

	//Initialise root
	root.reset(new LightNode);
	root->numLights = lights.size();
	root->firstLightIndex = 0;
	root->bounds = lights[0]->GetBounds();
	root->totalPower = 0;
	root->clusterVariance = 0;
	root->orientationCone = OrientationCone::MakeCone(lights[0]->GetDirection());
	root->children[0] = root->children[1] = nullptr;
	for (unsigned i = 0; i < root->numLights; ++i) {
		root->bounds = maths::Union(root->bounds, lights[i]->GetBounds());
		root->orientationCone = OrientationCone::Union(root->orientationCone, OrientationCone::MakeCone(lights[i]->GetDirection()));
		root->totalPower += lights[i]->Power();
	}
}

Real ManyLightSampler::ImportanceMeasure(const ScatterEvent &_event, LightNode *_node) {	//Optimise
	const Real E = _node->totalPower;
	Vec3 delta = _node->bounds.Center() - _event.hit->point;
	const Real d2 = maths::Dot(delta, delta);
	const Real d = std::sqrt(d2);
	delta /= d;
	const Real thetaI = std::acos(maths::Dot(delta, _event.hit->normalS));
	const Real theta = std::acos(maths::Dot(delta, _node->orientationCone.axis));
	const Vec3 box = _node->bounds.max - _node->bounds.min;
	const Real thetaU = std::atan((box.Magnitude() * .5) / d);
	const Real thetaDash = std::max(theta - _node->orientationCone.thetaO - thetaU, (Real)0);
	return (std::cos(std::max(thetaI - thetaU, (Real)0)) * E) / (d2) * (thetaDash < _node->orientationCone.thetaE ? std::cos(thetaDash) : 0);
}

Real ManyLightSampler::SplitMeasure(LightNode *_node) const {
	return 0;
}

Light *ManyLightSampler::PickLight(const ScatterEvent &_event, Real _epsilon, LightNode *_node, Real *_pdf) const {
	if (_node->IsLeaf()) {
		Real lpdf = 1;
		const unsigned i = leafDistributions.at(_node->firstLightIndex).SampleDiscrete(_epsilon, &lpdf);	//Sample light distribution of cluster
		*_pdf *= lpdf;
		return lights[_node->firstLightIndex + i];
	}
	else {
		const Real IL = ImportanceMeasure(_event, _node->children[0]);
		const Real IR = ImportanceMeasure(_event, _node->children[1]);
		const Real PL = IL / (IL + IR);
		const Real PR = 1 - PL;
		if (_epsilon < PL) {
			//_epsilon = _epsilon * (IL + IR) / IL;
			_epsilon /= PL;
			*_pdf *= PL;
			return PickLight(_event, _epsilon, _node->children[0], _pdf);
		}
		else {
			//_epsilon = (_epsilon * (IL + IR) - IL) / IR;
			_epsilon = (_epsilon - PL) / PR;
			*_pdf *= PR;
			return PickLight(_event, _epsilon, _node->children[1], _pdf);
		}
	}
}

LAMBDA_END