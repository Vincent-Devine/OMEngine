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
		OM::Core::Log::Logger::Get()->OpenLogFile("log.txt");
	}

	void Engine::Update()
	{

	}

	void Engine::Render() const
	{

	}

	void Engine::Destroy()
	{
		Log::Logger::Get()->Destroy();
	}
}