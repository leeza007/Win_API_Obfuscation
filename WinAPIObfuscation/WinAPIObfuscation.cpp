#include "pch.h"
#include <iostream>
#include "hash_work.h"
#include <vector>
#include <string>
using namespace std;

inline void log()
{
}

template <typename First, typename ...Rest>
void log(First&& message, Rest&& ...rest)
{
	std::cout << std::forward<First>(message) << std::endl;
	log(std::forward<Rest>(rest)...);
}

typedef int(WINAPI* ShellAboutProc)(HWND, LPCSTR, LPCSTR, HICON);

int main()
{
	// CreateFileA
	HANDLE hFile = hash_CreateFileA("log.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile) {
		log("[OK] CreateFileA");
	}
	else if (hFile == INVALID_HANDLE_VALUE) {
		log("[FAIL] CreateFileA");
	}

	// VirtualAlloc/VirtualFree/VirtualProtect
	LPVOID p = nullptr;

	p = hash_VirtualAlloc(nullptr, 4000, MEM_RESERVE, PAGE_READWRITE);

	bool vp = hash_VirtualProtect(p, 0, PAGE_EXECUTE_READWRITE, 0); // work

	if (p != nullptr)
	{
		log("[OK] VirtualAlloc");
		if (hash_VirtualFree(p, 0, MEM_RELEASE)){
			log("[OK] VirtualFree");
		}
		else{
			log("[FAIL] VirtualFree");
		}
	} else {
		log("[FAIL] VirtualAlloc");
	}
	// -----------------------

	// VirtualAllocEx
	LPVOID p1 = nullptr;
	p1 = hash_VirtualAllocEx(GetCurrentProcess(), nullptr, 4000, MEM_RESERVE, PAGE_READWRITE);
	if (p1 != nullptr)
	{
		log("[OK] VirtualAllocEx");
		if (hash_VirtualFreeEx(GetCurrentProcess(),p, 0, MEM_RELEASE)) {
			log("[OK] VirtualFreeEx");
		}
		else {
			log("[FAIL] VirtualFreeEx");
		}
	}
	else {
		log("[FAIL] VirtualAllocEx");
	}
	// -----------------------

	// QueryDosDeviceW

	// Query these on my machine, using unicode/wide character
	// All the drives available in the tested machine except B:, A: is floppy and K: is CDRW

	LPCWSTR lpDeviceName[7] = { L"A:", L"B:", L"C:", L"E:", L"F:", L"G:", L"K:" };
	//The buffer for storage
	TCHAR lpTargetPath[1000];
	int i, j;
	DWORD test;
	// For the drives/devices...
	for (i = 0; i <= 6; i++)
	{
		// If the function succeeds, the return value is the number of TCHARs stored
		// into the buffer pointed to by lpTargetPath. If the function fails, the return
		// value is zero. To get extended error information, call GetLastError.
		// If the buffer is too small, the function fails and the last error code
		// is ERROR_INSUFFICIENT_BUFFER.

		test = hash_QueryDosDeviceW(lpDeviceName[i], (LPWSTR)lpTargetPath, 1000);

		// Test the return value and error if any
		printf("QueryDosDevice() return value: %d, Error: %d\n", test, GetLastError());
		printf("The DOS device name for %S is: ", lpDeviceName[i]);

		// Display the result of the device name

		for (j = 0; j < 30; j++)
			printf("%c", lpTargetPath[j]);
		printf("\n");

	}
	printf("\n");

	// -----------------------

	// pGetDiskFreeSpaceEx
	BOOL  fResult;
	char  *pszDrive  = NULL;

	unsigned __int64 i64FreeBytesToCaller,
		 i64TotalBytes,
		 i64FreeBytes;

	fResult = hash_GetDiskFreeSpaceExW((LPCWSTR)pszDrive,
		 (PULARGE_INTEGER)& i64FreeBytesToCaller,
		 (PULARGE_INTEGER)& i64TotalBytes,
		 (PULARGE_INTEGER)& i64FreeBytes); // If the function succeeds, the return value is nonzero
	// -----------------------

	// LoadLibraryW
	 auto LoadLibraryW_ = hash_LoadLibraryW(L"user32.dll"); //If the function succeeds, the return value is a handle to the module.
	// -----------------------

	// GetModuleHandleExW
	 char path[MAX_PATH];
	 HMODULE hm = NULL;
	 int functionInThisDll;

	 if (hash_GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		 GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		 (LPCWSTR)& functionInThisDll, &hm) == 0)
	 {
		 int ret = GetLastError();
		 fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
		 // Return or however you want to handle an error.
	 }
	// -----------------------

	// GetModuleFileNameW
	 vector<wchar_t> pathBuf;
	 DWORD copied = 0;
	 do {
		 pathBuf.resize(pathBuf.size() + MAX_PATH);
		 copied = hash_GetModuleFileNameW(0, &pathBuf.at(0), pathBuf.size());
	 } while (copied >= pathBuf.size());

	 pathBuf.resize(copied);

	 std::string path2(pathBuf.begin(), pathBuf.end());

	 std::cout << path2;

	// -----------------------

	// GetModuleHandle
	 HMODULE hKernel32 = NULL;
	 hKernel32 = hash_GetModuleHandleA("Kernel32");
	// -----------------------

	// GetProcAddress
	HMODULE hModule = LoadLibrary(TEXT("Shell32.dll"));

	ShellAboutProc shellAbout = (ShellAboutProc)hash_GetProcAddress(hModule, "ShellAboutA");

	//shellAbout(NULL, "hello", "world", NULL);

	FreeLibrary(hModule);
	// -----------------------
	// GetModuleHandleW
	HMODULE hKernel32w = NULL;
	hKernel32w = hash_GetModuleHandleW(L"Kernel32.dll");
	// -----------------------
	// GetStdHandle
	HANDLE hStdout = hash_GetStdHandle(STD_INPUT_HANDLE);
	// -----------------------
	// GetConsoleScreenBufferInfo
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE hstdout2;
	hstdout2 = hash_GetStdHandle(STD_OUTPUT_HANDLE);

	bool gcsbi = hash_GetConsoleScreenBufferInfo(hstdout2, &csbiInfo);

	if (!gcsbi) {
		cout << "Error: " << GetLastError() << endl;
	}
	// -----------------------
	// SetConsoleTextAttribute
	bool scta = hash_SetConsoleTextAttribute(hstdout2, FOREGROUND_GREEN);

	if (!scta) {
		printf("Error hash_SetConsoleTextAttribute\n");
	} else {
		printf("\r\nGreen text\r\n");
	}
	// -----------------------
	// GetTickCount
	DWORD gt = hash_GetTickCount();
	// -----------------------
	// VerifyVersionInfoW
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = HIBYTE(_WIN32_WINNT_WINXP);
	osvi.dwMinorVersion = LOBYTE(_WIN32_WINNT_WINXP);
	osvi.wServicePackMajor = 0;

	bool vviw= hash_VerifyVersionInfoW((LPOSVERSIONINFOEXA)&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask);
	// -----------------------
	// GetSystemWindowsDirectoryW
	TCHAR buff[MAX_PATH * 2];
	buff[0] = 0;
	hash_GetSystemWindowsDirectoryW(buff, sizeof(buff));
	// -----------------------
	// GetWindowsDirectoryW
	TCHAR buff2[MAX_PATH * 2];
	buff2[0] = 0;
	hash_GetWindowsDirectoryW(buff2, sizeof(buff2));
	// -----------------------
	// GetSystemDirectoryW
	TCHAR buff3[MAX_PATH * 2];
	buff3[0] = 0;
	hash_GetSystemDirectoryW(buff3, sizeof(buff3));
	// -----------------------
	// GetSystemDirectoryA
	char buff4[MAX_PATH * 2];
	buff4[0] = 0;
	hash_GetSystemDirectoryA(buff4, sizeof(buff4));
	// -----------------------
	// GetModuleHandleExW
	// -----------------------
	// GetModuleHandleExW
	// -----------------------
	// GetModuleHandleExW
	// -----------------------
	// GetModuleHandleExW
	// -----------------------

	system("pause");

	return 1;
}