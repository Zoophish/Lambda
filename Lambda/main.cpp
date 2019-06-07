#include <iostream>
#include <maths/maths.h>
#include <image/Texture.h>

int main() {

	vec3f vec(1.123, 2.23432, 3.23423);

	TextureRGBAFloat tex(512,512, Colour(1, 0.5, .01));
	tex.SaveToImageFile("test.png");

	realf number = 1;
	std::cout << vec.Magnitude() * number << std::endl;
	system("pause");
	
	return 0;
}