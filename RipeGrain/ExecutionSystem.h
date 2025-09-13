#pragma once
#include "EngineServices.h"
#include "EngineComponent.h"
#include "../Crotine/Xecutor.hpp"

namespace RipeGrain
{
	class ExecutionSystem : public EngineComponent, public ExecutionService
	{
	private:
		Crotine::Xecutor executor;
	public:
		ExecutionSystem() : ExecutionService(executor) {}
	};
}