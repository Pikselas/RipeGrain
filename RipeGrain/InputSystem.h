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
				device.OnMove = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse , EventMouseInput::Type::Move)); };
				device.OnLeftPress = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse, EventMouseInput::Type::LeftPress)); };
				device.OnRightPress = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse, EventMouseInput::Type::RightPress)); };
			}
			else
			{
				device.OnKeyPress = [this](auto ev) { raiseInputEvent(ConstructEvent(EventKeyBoardInput::Type::KeyPress, ev.KEY_CODE));};
				device.OnKeyPress = [this](auto ev) { raiseInputEvent(ConstructEvent(EventKeyBoardInput::Type::KeyRelease, ev.KEY_CODE)); };
			}
		}
	private:

		inline EventMouseInput ConstructEvent(CustomWindow::Mouse& mouse, EventMouseInput::Type type)
		{
			EventMouseInput ev; 
			ev.type = type;
			ev.x_pos = mouse.GetX();
			ev.y_pos = mouse.GetY();
			return ev;
		}

		inline EventKeyBoardInput ConstructEvent(EventKeyBoardInput::Type type , unsigned char key_code)
		{
			EventKeyBoardInput ev;
			ev.type = type;
			ev.key_code = key_code;
			return ev;
		}

		template<typename T>
		inline void raiseInputEvent(T input_ev)
		{
			RaiseEvent(std::make_unique<EventObject<T>>(CreateEventObject(input_ev)));
		}
	};
}