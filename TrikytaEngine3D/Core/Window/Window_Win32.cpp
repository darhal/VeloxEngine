#include <Core/Misc/Defines/Platform.hpp>

#if defined(OS_WINDOWS)

#include <Core/Context/Context.hpp>
#include <Core/Window/Window.hpp>

TRE::Window::Window(uint width, uint height, const std::string& title, WindowStyle::window_style_t style)
{
	// Create class for OpenGL window
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowEventHandler;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = "OOGL_WINDOW";
	if (style & WindowStyle::Borderless) {
		wc.style = CS_HREDRAW | CS_VREDRAW;
	}else {
		wc.style = CS_OWNDC;
	}
	RegisterClass(&wc);

	// Configure window style
	uint64 windowStyle = 0;
	if (style & WindowStyle::Base) windowStyle |= WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE;
	if (style & WindowStyle::Close) windowStyle |= WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE | WS_SYSMENU;
	if (style & WindowStyle::Resize) windowStyle |= WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX;
	if (style & WindowStyle::Borderless) windowStyle |= WS_POPUP | WS_VISIBLE;

	int x = 0;
	int y = 0;

	if (!(style & WindowStyle::Fullscreen))
	{
		// Calculate window size for requested client size
		RECT rect = { (LONG)0, (LONG)0, (LONG)width, (LONG)height };
		AdjustWindowRect(&rect, (DWORD)windowStyle, false);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		// Center window on screen
		GetClientRect(GetDesktopWindow(), &rect);
		x = (rect.right - rect.left - width) / 2;
		y = (rect.bottom - rect.top - height) / 2;
	}

	// Create window
	HWND window = CreateWindow("OOGL_WINDOW", title.c_str(), (DWORD)windowStyle, x, y, width, height, NULL, NULL, GetModuleHandle(NULL), this);
	
	// Initialize fullscreen mode
	if (style & WindowStyle::Fullscreen)
	{
		DEVMODE dm;
		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;
		dm.dmBitsPerPel = 32;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

		SetWindowLong(window, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		SetWindowLong(window, GWL_EXSTYLE, WS_EX_APPWINDOW);

		SetWindowPos(window, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);
		ShowWindow(window, SW_SHOW);
	}

	// Initialize window properties
	RECT rect;
	GetWindowRect(window, &rect);

	windowPosition.x = rect.left;
	windowPosition.y = rect.top;

	GetClientRect(window, &rect);

	windowSize.x = rect.right - rect.left;
	windowSize.y = rect.bottom - rect.top;
	this->open = true;
	this->style = (DWORD)windowStyle;
	mousePosition.x = 0;
	mousePosition.y = 0;
	memset(this->mouse, 0, sizeof(this->mouse));
	memset(this->keys, 0, sizeof(this->keys));
	this->context = 0;
}

TRE::Window::~Window()
{
	if (context) delete context;

	DestroyWindow(window);

	UnregisterClass("OGLWINDOW", GetModuleHandle(NULL));
}

void TRE::Window::setWindowPosition(int x, int y)
{
	if (!open) return;
	SetWindowPos(window, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void TRE::Window::setWindowSize(uint width, uint height)
{
	if (!open) return;
	RECT rect = { (LONG)0, (LONG)0, (LONG)width, (LONG)height };
	AdjustWindowRect(&rect, style, false);
	SetWindowPos(window, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

void TRE::Window::setWindowTitle(const std::string& title)
{
	if (!open) return;
	SetWindowText(window, title.c_str());
}

void TRE::Window::setVisible(bool visible)
{
	if (!open) return;
	ShowWindow(window, visible ? SW_SHOW : SW_HIDE);
}

void TRE::Window::Close()
{
	CloseWindow(window);
	open = false;
}

bool TRE::Window::getEvent(Event& ev)
{
	// Fetch new events
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Return oldest event - if available
	if (events.empty()) return false;

	ev = events.front();
	events.pop();

	return true;
}

TRE::Context& TRE::Window::getContext()
{
	return *context;
}

TRE::Context& TRE::Window::initContext(uint8 vmajor, uint8 vminor, uint8 color, uint8 depth, uint8 stencil, uint8 antialias)
{
	return *(context = new Context(vmajor, vminor, color, depth, stencil, antialias, GetDC(window)));
}

void TRE::Window::Present()
{
	if (!context) return;
	context->Activate();
	SwapBuffers(GetDC(window));
}

LRESULT TRE::Window::WindowEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
	Event ev;
	ev.Type = Event::TE_UNKNOWN;

	// Translate message to Event
	switch (msg){
	case WM_CLOSE:
		Close();
		ev.Type = Event::TE_CLOSE;
		break;
	case WM_SIZE:
		windowSize.x = GET_X_LPARAM(lParam);
		windowSize.y = GET_Y_LPARAM(lParam);
		if (events.empty()) {
			ev.Type = Event::TE_RESIZE;
			ev.Window.Width = windowSize.x;
			ev.Window.Height = windowSize.y;
		}else if (events.back().Type == Event::TE_RESIZE) {
			events.back().Window.Width = windowSize.x;
			events.back().Window.Height = windowSize.y;
		}
		break;
	case WM_MOVE:
		RECT rect;
		GetWindowRect(window, &rect);
		windowPosition.x = rect.left;
		windowPosition.y = rect.top;

		if (events.empty()) {
			ev.Type = Event::TE_MOVE;
			ev.Window.X = windowPosition.x;
			ev.Window.Y = windowPosition.y;
		}else if (events.back().Type == Event::TE_MOVE) {
			events.back().Window.X = windowPosition.x;
			events.back().Window.Y = windowPosition.y;
		}
		break;
	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE) {
			ev.Type = Event::TE_BLUR;
			focus = false;
		}else{
			ev.Type = Event::TE_FOCUS;
			focus = true;
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		ev.Type = Event::TE_KEY_DOWN;
		ev.Key.Code = TranslateKey(wParam);
		ev.Key.Alt = HIWORD(GetAsyncKeyState(VK_MENU)) != 0;
		ev.Key.Control = HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0;
		ev.Key.Shift = HIWORD(GetAsyncKeyState(VK_SHIFT)) != 0;
		keys[ev.Key.Code] = true;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		ev.Type = Event::TE_KEY_UP;
		ev.Key.Code = TranslateKey(wParam);
		ev.Key.Alt = HIWORD(GetAsyncKeyState(VK_MENU)) != 0;
		ev.Key.Control = HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0;
		ev.Key.Shift = HIWORD(GetAsyncKeyState(VK_SHIFT)) != 0;
		keys[ev.Key.Code] = false;
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		mousePosition.x = GET_X_LPARAM(lParam);
		mousePosition.y = GET_Y_LPARAM(lParam);
		if (msg == WM_LBUTTONDOWN) ev.Mouse.Button = MouseButton::Left;
		else if (msg == WM_RBUTTONDOWN) ev.Mouse.Button = MouseButton::Right;
		else if (msg == WM_MBUTTONDOWN) ev.Mouse.Button = MouseButton::Middle;
		else break;
		ev.Type = Event::TE_MOUSE_BTN_DOWN;
		ev.Mouse.X = mousePosition.x;
		ev.Mouse.Y = mousePosition.y;
		mouse[ev.Mouse.Button] = true;
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		mousePosition.x = GET_X_LPARAM(lParam);
		mousePosition.y = GET_Y_LPARAM(lParam);
		if (msg == WM_LBUTTONUP) ev.Mouse.Button = MouseButton::Left;
		else if (msg == WM_RBUTTONUP) ev.Mouse.Button = MouseButton::Right;
		else if (msg == WM_MBUTTONUP) ev.Mouse.Button = MouseButton::Middle;
		else break;
		ev.Type = Event::TE_MOUSE_BTN_UP;
		ev.Mouse.X = mousePosition.x;
		ev.Mouse.Y = mousePosition.y;
		mouse[ev.Mouse.Button] = false;
		break;
	case WM_MOUSEWHEEL:
		mousePosition.x = GET_X_LPARAM(lParam);
		mousePosition.y = GET_Y_LPARAM(lParam);
		ev.Type = Event::TE_MOUSE_WHEEL;
		ev.Mouse.Delta = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
		ev.Mouse.X = mousePosition.x;
		ev.Mouse.Y = mousePosition.y;
		break;
	case WM_MOUSEMOVE:
		mousePosition.x = GET_X_LPARAM(lParam);
		mousePosition.y = GET_Y_LPARAM(lParam);
		ev.Type = Event::TE_MOUSE_MOVE;
		ev.Mouse.X = mousePosition.x;
		ev.Mouse.Y = mousePosition.y;
		break;
	default:
		return DefWindowProc(window, msg, wParam, lParam);
	}
	// Add event to internal queue
	if (ev.Type != Event::TE_UNKNOWN)
		events.push(ev);
	return 0;
}

LRESULT CALLBACK TRE::Window::WindowEventHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TRE::Window* window;
	if (msg == WM_NCCREATE){
		// Store pointer to associated Window class as userdata in Win32 window
		window = reinterpret_cast<Window*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
		window->window = hwnd;
		SetWindowLong(hwnd, GWL_USERDATA, reinterpret_cast<long>(window));
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}else{
		window = reinterpret_cast<Window*>(GetWindowLong(hwnd, GWL_USERDATA));
		if (window != nullptr)
			return window->WindowEvent(msg, wParam, lParam);
		else
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

FORCEINLINE Key::key_t TRE::Window::TranslateKey(uint code)
{
	switch (code)
	{
	case VK_SHIFT: return Key::Shift;
	case VK_MENU: return Key::Alt;
	case VK_CONTROL: return Key::Control;
	case VK_OEM_1: return Key::Semicolon;
	case VK_OEM_2: return Key::Slash;
	case VK_OEM_PLUS: return Key::Equals;
	case VK_OEM_MINUS: return Key::Hyphen;
	case VK_OEM_4: return Key::LeftBracket;
	case VK_OEM_6: return Key::RightBracket;
	case VK_OEM_COMMA: return Key::Comma;
	case VK_OEM_PERIOD: return Key::Period;
	case VK_OEM_7: return Key::Quote;
	case VK_OEM_5: return Key::Backslash;
	case VK_OEM_3: return Key::Tilde;
	case VK_ESCAPE: return Key::Escape;
	case VK_SPACE: return Key::Space;
	case VK_RETURN: return Key::Enter;
	case VK_BACK: return Key::Backspace;
	case VK_TAB: return Key::Tab;
	case VK_PRIOR: return Key::PageUp;
	case VK_NEXT: return Key::PageDown;
	case VK_END: return Key::End;
	case VK_HOME: return Key::Home;
	case VK_INSERT: return Key::Insert;
	case VK_DELETE: return Key::Delete;
	case VK_ADD: return Key::Add;
	case VK_SUBTRACT: return Key::Subtract;
	case VK_MULTIPLY: return Key::Multiply;
	case VK_DIVIDE: return Key::Divide;
	case VK_PAUSE: return Key::Pause;
	case VK_LEFT: return Key::Left;
	case VK_RIGHT: return Key::Right;
	case VK_UP: return Key::Up;
	case VK_DOWN: return Key::Down;

	default:
		if (code >= VK_F1 && code <= VK_F12)
			return (Key::key_t)(Key::F1 + code - VK_F1);
		else if (code >= VK_NUMPAD0 && code <= VK_NUMPAD9)
			return (Key::key_t)(Key::Numpad0 + code - VK_NUMPAD0);
		else if (code >= 'A' && code <= 'Z')
			return (Key::key_t)(Key::A + code - 'A');
		else if (code >= '0' && code <= '9')
			return (Key::key_t)(Key::Num0 + code - '0');
	}

	return Key::Unknown;
}

#endif