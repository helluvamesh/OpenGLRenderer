#pragma once

struct GLFWwindow;
struct ImGuiContext;


enum class EKeyAction : uint8 { UNKNOWN, PRESS, RELEASE, REPEAT };


enum class EKeyCode : uint8
{ 
	UNKNOWN,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	TOP_0, TOP_1, TOP_2, TOP_3, TOP_4, TOP_5, TOP_6, TOP_7, TOP_8, TOP_9,
	LEFT_SHIFT, LEFT_CTRL, ALT, RIGHT_SHIFT, RIGHT_CTRL, ALT_GR, CAPS_LOCK, NUM_LOCK,
	ENTER, SPACE, BACKSPACE, ESC, TAB, MENU,
	UP, DOWN, LEFT, RIGHT,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	INSERT, DEL, HOME, END, PAGE_UP, PAGE_DOWN,
	NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
	NUM_DIVIDE, NUM_MULTIPLY, NUM_SUBTRACT, NUM_ADD, NUM_ENTER, NUM_DECIMAL,
	PRINT_SCREEN, SCROLL_LOCK, PAUSE
};


struct DKeyEvent
{
private:
	static DMap<int32, EKeyCode> KeyMap;
	
	int32 Key;
	int32 Action;
	int32 Mods;

public:
	DKeyEvent(int32 InKey, int32 InAction, int32 InMods);

	EKeyCode GetKey();
	EKeyAction GetAction();
	bool IsShiftDown();
	bool IsCtrlDown();
	bool IsAltDown();
	bool IsCapsLockSet();
	bool IsNumLockSet();
};


struct DMouseMotionEvent
{
private:
	float X;
	float Y;

public:
	DMouseMotionEvent(float InX, float InY);

	float GetX();
	float GetY();
};


enum class EMouseAction : uint8 { UNKNOWN, PRESS, RELEASE };


enum class EMouseButton : uint8 { UNKNOWN, LEFT, RIGHT, MIDDLE };


struct DMouseButtonEvent
{
private:
	int32 Button;
	int32 Action;
	int32 Mods;

public:
	DMouseButtonEvent(int32 InButton, int32 InAction, int32 InMods);

	EMouseButton GetKey();
	EMouseAction GetAction();
	bool IsShiftDown();
	bool IsCtrlDown();
	bool IsAltDown();
};


struct DScrollEvent
{
private:
	float X;
	float Y;

public:
	DScrollEvent(float InX, float InY);
	float GetX();
	float GetY();
};


enum class ECursorMode : uint8 { NORMAL, HIDDEN, HIDDEN_AND_LOCKED };


using DKeyEventDelegate = DMultiCastDelegate<DKeyEvent>;
using DCharEventDelegate = DMultiCastDelegate<tchar>;
using DMouseMotionEventDelegate = DMultiCastDelegate<DMouseMotionEvent>;
using DCursorPosDelegate = DMultiCastDelegate<float, float>;
using DMouseButtonEventDelegate = DMultiCastDelegate<DMouseButtonEvent>;
using DScrollEventDelegate = DMultiCastDelegate<DScrollEvent>;
using DWindowPosDelegate = DMultiCastDelegate<int32, int32>;
using DWindowSizeDelegate = DMultiCastDelegate<int32, int32>;
using DWindowIconifyDelegate = DMultiCastDelegate<bool>;
using DWindowCloseRequestDelegate = DSingleCastDelegate<bool>;
using DFileDropDelegate = DSingleCastDelegate<void, const DArray<DString>&>;

using DRenderDelegate = DSingleCastDelegate<void>;


class HWindow3D : public HObject
{
private:

	GLFWwindow* GLFWwindowPtr = nullptr;
	ImGuiContext* ImGuiCtx = nullptr;

	float PrevMouseX = .0f;
	float PrevMouseY = .0f;
	float MouseDeltaX = .0f;
	float MouseDeltaY = .0f;
	ECursorMode CursorMode = ECursorMode::NORMAL;

	bool Paused = false;
	bool Iconified = false;

public:

	DKeyEventDelegate OnKey;
	DCharEventDelegate OnCharTyped;
	DMouseMotionEventDelegate OnMouseMotion;
	DMouseButtonEventDelegate OnMouseButton;
	DScrollEventDelegate OnScroll;
	DWindowPosDelegate OnWindowPos;
	DWindowSizeDelegate OnWindowSize;
	DWindowIconifyDelegate OnWindowIconified;
	DWindowCloseRequestDelegate OnWindowCloseRequest;
	DFileDropDelegate OnFileDrop;

	DRenderDelegate OnRenderStart;
	DRenderDelegate OnRenderFrame;
	DRenderDelegate OnRenderUI;
	DRenderDelegate OnRenderStop;

	HWindow3D(int32 InWidth = 800, int32 InHeight = 600, DString8 InName = "Window", bool InHidden = false);
	~HWindow3D();

private:

	static void KeyCallback(GLFWwindow* InGLFWwindow, int32 Key, int32 Scancode, int32 Action, int32 Mods);
	static void CharCallback(GLFWwindow* InGLFWwindow, uint32 CodePoint);
	static void CursorPosCallback(GLFWwindow* InGLFWwindow, double X, double Y);
	static void MouseButtonCallback(GLFWwindow* InGLFWwindow, int32 Button, int32 Action, int32 Mods);
	static void ScrollCallback(GLFWwindow* InGLFWwindow, double X, double Y);
	static void WindowPosCallback(GLFWwindow* InGLFWwindow, int32 X, int32 Y);
	static void WindowSizeCallback(GLFWwindow* InGLFWwindow, int32 Width, int32 Height);
	static void WindowIconifyCallback(GLFWwindow* InGLFWwindow, int32 Bool);
	static void WindowCloseCallback(GLFWwindow* InGLFWwindow);
	static void DropCallback(GLFWwindow* InGLFWwindow, int32 Count, const char** Paths);

public:

	void StartRender();
	void StopRender();

	void GetMousePos(float& OutX, float& OutY) const;
	void GetMouseDelta(float& OutX, float& OutY) const;
	ECursorMode GetCursorMode() const;
	void SetCursorMode(ECursorMode Mode);
	void GetPos(int32& OutX, int32& OutY) const;
	void GetSize(int32& OutWidth, int32& OutHeight) const;
	void SetPos(int32 NewX, int32 NewY);
	void SetPosIncludingFrame(int32 NewX, int32 NewY);
	void SetSize(int32 NewWidth, int32 NewHeight);
	void SetSizeIncludingFrame(int32 NewWidth, int32 NewHeight);
	bool IsIconified();
	void SetIconified(bool Value);
	bool GetShouldClose();
	void SetShouldClose(bool Value);
	void GetOpenGLVersion(int32& OutMajor, int32& OutMinor);
	void SetUseVSync(bool Value);

	void SetPaused(bool Value);
	bool IsPaused();
};