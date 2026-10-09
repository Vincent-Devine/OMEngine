#include <OMEngine.hpp>

int main()
{
	OM::Core::Log::Logger::GetInstance()->OpenLogFile("old_log.txt");
	OM::Core::Log::Logger::GetInstance()->OpenLogFile("log.txt");

	OM_LOG_DEBUG("Hello!!", OM::Core::Log::ELogTag::TagEditor);

	OM::Core::Log::Logger::GetInstance()->Destroy();
	return 0;
}