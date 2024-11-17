///
/// @file main.cpp
///
/// Entry point of the library.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <windows.h>

/// Entry point of the library.
///
/// @param instance Handle to DLL module.
/// @param reason Reason for calling function.
/// @param reserved Reserved.
///
/// @return When the system calls the DllMain function with the DLL_PROCESS_ATTACH value,
/// the function returns TRUE if it succeeds or FALSE if initialization fails.
/// If the return value is FALSE when DllMain is called because the process uses the
/// LoadLibrary function, LoadLibrary returns NULL. (The system immediately calls your
/// entry-point function with DLL_PROCESS_DETACH and unloads the DLL.) If the return value
/// is FALSE when DllMain is called during process initialization, the process terminates
/// with an error. To get extended error information, call GetLastError.
///
/// When the system calls the DllMain function with any value other than DLL_PROCESS_ATTACH,
/// the return value is ignored.
BOOL WINAPI DllMain(
	HINSTANCE instance, // handle to DLL module
	DWORD     reason,   // reason for calling function
	LPVOID    reserved)    // reserved
{
	(void) instance; // Unused parameter.
	(void) reserved; // Unused parameter.

	BOOL result{ TRUE };

	// Perform actions based on the reason for calling.
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return result;
}