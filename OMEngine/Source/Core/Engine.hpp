#pragma once

#include <Utils/OMResult.hpp>

namespace OM::Core
{
	class Engine
	{
	public:
		void Run();

	private:
		void Initialization();
		void Update();
		void Render() const;
		void Destroy();
	};
}