#include "Engine/pch.h"
#include "HWindow3D.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Engine\Dependencies\IMGUI\ImGuiAll.h"


bool IsBitSet(int32 N, int32 Bit) 
{	
	return N & (1 << (Bit - 1));
} 


DMap<int32, EKeyCode> DKeyEvent::KeyMap {

	{ GLFW_KEY_UNKNOWN		, EKeyCode::UNKNOWN },
	{ GLFW_KEY_SPACE		, EKeyCode::SPACE },
	//{ GLFW_KEY_APOSTROPHE	, EKeyCode:: },
	//{ GLFW_KEY_COMMA		, EKeyCode:: },
	//{ GLFW_KEY_MINUS		, EKeyCode:: },
	//{ GLFW_KEY_PERIOD		, EKeyCode:: },
	//{ GLFW_KEY_SLASH		, EKeyCode:: },
	{ GLFW_KEY_0			, EKeyCode::TOP_0 },
	{ GLFW_KEY_1			, EKeyCode::TOP_1 },
	{ GLFW_KEY_2			, EKeyCode::TOP_2 },
	{ GLFW_KEY_3			, EKeyCode::TOP_3 },
	{ GLFW_KEY_4			, EKeyCode::TOP_4 },
	{ GLFW_KEY_5			, EKeyCode::TOP_5 },
	{ GLFW_KEY_6			, EKeyCode::TOP_6 },
	{ GLFW_KEY_7			, EKeyCode::TOP_7 },
	{ GLFW_KEY_8			, EKeyCode::TOP_8 },
	{ GLFW_KEY_9			, EKeyCode::TOP_9 },
	//{ GLFW_KEY_SEMICOLON	, EKeyCode:: },
	//{ GLFW_KEY_EQUAL		, EKeyCode:: },
	{ GLFW_KEY_A			, EKeyCode::A },
	{ GLFW_KEY_B			, EKeyCode::B },
	{ GLFW_KEY_C			, EKeyCode::C },
	{ GLFW_KEY_D			, EKeyCode::D },
	{ GLFW_KEY_E			, EKeyCode::E },
	{ GLFW_KEY_F			, EKeyCode::F },
	{ GLFW_KEY_G			, EKeyCode::G },
	{ GLFW_KEY_H			, EKeyCode::H },
	{ GLFW_KEY_I			, EKeyCode::I },
	{ GLFW_KEY_J			, EKeyCode::J },
	{ GLFW_KEY_K			, EKeyCode::K },
	{ GLFW_KEY_L			, EKeyCode::L },
	{ GLFW_KEY_M 			, EKeyCode::M },
	{ GLFW_KEY_N			, EKeyCode::N },
	{ GLFW_KEY_O			, EKeyCode::O },
	{ GLFW_KEY_P			, EKeyCode::P },
	{ GLFW_KEY_Q			, EKeyCode::Q },
	{ GLFW_KEY_R			, EKeyCode::R },
	{ GLFW_KEY_S			, EKeyCode::S },
	{ GLFW_KEY_T			, EKeyCode::T },
	{ GLFW_KEY_U			, EKeyCode::U },
	{ GLFW_KEY_V			, EKeyCode::V },
	{ GLFW_KEY_W			, EKeyCode::W },
	{ GLFW_KEY_X			, EKeyCode::X },
	{ GLFW_KEY_Y			, EKeyCode::Y },
	{ GLFW_KEY_Z			, EKeyCode::Z },
	//{ GLFW_KEY_LEFT_BRACKET	, EKeyCode:: },
	//{ GLFW_KEY_BACKSLASH	, EKeyCode:: },
	//{ GLFW_KEY_RIGHT_BRACKET, EKeyCode:: },
	//{ GLFW_KEY_GRAVE_ACCENT	, EKeyCode:: },
	//{ GLFW_KEY_WORLD_1		, EKeyCode:: },
	//{ GLFW_KEY_WORLD_2		, EKeyCode:: },
	{ GLFW_KEY_ESCAPE		, EKeyCode::ESC },
	{ GLFW_KEY_ENTER		, EKeyCode::ENTER },
	{ GLFW_KEY_TAB			, EKeyCode::TAB },
	{ GLFW_KEY_BACKSPACE	, EKeyCode::BACKSPACE },
	{ GLFW_KEY_INSERT		, EKeyCode::INSERT },
	{ GLFW_KEY_DELETE		, EKeyCode::DEL },
	{ GLFW_KEY_RIGHT		, EKeyCode::RIGHT },
	{ GLFW_KEY_LEFT			, EKeyCode::LEFT },
	{ GLFW_KEY_DOWN			, EKeyCode::DOWN },
	{ GLFW_KEY_UP			, EKeyCode::UP },
	{ GLFW_KEY_PAGE_UP		, EKeyCode::PAGE_UP },
	{ GLFW_KEY_PAGE_DOWN	, EKeyCode::PAGE_DOWN },
	{ GLFW_KEY_HOME			, EKeyCode::HOME },
	{ GLFW_KEY_END			, EKeyCode::END },
	{ GLFW_KEY_CAPS_LOCK	, EKeyCode::CAPS_LOCK },
	{ GLFW_KEY_SCROLL_LOCK	, EKeyCode::SCROLL_LOCK },
	{ GLFW_KEY_NUM_LOCK		, EKeyCode::NUM_LOCK },
	{ GLFW_KEY_PRINT_SCREEN	, EKeyCode::PRINT_SCREEN },
	{ GLFW_KEY_PAUSE		, EKeyCode::PAUSE },
	{ GLFW_KEY_F1			, EKeyCode::F1 },
	{ GLFW_KEY_F2			, EKeyCode::F2 },
	{ GLFW_KEY_F3			, EKeyCode::F3 },
	{ GLFW_KEY_F4			, EKeyCode::F4 },
	{ GLFW_KEY_F5			, EKeyCode::F5 },
	{ GLFW_KEY_F6			, EKeyCode::F6 },
	{ GLFW_KEY_F7			, EKeyCode::F7 },
	{ GLFW_KEY_F8			, EKeyCode::F8 },
	{ GLFW_KEY_F9			, EKeyCode::F9 },
	{ GLFW_KEY_F10			, EKeyCode::F10 },
	{ GLFW_KEY_F11			, EKeyCode::F11 },
	{ GLFW_KEY_F12			, EKeyCode::F12 },
	//{ GLFW_KEY_F13			, EKeyCode:: },
	//{ GLFW_KEY_F14			, EKeyCode:: },
	//{ GLFW_KEY_F15			, EKeyCode:: },
	//{ GLFW_KEY_F16			, EKeyCode:: },
	//{ GLFW_KEY_F17			, EKeyCode:: },
	//{ GLFW_KEY_F18			, EKeyCode:: },
	//{ GLFW_KEY_F19			, EKeyCode:: },
	//{ GLFW_KEY_F20			, EKeyCode:: },
	//{ GLFW_KEY_F21			, EKeyCode:: },
	//{ GLFW_KEY_F22			, EKeyCode:: },
	//{ GLFW_KEY_F23			, EKeyCode:: },
	//{ GLFW_KEY_F24			, EKeyCode:: },
	//{ GLFW_KEY_F25			, EKeyCode:: },
	{ GLFW_KEY_KP_0			, EKeyCode::NUM_0 },
	{ GLFW_KEY_KP_1			, EKeyCode::NUM_1 },
	{ GLFW_KEY_KP_2			, EKeyCode::NUM_2 },
	{ GLFW_KEY_KP_3			, EKeyCode::NUM_3 },
	{ GLFW_KEY_KP_4			, EKeyCode::NUM_4 },
	{ GLFW_KEY_KP_5			, EKeyCode::NUM_5 },
	{ GLFW_KEY_KP_6			, EKeyCode::NUM_6 },
	{ GLFW_KEY_KP_7			, EKeyCode::NUM_7 },
	{ GLFW_KEY_KP_8			, EKeyCode::NUM_8 },
	{ GLFW_KEY_KP_9			, EKeyCode::NUM_9 },
	{ GLFW_KEY_KP_DECIMAL	, EKeyCode::NUM_DECIMAL },
	{ GLFW_KEY_KP_DIVIDE	, EKeyCode::NUM_DIVIDE },
	{ GLFW_KEY_KP_MULTIPLY	, EKeyCode::NUM_MULTIPLY },
	{ GLFW_KEY_KP_SUBTRACT	, EKeyCode::NUM_SUBTRACT },
	{ GLFW_KEY_KP_ADD		, EKeyCode::NUM_ADD },
	{ GLFW_KEY_KP_ENTER		, EKeyCode::NUM_ENTER },
	//{ GLFW_KEY_KP_EQUAL		, EKeyCode:: },
	{ GLFW_KEY_LEFT_SHIFT	, EKeyCode::LEFT_SHIFT },
	{ GLFW_KEY_LEFT_CONTROL	, EKeyCode::LEFT_CTRL },
	{ GLFW_KEY_LEFT_ALT		, EKeyCode::ALT },
	//{ GLFW_KEY_LEFT_SUPER 	, EKeyCode:: },
	{ GLFW_KEY_RIGHT_SHIFT	, EKeyCode::RIGHT_SHIFT },
	{ GLFW_KEY_RIGHT_CONTROL, EKeyCode::RIGHT_CTRL },
	{ GLFW_KEY_RIGHT_ALT	, EKeyCode::ALT_GR },
	//{ GLFW_KEY_RIGHT_SUPER	, EKeyCode:: },
	{ GLFW_KEY_MENU			, EKeyCode::MENU }
};


DKeyEvent::DKeyEvent(int32 InKey, int32 InAction, int32 InMods)
	: Key(InKey), Action(InAction), Mods(InMods)
{
}


EKeyCode DKeyEvent::GetKey()
{
	EKeyCode* code = KeyMap.Find(this->Key);
	if (code) 
		return *code;
	else 
		return EKeyCode::UNKNOWN;
}


EKeyAction DKeyEvent::GetAction()
{
	switch (this->Action)
	{
	case GLFW_PRESS: return EKeyAction::PRESS;
	case GLFW_RELEASE: return EKeyAction::RELEASE;
	case GLFW_REPEAT: return EKeyAction::REPEAT;
	default: return EKeyAction::UNKNOWN;
	}
}


bool DKeyEvent::IsShiftDown()
{
	return IsBitSet(this->Mods, GLFW_MOD_SHIFT);
}


bool DKeyEvent::IsCtrlDown()
{
	return IsBitSet(this->Mods, GLFW_MOD_CONTROL);
}


bool DKeyEvent::IsAltDown()
{
	return IsBitSet(this->Mods, GLFW_MOD_ALT);
}


bool DKeyEvent::IsCapsLockSet()
{
	return IsBitSet(this->Mods, GLFW_MOD_CAPS_LOCK);
}


bool DKeyEvent::IsNumLockSet()
{
	return IsBitSet(this->Mods, GLFW_MOD_NUM_LOCK);
}


DMouseButtonEvent::DMouseButtonEvent(int32 InButton, int32 InAction, int32 InMods)
	: Button(InButton), Action(InAction), Mods(InMods)
{
}


EMouseButton DMouseButtonEvent::GetKey()
{
	switch (this->Button)
	{
	case GLFW_MOUSE_BUTTON_LEFT: return EMouseButton::LEFT;
	case GLFW_MOUSE_BUTTON_RIGHT: return EMouseButton::RIGHT;
	case GLFW_MOUSE_BUTTON_MIDDLE: return EMouseButton::MIDDLE;
	default: return EMouseButton::UNKNOWN;
	}
}


EMouseAction DMouseButtonEvent::GetAction()
{
	switch (this->Action)
	{
	case GLFW_PRESS: return EMouseAction::PRESS;
	case GLFW_RELEASE: return EMouseAction::RELEASE;
	default: return EMouseAction::UNKNOWN;
	}
}


bool DMouseButtonEvent::IsShiftDown()
{
	return IsBitSet(this->Mods, GLFW_MOD_SHIFT);
}


bool DMouseButtonEvent::IsCtrlDown()
{
	return IsBitSet(this->Mods, GLFW_MOD_CONTROL);
}


bool DMouseButtonEvent::IsAltDown()
{
	return IsBitSet(this->Mods, GLFW_MOD_ALT);
}


DScrollEvent::DScrollEvent(float InX, float InY) 
	: X(InX), Y(InY)
{
}


float DScrollEvent::GetX()
{
	return this->X;
}


float DScrollEvent::GetY()
{
	return this->Y;
}


DMouseMotionEvent::DMouseMotionEvent(float InX, float InY)
	: X(InX), Y(InY)
{
}

float DMouseMotionEvent::GetX()
{
	return this->X;
}


float DMouseMotionEvent::GetY()
{
	return this->Y;
}


HWindow3D::HWindow3D(int32 InWidth, int32 InHeight, DString8 InName, bool InHidden)
{
	if (glfwInit() == false) 
	{
		SLog::Error("GLFW init error");
		return;
	}

	glfwWindowHint(GLFW_VISIBLE, InHidden? GLFW_FALSE : GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

	this->GLFWwindowPtr = glfwCreateWindow(InWidth, InHeight, InName.CString(), NULL, NULL);
	if (this->GLFWwindowPtr == nullptr)
	{
		glfwTerminate();
		SLog::Error("GLFW window creation error");
		return;
	}
	
	glfwMakeContextCurrent(this->GLFWwindowPtr);
	glfwSwapInterval(1);  // Enable vsync

	IMGUI_CHECKVERSION();
	this->ImGuiCtx = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(this->GLFWwindowPtr, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	
	if (glewInit() != GLEW_OK)
	{
		SLog::Error("Glew Init Error");
		return;
	}

	glfwSetWindowUserPointer(this->GLFWwindowPtr, this);
	glfwSetKeyCallback(this->GLFWwindowPtr, &HWindow3D::KeyCallback);
	glfwSetCharCallback(this->GLFWwindowPtr, &HWindow3D::CharCallback);
	glfwSetCursorPosCallback(this->GLFWwindowPtr, &HWindow3D::CursorPosCallback);
	glfwSetMouseButtonCallback(this->GLFWwindowPtr, &HWindow3D::MouseButtonCallback);
	glfwSetScrollCallback(this->GLFWwindowPtr, &HWindow3D::ScrollCallback);
	glfwSetWindowPosCallback(this->GLFWwindowPtr, &HWindow3D::WindowPosCallback);
	glfwSetFramebufferSizeCallback(this->GLFWwindowPtr, &HWindow3D::WindowSizeCallback);
	glfwSetWindowIconifyCallback(this->GLFWwindowPtr, &HWindow3D::WindowIconifyCallback);
	glfwSetWindowCloseCallback(this->GLFWwindowPtr, &HWindow3D::WindowCloseCallback);
	glfwSetDropCallback(this->GLFWwindowPtr, &HWindow3D::DropCallback);
}


HWindow3D::~HWindow3D()
{
	glfwSetWindowUserPointer(this->GLFWwindowPtr, nullptr);
	glfwSetKeyCallback(this->GLFWwindowPtr, nullptr);
	glfwSetCharCallback(this->GLFWwindowPtr, nullptr);
	glfwSetCursorPosCallback(this->GLFWwindowPtr, nullptr);
	glfwSetMouseButtonCallback(this->GLFWwindowPtr, nullptr);
	glfwSetScrollCallback(this->GLFWwindowPtr, nullptr);
	glfwSetWindowPosCallback(this->GLFWwindowPtr, nullptr);
	glfwSetFramebufferSizeCallback(this->GLFWwindowPtr, nullptr);
	glfwSetWindowIconifyCallback(this->GLFWwindowPtr, nullptr);
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(this->ImGuiCtx);
	
	glfwSetWindowShouldClose(this->GLFWwindowPtr, 1);
	glfwDestroyWindow(this->GLFWwindowPtr);
	glfwTerminate();
}


void HWindow3D::StartRender()
{
	this->OnRenderStart.Call();

	// render loop
	while (glfwWindowShouldClose(this->GLFWwindowPtr) == false)
	{
		if (this->Iconified == false)
		{
			// draw scene
			if (this->Paused == false)
			{
				this->OnRenderFrame.Call();
			}
			
			// draw ui
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			this->OnRenderUI.Call();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			//
			glfwSwapBuffers(this->GLFWwindowPtr);
		}

		// needs to be called no matter what otherwise the window dies
		glfwPollEvents();
	}
		
	this->OnRenderStop.Call();
}


void HWindow3D::StopRender()
{
	glfwSetWindowShouldClose(this->GLFWwindowPtr, true);
}


void HWindow3D::KeyCallback(GLFWwindow* InGLFWwindow, int32 Key, int32 Scancode, int32 Action, int32 Mods) 
{
	if (Key >= 0 && Key < 512)
	{
		ImGui::GetIO().KeysDown[Key] = Action == GLFW_PRESS || Action == GLFW_REPEAT;
	}

	if (ImGui::GetIO().WantCaptureKeyboard) return;

	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnKey.IsBound())
	{
		window->OnKey.Call(DKeyEvent(Key, Action, Mods));
	}	
}


void HWindow3D::CharCallback(GLFWwindow* InGLFWwindow, uint32 CodePoint)
{
	ImGui::GetIO().AddInputCharacter(CodePoint);
	
	if (ImGui::GetIO().WantCaptureKeyboard) return;

	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnCharTyped.IsBound())
	{
		window->OnCharTyped.Call((tchar) CodePoint);
	}
}


void HWindow3D::CursorPosCallback(GLFWwindow* InGLFWwindow, double X, double Y)
{
	if (ImGui::GetIO().WantCaptureMouse) return;

	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnMouseMotion.IsBound())
	{
		window->MouseDeltaX = (float)X - window->PrevMouseX;
		window->MouseDeltaY = (float)Y - window->PrevMouseY;
		window->PrevMouseX = (float)X;
		window->PrevMouseY = (float)Y;
		window->OnMouseMotion.Call(DMouseMotionEvent((float)X, (float)Y));
	}
}


void HWindow3D::MouseButtonCallback(GLFWwindow* InGLFWwindow, int32 Button, int32 Action, int32 Mods)
{
	if (ImGui::GetIO().WantCaptureMouse) return;

	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnMouseButton.IsBound())
	{
		window->OnMouseButton.Call(DMouseButtonEvent(Button, Action, Mods));
	}
}


void HWindow3D::ScrollCallback(GLFWwindow* InGLFWwindow, double X, double Y)
{
	if (ImGui::GetIO().WantCaptureMouse) 
	{
		ImGui::GetIO().MouseWheel = (float)Y;
		return;
	}

	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnScroll.IsBound())
	{
		window->OnScroll.Call(DScrollEvent((float)X, (float)Y));
	}
}


void HWindow3D::WindowPosCallback(GLFWwindow* InGLFWwindow, int32 X, int32 Y)
{
	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnWindowPos.IsBound())
	{
		window->OnWindowPos.Call(X, Y);
	}
}


void HWindow3D::WindowSizeCallback(GLFWwindow* InGLFWwindow, int32 Width, int32 Height)
{
	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnWindowSize.IsBound())
	{
		window->OnWindowSize.Call(Width, Height);
	}
}


void HWindow3D::WindowIconifyCallback(GLFWwindow* InGLFWwindow, int32 Bool)
{
	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnWindowSize.IsBound())
	{
		window->Iconified = Bool;
		window->OnWindowIconified.Call(Bool);
	}
}


void HWindow3D::WindowCloseCallback(GLFWwindow* InGLFWwindow)
{
	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnWindowCloseRequest.IsBound())
	{
		bool canClose = window->OnWindowCloseRequest.Call();
		if (canClose == false)
		{
			glfwSetWindowShouldClose(InGLFWwindow, false);
		}
	}
}


void HWindow3D::DropCallback(GLFWwindow* InGLFWwindow, int32 Count, const char** Paths)
{
	HWindow3D* window = reinterpret_cast<HWindow3D*>(glfwGetWindowUserPointer(InGLFWwindow));
	if (window && window->OnFileDrop.IsBound())
	{
		DArray<DString> filepaths;
		filepaths.Reserve(Count);
		for (int32 i = 0; i < Count; ++i)
		{
			filepaths.Add(DStrings::UTF8ToUTF16(Paths[i]));
		}
		window->OnFileDrop.Call(filepaths);
	}
}


void HWindow3D::GetMousePos(float& OutX, float& OutY) const
{
	double x, y;
	glfwGetCursorPos(this->GLFWwindowPtr, &x, &y);
	OutX = (float)x;
	OutY = (float)y;
}


void HWindow3D::GetMouseDelta(float& OutX, float& OutY) const
{
	OutX = this->MouseDeltaX;
	OutY = this->MouseDeltaY;
}


ECursorMode HWindow3D::GetCursorMode() const
{
	return this->CursorMode;
}


void HWindow3D::SetCursorMode(ECursorMode Mode)
{
	switch (Mode)
	{
	case ECursorMode::NORMAL:
		glfwSetInputMode(this->GLFWwindowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case ECursorMode::HIDDEN:
		glfwSetInputMode(this->GLFWwindowPtr, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case ECursorMode::HIDDEN_AND_LOCKED:
		glfwSetInputMode(this->GLFWwindowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	}
}


void HWindow3D::GetPos(int32& OutX, int32& OutY) const
{
	glfwGetWindowPos(this->GLFWwindowPtr, &OutX, &OutY);
}


void HWindow3D::GetSize(int32& OutWidth, int32& OutHeight) const
{
	glfwGetWindowSize(this->GLFWwindowPtr, &OutWidth, &OutHeight);
}


void HWindow3D::SetPos(int32 NewX, int32 NewY)
{
	glfwSetWindowPos(this->GLFWwindowPtr, NewX, NewY);
}


void HWindow3D::SetPosIncludingFrame(int32 NewX, int32 NewY)
{
	int32 left, top, right, bottom = 0;
	glfwGetWindowFrameSize(this->GLFWwindowPtr, &left, &top, &right, &bottom);
	glfwSetWindowPos(this->GLFWwindowPtr, NewX + left, NewY + top);
}


void HWindow3D::SetSize(int32 NewWidth, int32 NewHeight)
{
	glfwSetWindowSize(this->GLFWwindowPtr, NewWidth, NewHeight);
}


void HWindow3D::SetSizeIncludingFrame(int32 NewWidth, int32 NewHeight)
{
	int32 left, top, right, bottom = 0;
	glfwGetWindowFrameSize(this->GLFWwindowPtr, &left, &top, &right, &bottom);
	glfwSetWindowSize(this->GLFWwindowPtr, NewWidth - left - right, NewHeight - top - bottom);
}


bool HWindow3D::GetShouldClose()
{
	return glfwWindowShouldClose(this->GLFWwindowPtr);
}


bool HWindow3D::IsIconified()
{
	return this->Iconified;
}


void HWindow3D::SetIconified(bool Value)
{
	this->Iconified = Value;
	if (Value)
	{
		glfwIconifyWindow(this->GLFWwindowPtr);
	}
	else
	{
		glfwRestoreWindow(this->GLFWwindowPtr);
	}
}


void HWindow3D::SetShouldClose(bool Value)
{
	glfwSetWindowShouldClose(this->GLFWwindowPtr, Value);
}


void HWindow3D::GetOpenGLVersion(int32& OutMajor, int32& OutMinor)
{
	GLint majorVersion = -1;
	GLint minorVersion = -1;
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	OutMajor = majorVersion;
	OutMinor = minorVersion;
}


void HWindow3D::SetUseVSync(bool Value)
{
	glfwSwapInterval((int32)Value);
}


void HWindow3D::SetPaused(bool Value)
{
	this->Paused = Value;
}


bool HWindow3D::IsPaused()
{
	return this->Paused;
}