#include "ImportUtilities.h"
#include <iostream>

LAMBDA_BEGIN

namespace ImportUtilities {

	ImportMetrics::ImportMetrics(const std::string &_name, const std::string &_path) {
		name = _name;
		const size_t lastSlash = _path.find_last_of("\\/");
		if (lastSlash != std::string::npos) path = _path.substr(0, lastSlash);
		else path = _path;
	}

	void ImportMetrics::AppendMetric(const std::string &_metric) {
		metrics.push_back(_metric);
	}

	void ImportMetrics::AppendError(const std::string &_error) {
		errors.push_back(_error);
	}

	void ImportMetrics::LogMetrics() const {
		std::cout << std::endl << name << ':';
		for (auto &it : metrics) {
			std::cout << std::endl << '	' << it;
		}
	}

	void ImportMetrics::LogErrors() const {
		if (metrics.size() == 0) {
			std::cout << std::endl << "No errors.";
			return;
		}
		std::cout << std::endl << name << " - " << errors.size() << " errors:";
		for (auto &it : errors) {
			std::cout << std::endl << '	' << it;
		}
	}

	void ImportMetrics::LogAll() const {
		LogMetrics();
		if(errors.size() > 0) LogErrors();
	}

	bool ImportMetrics::HasErrors() const {
		return errors.size() > 0;
	}

}

LAMBDA_END