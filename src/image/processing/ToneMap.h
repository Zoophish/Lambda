#pragma once
#include "PostProcessing.h"

LAMBDA_BEGIN
namespace PostProcessing {

	enum class ToneMapMethod {
		NONE,
		ACES_FILMIC,
		REINHARD
	};

	class ToneMap : public PostProcess {
		public:
			ToneMapMethod method;
			bool clamp = true;

			ToneMap(const ToneMapMethod _method = ToneMapMethod::ACES_FILMIC);

			void Process(Texture *_target) const override;
	};

}
LAMBDA_END