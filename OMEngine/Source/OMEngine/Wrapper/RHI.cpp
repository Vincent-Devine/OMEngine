#include "pch.h"

#include "OMEngine/Wrapper/RHI.hpp"

namespace OM::Wrapper
{
	RHI* RHI::GetInstance()
	{
		if (!_instance)
			_instance = new RHI();
		return _instance;
	}

	bool RHI::Initialisation()
	{
		return true;
	}

	void RHI::Update()
	{
	}

	void RHI::Render() const
	{
	}

	void RHI::Destroy()
	{
	}
}