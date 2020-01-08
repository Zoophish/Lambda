#pragma once
#include <string>
#include <vector>
#include <maths/maths.h>
#include <assimp/scene.h>
#include "ResourceManager.h"

namespace ImportUtilities {

	struct ImportMetrics {
		std::string name, path;
		std::vector<std::string> metrics, errors;

		ImportMetrics(const std::string &_name, const std::string &_path = "");
		
		void AppendMetric(const std::string &_metric);

		void AppendError(const std::string &_error);

		void LogMetrics() const;

		void LogErrors() const;

		void LogAll() const;

		bool HasErrors() const;
	};



	/*
		Converts aiMatrix4x4 to Affine3.
	*/
	inline Affine3 aiMatToAff3(const aiMatrix4x4 &_aiMat) {
		Affine3 aff;
		aff[0] = _aiMat.a1; aff[1] = _aiMat.a2; aff[2] = _aiMat.a3;
		aff[3] = _aiMat.b1; aff[4] = _aiMat.b2; aff[5] = _aiMat.b3;
		aff[6] = _aiMat.c1; aff[7] = _aiMat.c2; aff[8] = _aiMat.c3;
		aff[9] = _aiMat.d1; aff[10] = _aiMat.d2; aff[11] = _aiMat.d3;
		return aff;
	}
}