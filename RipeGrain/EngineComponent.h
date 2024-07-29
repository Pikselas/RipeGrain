#pragma once

class EngineComponent
{
public:
	virtual void OnUpdate() {};
public:
	virtual ~EngineComponent() = default;
};

class EngineEventRaiser : public virtual EngineComponent
{

};

class EngineEventSubscriber : public virtual EngineComponent
{

};