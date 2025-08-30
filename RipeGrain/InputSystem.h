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
				device.OnLeftRelease = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse, EventMouseInput::Type::LeftRelease)); };
				device.OnRightPress = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse, EventMouseInput::Type::RightPress)); };
				device.OnRightRelease = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse, EventMouseInput::Type::RightRelease)); };
				device.OnWheel = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse, EventMouseInput::Type::Wheel)); };
				device.OnLeftDoubleClick = [this](auto& wnd) { raiseInputEvent(ConstructEvent(wnd.mouse, EventMouseInput::Type::LeftDoublePress)); };
			}
			else
			{
				device.OnCharInput = [this](auto ev) { raiseInputEvent(ConstructEvent(EventKeyBoardInput::Type::CharInput, ev.KEY_CODE, ev.IS_REPEATED)); };
				device.OnKeyPress = [this](auto ev) { raiseInputEvent(ConstructEvent(EventKeyBoardInput::Type::KeyPress, ev.KEY_CODE, ev.IS_REPEATED));};
				device.OnKeyRelease = [this](auto ev) { raiseInputEvent(ConstructEvent(EventKeyBoardInput::Type::KeyRelease, ev.KEY_CODE, ev.IS_REPEATED));};
			}
		}
	private:

		inline EventMouseInput ConstructEvent(CustomWindow::Mouse& mouse, EventMouseInput::Type type)
		{
			EventMouseInput ev; 
			ev.type = type;
			ev.x_pos = mouse.GetX();
			ev.y_pos = mouse.GetY();
			ev.client_x = mouse.GetX();
			ev.client_y = mouse.GetY();
			ev.delta = mouse.GetWheelDelta();
			return ev;
		}

		inline EventKeyBoardInput ConstructEvent(EventKeyBoardInput::Type type , unsigned char key_code , bool repeated)
		{
			EventKeyBoardInput ev;
			ev.type = type;
			ev.key_code = key_code;
			ev.is_repeated = repeated;
			return ev;
		}

		template<typename T>
		inline void raiseInputEvent(T input_ev)
		{
			RaiseEvent(CreateEventObject(input_ev));
		}
	};
}