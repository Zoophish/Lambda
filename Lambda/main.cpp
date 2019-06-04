#include <iostream>
#include <maths.h>

int main() {

	vec3f vec(1.123, 2.23432, 3.23423);
	
	realf number = 1;
	std::cout << vec.Magnitude() * number << std::endl;
	system("pause");
	
	return 0;
}