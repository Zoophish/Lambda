#include <vector>
#include "../Texture.h"

namespace PostProcessing {

	class PostProcess {
		public:
			virtual void Process(Texture *_texture) const = 0;
	};

	class PostProcessStack {
		public:
			std::vector<PostProcess> stack;

			PostProcessStack() {}

			void Process(Texture *_texture) const;
	};

	class ConvolutionFilter : public PostProcess {
		public:
			Texture *kernel;
			Real intensity;

			ConvolutionFilter(Texture *_kernel, const Real _intensity);

			void Process(Texture *_texture) const override;

		private:
			Colour Filter(unsigned _x, unsigned _y, Texture *_target) const;
	};

}