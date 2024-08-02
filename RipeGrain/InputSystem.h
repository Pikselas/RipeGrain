#pragma once
#include "EngineComponent.h"
#include "RepulsiveEngine/CustomWindow.h"

namespace RipeGrain
{
	template<typename T>
	concept CInputDevice = std::is_same_v<T , CustomWindow::Mouse> or std::is_same_v<T, CustomWindow::KeyBoard>;

	class InputSystem : public EngineEventRaiser
	{
	public:
		template<CInputDevice InputDevice>
		InputSystem(InputDevice& device)
		{
			if constexpr (std::is_same_v<InputDevice, CustomWindow::Mouse>)
			{
				device.OnMove = [this](auto& wnd) { raiseInputEvent(ConstructMouseEvent(wnd.mouse , EventMouseInput::Type::Move)); };
				device.OnLeftPress = [this](auto& wnd) { raiseInputEvent(ConstructMouseEvent(wnd.mouse, EventMouseInput::Type::LeftPress)); };
				device.OnRightPress = [this](auto& wnd) { raiseInputEvent(ConstructMouseEvent(wnd.mouse, EventMouseInput::Type::RightPress)); };
			}
		}
	private:

		inline EventMouseInput ConstructMouseEvent(CustomWindow::Mouse& mouse, EventMouseInput::Type type)
		{
			EventMouseInput ev; 
			ev.type = type;
			ev.x_pos = mouse.GetX();
			ev.y_pos = mouse.GetY();
			return ev;
		}

		template<typename T>
		inline void raiseInputEvent(T input_ev)
		{
			RaiseEvent(std::make_unique<EventObject<T>>(CreateEventObject(input_ev)));
		}
	};
}