#pragma once

#include "OMEngine/Base.hpp"

namespace OM::Core
{
	class OM_ENGINE_API Engine
	{
	public:
		bool Initialisation();
		void Update();
		void Render() const;
		void Destroy();
	};
}