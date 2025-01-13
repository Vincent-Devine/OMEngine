#pragma once

#include "OMEngine/Base.hpp"

namespace OM::Wrapper
{
	class OM_ENGINE_API RHI
	{
	public:
		static RHI* GetInstance();

		bool Initialisation();
		void Update();
		void Render() const;
		void Destroy();

	private:
		static inline RHI* _instance = nullptr;
	};
}