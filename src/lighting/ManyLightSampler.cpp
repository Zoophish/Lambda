#pragma once
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include "ManyLightSampler.h"

LAMBDA_BEGIN

ManyLightSampler::ManyLightSampler(const Real _splitThreshold, const bool _useSplits) :
	splitThreshold(_splitThreshold), useSplits(_useSplits), root(nullptr) {}

ManyLightSampler::ManyLightSampler(const Scene &_scene, const Real _splitThreshold, const bool _useSplits) : LightSampler(&_scene) {
	splitThreshold = _splitThreshold;
	useSplits = _useSplits;
}

Light *ManyLightSampler::Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const {
	Real epsilon = _sampler.Get1D();
	const Real pTree = treePower / (infPower + treePower);
	if (epsilon < pTree) {	//Sample tree
		epsilon /= pTree;
		*_pdf *= pTree;
		return PickLight(_event, _sampler.Get1D(), root.get(), _pdf);
	}
	*_pdf *= 1 - pTree;	//Sample infinite light (replace *= with =) ?
	return infiniteLight;
}

Real ManyLightSampler::Pdf(const ScatterEvent &_event, const Light *_light) const {
	if (_light == infiniteLight) return infPower / (infPower + treePower);
	auto tl = triangleLights.find({ _light, _event.hit->primId });	//If is a mesh light, it will replace pointer with triangle light's
	if (tl != triangleLights.end()) _light = &tl->second;
	const std::pair<LightNode *, unsigned> &nodeRef = lightNodeDistributionMap.at(_light);
	return leafDistributions.at(nodeRef.first->firstLightIndex).PDF(nodeRef.second) * RecursivePDF(_event, nodeRef.first);
}

void ManyLightSampler::Commit() {
	std::cout << std::endl << "Building light tree...";
	InitLights(scene->lights);
	treePower = 0;
	infPower = infiniteLight->Power() * .1;
	if(lights.size() > 0) RecursiveBuild(root.get());
	std::cout << std::endl << "Done.";
}

ManyLightSampler::OrientationCone ManyLightSampler::OrientationCone::MakeCone(const Vec3 &_axis, const Real _thetaO, const Real _thetaE) {
	return { _axis, _thetaO, _thetaE };
}

ManyLightSampler::OrientationCone ManyLightSampler::OrientationCone::Union(const OrientationCone &_a, const OrientationCone &_b) {
	if (_b.thetaO > _a.thetaO) std::swap(*const_cast<OrientationCone *>(&_a), *const_cast<OrientationCone *>(&_b));
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
	return PowerVariance(_node->firstLightIndex, _node->numLights);
}

Real ManyLightSampler::PowerVariance(const unsigned _firstLightIndex, const unsigned _numLights) const {
	std::unique_ptr<Real[]> powers(new Real[_numLights]);
	Real mean = 0;
	for (unsigned i = 0; i < _numLights; ++i) {
		powers[i] = lights[i + _firstLightIndex]->Power();
		mean += powers[i];
	}
	mean /= (Real)_numLights;
	Real sum = 0;
	for (unsigned i = 0; i < _numLights; ++i) {
		const Real term = powers[i] - mean;
		sum += term * term;
	}
	return sum / (Real)(_numLights - 1);
}

Real ManyLightSampler::GeometricVariance(LightNode *_node, const Vec3 &_point, Real *_mean) const {
	const Real radius = std::sqrt(_node->bounds.DiagonalLength() * (Real).5);
	const Real dist = (_node->bounds.Center() - _point).Magnitude();
	const Real a = dist - radius;
	const Real b = dist + radius;
	*_mean = (Real)1 / (a * b);
	const Real meanSquared = *_mean * *_mean;
	const Real a3 = a * a * a;
	const Real b3 = b * b * b;
	return (b3 - a3) / ((Real)3 * (b - a) * a3 * b3) - meanSquared;
}

void ManyLightSampler::InitLeaf(LightNode *_node) {
	std::unique_ptr<Real[]> d(new Real[_node->numLights]);
	for (unsigned i = 0; i < _node->numLights; ++i) {
		d[i] = lights[_node->firstLightIndex + i]->Power();
		lightNodeDistributionMap[lights[_node->firstLightIndex + i]] = { _node, i };	//Add it to light map
		treePower += d[i];	//Accumulate tree power
	}
	leafDistributions[_node->firstLightIndex] = Distribution::Piecewise1D(&d[0], _node->numLights);
	_node->children[0] = _node->children[1] = nullptr;
}

bool ManyLightSampler::SplitAxis(LightNode *_node, const unsigned _axis, Real *_minCost, unsigned *_bucketIndex, Bucket *_leftBucket, Bucket *_rightBucket) {
	constexpr unsigned numBuckets = 12;

	std::unique_ptr<Bucket[]> buckets(new Bucket[numBuckets]);	//To self: Doesn't have to be on heap
	std::unique_ptr<bool[]> bucketInits(new bool[numBuckets]);
	std::fill(&bucketInits[0], &bucketInits[numBuckets], false);
	for (unsigned i = _node->firstLightIndex; i < _node->firstLightIndex + _node->numLights; ++i) {
		const Light *light = lights[i];
		unsigned b = (Real)numBuckets * _node->bounds.Offset(light->GetBounds().Center())[_axis];
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
	bool cheaper = false;
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
		const Real cost = SAOH(*_node, bR, bL, _axis);
		if (cost < *_minCost) {	//Check if split cost beats the best cost
			*_minCost = cost;
			*_leftBucket = bL;	//Store best buckets
			*_rightBucket = bR;
			*_bucketIndex = i;
			cheaper = true;
		}
	}
	return cheaper;
}

void ManyLightSampler::RecursiveBuild(LightNode *_node) {

	_node->bounds = lights[_node->firstLightIndex]->GetBounds();
	_node->orientationCone = OrientationCone::MakeCone(lights[_node->firstLightIndex]->GetDirection());
	for (unsigned i = 0; i < _node->numLights; ++i) {
		_node->bounds = maths::Union(_node->bounds, lights[i + _node->firstLightIndex]->GetBounds());
		_node->orientationCone = OrientationCone::Union(_node->orientationCone, OrientationCone::MakeCone(lights[i + _node->firstLightIndex]->GetDirection()));
	}

	Bucket leftBucket, rightBucket;
	Real minCost = INFINITY;
	unsigned bucketIndex = 0;
	int splitAxis = -1;

	for (int i = 0; i < 3; ++i) {
		if (SplitAxis(_node, i, &minCost, &bucketIndex, &leftBucket, &rightBucket))
			splitAxis = i;
	}

	//Create leaf or split node
	if (_node->numLights > 1 && minCost < _node->totalPower) {	//Split

		auto pred = [&](const Light *l) {
			unsigned b = _node->bounds.Offset(l->GetBounds().Center())[splitAxis] * (Real)numBuckets;
			if (b == numBuckets) b--;
			return b <= bucketIndex;
		};

		Light **it = std::partition(&lights[_node->firstLightIndex], &lights[_node->firstLightIndex + _node->numLights - 1] + 1, pred);	//Range not inclusive of last, hence +1
		const unsigned pivot = it - &lights[0];
		//Set the light primitive range
		const unsigned leftNumLights = pivot - _node->firstLightIndex;
		const unsigned rightNumLights = _node->numLights - leftNumLights;

		if (leftNumLights != 0 && rightNumLights != 0) {
			_node->children[0] = new LightNode{	//Left
				{nullptr, nullptr},
				_node,
				_node->firstLightIndex,
				leftNumLights,
				leftBucket.bounds,
				leftBucket.cone,
				leftBucket.totalPower,
				PowerVariance(_node->firstLightIndex, leftNumLights)
			};
			_node->children[1] = new LightNode{	//Right
				{nullptr, nullptr},
				_node,
				pivot,
				rightNumLights,
				rightBucket.bounds,
				rightBucket.cone,
				rightBucket.totalPower,
				PowerVariance(pivot, rightNumLights)
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

	//Filter out infinite light(s)
	auto l = lightList.begin();
	while (l != lightList.end()) {
		EnvironmentLight *envLight = dynamic_cast<EnvironmentLight *>(*l);
		if (envLight) {
			infiniteLight = envLight;
			l = lightList.erase(l);
		}
		else ++l;
	}

	//Filter out mesh lights and represent as individual triangle lights
	l = lightList.begin();
	while (l != lightList.end()) {
		MeshLight *meshLight = dynamic_cast<MeshLight *>(*l);
		if (meshLight) {
			const TriangleMesh *mesh = &meshLight->GetMesh();
			triangleLights.reserve(triangleLights.size() + mesh->numTriangles);
			for (size_t i = 0; i < mesh->numTriangles; ++i) {
				triangleLights.insert({ { meshLight, i }, TriangleLight(meshLight, i) });
			}
			lightList.erase(l++);
		}
		else l++;
	}

	//Add the triangle lights ptrs to lights
	for (auto &it : triangleLights) lights.push_back(&it.second);

	//Add remaining from _lights to lights vector
	for (auto l : lightList) lights.push_back(l);

	//Initialise root
	if (lights.size() > 0) {
		root.reset(new LightNode);
		root->parent = nullptr;
		root->numLights = lights.size();
		root->firstLightIndex = 0;
		root->bounds = lights[0]->GetBounds();
		root->totalPower = 0;
		root->powerVariance = 0;
		root->orientationCone = OrientationCone::MakeCone(lights[0]->GetDirection());	//TO SELF: Will need to account for thetaE in future
		root->children[0] = root->children[1] = nullptr;
		for (unsigned i = 0; i < root->numLights; ++i) {
			root->bounds = maths::Union(root->bounds, lights[i]->GetBounds());
			root->orientationCone = OrientationCone::Union(root->orientationCone, OrientationCone::MakeCone(lights[i]->GetDirection()));
			root->totalPower += lights[i]->Power();
		}
	}
}

Real ManyLightSampler::ImportanceMeasure(const ScatterEvent &_event, LightNode *_node) const {	//TO SELF: Optimise
	Vec3 delta = _node->bounds.Center() - _event.hit->point;
	const Real clusterDiameter = _node->bounds.DiagonalLength();
	Real d2 = maths::Dot(delta, delta);
	Real d = std::sqrt(d2);
	if (!useSplits) {	//Clamp distance to half the cluster radius to prevent inaccurate importance values when centroid is very close
		const Real halfRadius = clusterDiameter * (Real).25;
		d = std::max(d, halfRadius);
		d2 = d * d;
	}
	const Real invD = (Real)1 / d;
	delta *= invD;
	const Real theta = std::acos(maths::Dot(delta, _node->orientationCone.axis));
	const Real thetaU = std::atan((clusterDiameter * (Real).5) * invD);
	const Real thetaDash = std::max(theta - _node->orientationCone.thetaO - thetaU, (Real)0);
	if (thetaDash < _node->orientationCone.thetaE) {
		const Real E = _node->totalPower;
		const Real thetaI = std::acos(maths::Dot(delta, _event.hit->normalS));
		return (std::cos(std::max(thetaI - thetaU, (Real)0)) * E) / d2 * std::cos(thetaDash);
	}
	else return 0;
}

bool ManyLightSampler::Split(const ScatterEvent &_event, LightNode *_node) const {
	const Real &powerVariance = _node->powerVariance;
	Real geometricMean;
	const Real geometricVariance = GeometricVariance(_node, _event.hit->point, &geometricMean);
	const Real geometricMean2 = geometricMean * geometricMean;
	const Real powerMean = _node->totalPower / (Real)_node->numLights;
	const Real powerMean2 = powerMean * powerMean;
	const Real numLights2 = _node->numLights * _node->numLights;
	const Real clusterVariance = (geometricVariance * powerVariance + powerVariance * geometricMean2 + powerMean2 * geometricVariance) * numLights2;
	const Real normalisedVariance = std::sqrt(std::sqrt(	(Real)1 / ((Real)1 + std::sqrt(clusterVariance))	));	//Threshold can now be between [0, 1]
	if (normalisedVariance < splitThreshold) return true;
	else return false;
}

Light *ManyLightSampler::GetLights(const ScatterEvent &_event, Real _epsilon, LightNode *_node, Real *_pdf) const {
	//if (_node->IsLeaf()) {
	//	Real lpdf = 1;
	//	const unsigned i = leafDistributions.at(_node->firstLightIndex).SampleDiscrete(_epsilon, &lpdf);	//Sample light distribution of cluster
	//	*_pdf *= lpdf;
	//	return lights[_node->firstLightIndex + i];
	//}
		if (Split(_event, _node)) {

		}
	return nullptr;
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
			_epsilon /= PL;
			*_pdf *= PL;
			return PickLight(_event, _epsilon, _node->children[0], _pdf);
		}
		else {
			_epsilon = (_epsilon - PL) / PR;
			*_pdf *= PR;
			return PickLight(_event, _epsilon, _node->children[1], _pdf);
		}
	}
}

Real ManyLightSampler::RecursivePDF(const ScatterEvent &_event, const LightNode *_node, const LightNode *_child) const {
	if (_child) {
		const bool side = _child == _node->children[0] ? 0 : 1;
		const Real IL = ImportanceMeasure(_event, _node->children[0]);
		const Real IR = ImportanceMeasure(_event, _node->children[1]);
		Real pdfs[2];
		pdfs[0] = IL / (IL + IR);
		pdfs[1] = 1 - IR;
		if (_node->parent) pdfs[side] *= RecursivePDF(_event, _node->parent, _node);
		return pdfs[side];
	}
	else if (_node->parent) return RecursivePDF(_event, _node->parent, _node);
	return 1;	//Shouldn't happen
}

LAMBDA_END