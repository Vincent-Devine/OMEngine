#include <iostream>
#include <HelloWorld.hpp>

void HelloWorld()
{
    std::cout << "Hello World!" << std::endl;

#if Debug
    std::cout << "Hello World Debug!" << std::endl;
#elif Development
    std::cout << "Hello World Development!" << std::endl;
#elif Release
    std::cout << "Hello World Release!" << std::endl;
#endif
}