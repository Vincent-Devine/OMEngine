#include <OMEngine.hpp>

int main()
{
	OM::Core::Engine engine;
	engine.Run();

	OM_LOG_DEBUG("Hello!!", OM::Core::Log::ELogTag::TagEditor);
	
	return 0;
}