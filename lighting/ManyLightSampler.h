/*
	---- By Sam Warren 2020 ----
	An implementation of many lights sampling based on:
		Importance Sampling of Many Lights with Adaptive Tree Splitting - Estevez & Kulla
	- For use in scenes with high-res mesh lights or many individual lights.
	- Higher latency but more accurate importance sampling.
	- Instead of procedurally evaulating the leaf node CDF of lights, they are stored
	during tree construction and accessed using a hash table.
*/
#pragma once
#include <deque>
#include <unordered_map>
#include "LightSampler.h"

LAMBDA_BEGIN

class TriangleLight;

class ManyLightSampler : public LightSampler {
	public:
		Real threshold;

		ManyLightSampler(const Real _threshold = 0.1);

		ManyLightSampler(const Scene &_scene, const Real _threshold = 0.1);

		/*
			Stochastically samples a good light to sample from shading event, _event.
		*/
		Light *Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const override;

		/*
			Probability of choosing _light via traversal.
		*/
		Real Pdf(const ScatterEvent &_event, const Light *_light) const override;

		/*
			Builds the light tree.
		*/
		void Commit() override;

	private:
		/*
			thetaO bounds normals of lights; thetaE bounds emission profiles of lights.
			Most light's will have an emission profile of PI/2 (180� cone) with the exception of spotlights & similar.
			I mainly included thetaE to future-proof the sampler with emission profiles & spotlights etc.
		*/
		struct OrientationCone {
			Vec3 axis = { 0,1,0 };	//Cone axis
			Real thetaO = 0, thetaE = 0;

			static OrientationCone MakeCone(const Vec3 &_axis, const Real _thetaO = 0, const Real thetaE = PI / 2.);

			/*
				Returns the union of cones _a and _b.
			*/
			static OrientationCone Union(const OrientationCone &_a, const OrientationCone &_b);
		};

		struct LightNode {
			LightNode *children[2];	//If both nullptr, it is a leaf
			LightNode *parent;
			unsigned firstLightIndex, numLights;
			Bounds bounds;	//'each level of the hierarchy stores spatial and orientation bounds, as well as the energy total for all lights contained below.'
			OrientationCone orientationCone;
			Real totalPower, clusterVariance;	//Total power in cluster and variance of cluster

			~LightNode();

			inline bool IsLeaf() const {
				return !children[0] && !children[1];
			}
		};

		/*
			Lightweight representation of a node for tree construction (PBRT-style).
		*/
		struct Bucket {
			Real totalPower = 0;
			Bounds bounds;
			OrientationCone cone;
		};

		struct pair_hash {	//Silly stuff for being able to hash the pair
			template<class A, class B>
			inline size_t operator() (const std::pair<A, B> &_pair) const {
				return std::hash<A>()(_pair.first) ^ std::hash<B>()(_pair.second);
			}
		};
		
		typedef std::pair<const Light *, unsigned> TriangleLightKey;

		static constexpr unsigned numBuckets = 12;
		std::unordered_map<TriangleLightKey, TriangleLight, pair_hash> triangleLights;	//Edge case to triangle light from Light ptr and primitive index
		std::vector<Light *> lights;	//Keep infinite lights separate from finite lights for convenience when sampling
		Light *infiniteLight;
		std::unique_ptr<LightNode> infiniteNode;
		std::unordered_map<unsigned, Distribution::Piecewise1D> leafDistributions;	//First light index of leaf node 
		std::unordered_map<const Light *, std::pair<LightNode *, unsigned>> lightNodeDistributionMap;	//Required to quickly find *any* light's leaf node and position in leaf distribution.
		std::unique_ptr<LightNode> root;	//Root node of light tree

		/*
			Filters lights in _lights into respective containers.
			Converts mesh lights into triangle lights.
			Initiates root node as an unfinished leaf over all lights ready for RecursiveBuild().
		*/
		void InitLights(const std::vector<Light *> &_lights);

		/*
			Converts _node into a leaf over range of lights and builds respective distribution.
		*/
		void InitLeaf(LightNode *_node);

		/*
			Orientation cone measure function - analagous to bounding box surface area (MArea)
		*/
		static Real MOmega(const OrientationCone &_cone);

		/*
			Surface area orientation heuristic
		*/
		static Real SAOH(LightNode &_P, const Bucket &_R, const Bucket &_L, const unsigned _axis);

		/*
			Returns power variance (termed energy variance in paper) of cluster
		*/
		Real PowerVariance(LightNode *_node) const;

		/*
			Returns geometric variance of cluster
		*/
		Real GeometricVariance(LightNode *_node) const;

		/*
			Makes the best split cost along _axis of _node and stores the split in _leftBucket and _rightBucket if it beats _minCost.
			Returns true if it beat _minCost.
		*/
		bool SplitAxis(LightNode *_node, const unsigned _axis, Real *_minCost, unsigned *_bucketIndex, Bucket *_leftBucket, Bucket *_rightBucket);

		/*
			Recursively splits nodes with lowest SAOH cost split.
			Considers all axes.
		*/
		void RecursiveBuild(LightNode *_P);

		/*
			Returns the importance of node relative to scatter event. Will fail on leaf nodes.
		*/
		static Real ImportanceMeasure(const ScatterEvent &_event, LightNode *_node);

		/*
			Stochastically traverses a single branch and pulls light from leaf distribution.
		*/
		Light *PickLight(const ScatterEvent &_event, Real _epsilon, LightNode *_node, Real *_pdf) const;

		/*
			Returns probability of choosing _node via backwards traversal.
		*/
		Real RecursivePDF(const ScatterEvent &_event, const LightNode *_node, const LightNode *_child = nullptr) const;
};

LAMBDA_END