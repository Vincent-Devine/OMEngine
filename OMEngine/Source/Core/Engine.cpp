#include <Core/Engine.hpp>

#include <Core/Log/Logger.hpp>

namespace OM::Core
{
	void Engine::Run()
	{
		Initialization();

		// todo: game loop
		// while(true)
		Update();
		Render();

		Destroy();
	}

	void Engine::Initialization()
	{
		OM_LOG_INFO("Start initialization", OM::Core::Log::ELogTag::TagCore);
		Log::Logger::Get()->OpenLogFile("log.txt");
		OM_LOG_INFO("End initialization", OM::Core::Log::ELogTag::TagCore);
	}

	void Engine::Update()
	{
		OM_LOG_INFO("Update", OM::Core::Log::ELogTag::TagCore);
	}

	void Engine::Render() const
	{
		OM_LOG_INFO("Render", OM::Core::Log::ELogTag::TagCore);
	}

	void Engine::Destroy()
	{
		OM_LOG_INFO("Start destroy", OM::Core::Log::ELogTag::TagCore);
		Log::Logger::Get()->Destroy();
		OM_LOG_INFO("End destroy", OM::Core::Log::ELogTag::TagCore);
	}
}