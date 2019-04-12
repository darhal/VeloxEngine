#pragma once

#include <Core/Misc/Defines/Common.hpp>

namespace MouseButton
{
	enum mouse_button_t
	{
		Left,
		Right,
		Middle
	};
}

struct MouseEvent
{
	uint32 X, Y;
	union
	{
		int Delta;
		MouseButton::mouse_button_t Button;
	};
};

namespace Key
{
	enum key_t
	{
		Unknown,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
		LeftBracket, RightBracket, Semicolon, Comma, Period, Quote, Slash, Backslash, Tilde, Equals, Hyphen,
		Escape, Control, Shift, Alt, Space, Enter, Backspace, Tab, PageUp, PageDown, End, Home, Insert, Delete, Pause,
		Left, Right, Up, Down,
		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		Add, Subtract, Multiply, Divide
	};
}


struct KeyEvent
{
	Key::key_t Code;
	bool Alt;
	bool Control;
	bool Shift;
};

struct WindowEvent
{
	union { int X; uint32 Width; };
	union { int Y; uint32 Height; };
};

struct Event
{
	enum event_t
	{
		TE_UNKNOWN,
		TE_CLOSE,
		TE_RESIZE,
		TE_MOVE,
		TE_FOCUS,
		TE_BLUR,
		TE_KEY_DOWN,
		TE_KEY_UP,
		TE_MOUSE_BTN_DOWN,
		TE_MOUSE_BTN_UP,
		TE_MOUSE_WHEEL,
		TE_MOUSE_MOVE
	};

	event_t Type;

	union
	{
		MouseEvent Mouse;
		KeyEvent Key;
		WindowEvent Window;
	};
};