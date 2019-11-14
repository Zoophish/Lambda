#include "ImportUtilities.h"
#include <iostream>

namespace ImportUtilities {

	ImportMetrics::ImportMetrics(const std::string &_name) {
		name = _name;
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

	bool ImportMetrics::HasErrors() const {
		return errors.size() > 0;
	}

}