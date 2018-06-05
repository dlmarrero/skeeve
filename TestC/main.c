#include <windows.h>
#include <initguid.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <Lmcons.h>
#include <wchar.h>
#include <VersionHelpers.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")


int wmain(void) {
	// Screen size
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);

	wprintf(L"The screen size is: %dx%d\n", x, y);

	// Hostname
	wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);

	int cnameResult = GetComputerNameW(computerName, &size);

	if (cnameResult == 0) {
		wprintf(L"Failed to get computer name %ld", GetLastError());
		return 1;
	}

	wprintf(L"Computer name: %ls\n", computerName);

	// Username
	wchar_t username[UNLEN + 1];
	DWORD len = sizeof(username) / sizeof(wchar_t);

	int unameResult = GetUserNameW(username, &len);

	if (unameResult == 0) {
		wprintf(L"Failed to get username %1d", GetLastError());
		return 1;
	}

	wprintf(L"User name: %ls\n", username);

	// Current working directory
	wchar_t pathBuf[MAX_PATH];
	DWORD pathResult = GetCurrentDirectoryW(MAX_PATH, pathBuf);
	wprintf(L"cwd is %ls\n", pathBuf);

	// Windows version (inaccurate)
	if (IsWindows10OrGreater()) {
		wprintf(L"Win 10+");
	}
	else if (IsWindows8Point1OrGreater()) {
		wprintf(L"This is Windows 8.1+\n");
	}
	else if (IsWindows8OrGreater()) {
		wprintf(L"This is Windows 8\n");
	}
	else if (IsWindows7OrGreater()) {
		wprintf(L"This is Windows 7\n");
	}
	else if (IsWindowsVistaOrGreater()) {
		wprintf(L"This is Windows Vista\n");
	}
	else if (IsWindowsXPOrGreater()) {
		wprintf(L"This is Windows XP\n");
	}

	// Memory usage
	MEMORYSTATUSEX mem = { 0 };

	mem.dwLength = sizeof(mem);

	int memResult = GlobalMemoryStatusEx(&mem);

	if (memResult == 0) {
		wprintf(L"Failed to memory status %ld", GetLastError());
		return 1;
	}

	wprintf(L"Memory in use: %ld percent\n", mem.dwMemoryLoad);
	wprintf(L"Total physical memory: %lldB\n", mem.ullTotalPhys);
	wprintf(L"Free physical memory: %lldB\n", mem.ullAvailPhys);
	wprintf(L"Total virtual memory: %lldB\n", mem.ullTotalVirtual);
	wprintf(L"Free virtual memory: %lldB\n", mem.ullAvailVirtual);

	// Known folders
	PWSTR path = NULL;

	HRESULT documents = SHGetKnownFolderPath(&FOLDERID_Documents, 0, NULL, &path);

	if (SUCCEEDED(documents)) {
		wprintf(L"%ls\n", path);
	}

	HRESULT startup = SHGetKnownFolderPath(&FOLDERID_CommonStartup, 0, NULL, &path);
	
	if (SUCCEEDED(startup)) {
		wprintf(L"%ls\n", path);
	}

	CoTaskMemFree(path);

	// Drive letters
	wchar_t LogicalDrives[MAX_PATH] = { 0 };
	DWORD driveResult = GetLogicalDriveStringsW(MAX_PATH, LogicalDrives);

	if (driveResult == 0) {
		wprintf(L"Failed to get drive names %ld", GetLastError());
		return 1;
	}

	if (driveResult > 0 && driveResult <= MAX_PATH) {

		wchar_t *SingleDrive = LogicalDrives;

		while (*SingleDrive) {
			wprintf(L"%ls\n", SingleDrive);

			SingleDrive += wcslen(SingleDrive) + 1;
		}
	}

	// Disk space
	unsigned __int64 freeCall,
		total,
		free;

	// TODO: iterate over drive letters collected
	int diskSpaceResult = GetDiskFreeSpaceExW(L"C:\\", (PULARGE_INTEGER)&freeCall,
		(PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);

	if (diskSpaceResult == 0) {

		wprintf(L"Failed to get free disk space %ld", GetLastError());
		return 1;
	}

	wprintf(L"Available space to caller: %I64u MB\n", freeCall / (1024 * 1024));
	wprintf(L"Total space: %I64u MB\n", total / (1024 * 1024));
	wprintf(L"Free space on drive: %I64u MB\n", free / (1024 * 1024));

	// CPU speed
	DWORD BufSize = MAX_PATH;
	DWORD mhz = MAX_PATH;
	HKEY key;

	long r = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &key);

	if (r != ERROR_SUCCESS) {

		wprintf(L"RegOpenKeyExW() failed %ld", GetLastError());
		return 1;
	}

	r = RegQueryValueExW(key, L"~MHz", NULL, NULL, (LPBYTE)&mhz, &BufSize);

	if (r != ERROR_SUCCESS) {

		wprintf(L"RegQueryValueExW() failed %ld", GetLastError());
		return 1;
	}

	wprintf(L"CPU speed: %lu MHz\n", mhz);

	r = RegCloseKey(key);

	if (r != ERROR_SUCCESS) {

		wprintf(L"Failed to close registry handle %ld", GetLastError());
		return 1;
	}

	return 0;
}