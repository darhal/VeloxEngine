#include <Core/Window/Window.hpp>

bool TRE::Window::isOpen()
{
	return open;
}

bool TRE::Window::hasFocus()
{
	return focus;
}

bool TRE::Window::isMouseButtonDown(MouseButton::mouse_button_t button)
{
	if (button >= sizeof(mouse) / sizeof(bool)) return false;
	return mouse[button];
}

bool TRE::Window::isKeyDown(Key::key_t key)
{
	if (key >= sizeof(keys) / sizeof(bool)) return false;
	return keys[key];
}

const Vec2<uint32>& TRE::Window::getSize() const
{
	return windowSize;
}

const Vec2<int32>& TRE::Window::getPosition() const
{
	return windowPosition;
}

const Vec2<int32>& TRE::Window::getMousePosition() const
{
	return mousePosition;
}