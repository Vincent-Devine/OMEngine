#include <iostream>
#include <HelloWorld.hpp>

void HelloWorld()
{
    std::cout << "Hello World!" << std::endl;

#if defined(OM_DEBUG)
    std::cout << "Hello World Debug!" << std::endl;
#elif defined(OM_DEVELOPMENT)
    std::cout << "Hello World Development!" << std::endl;
#elif defined(OM_RELEASE)
    std::cout << "Hello World Release!" << std::endl;
#endif
}