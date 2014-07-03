#include <stdio.h>
#include "Common.h"

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 

BOOL IsWow64()
{
#ifndef RT_ARCH_AMD64
	BOOL ret = FALSE;
	BOOL bIsWow64;
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	HMODULE hKer = NULL;
	hKer = LoadLibrary(TEXT("kernel32"));
	if (!hKer)
		return FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hKer ,"IsWow64Process"); 
	if (!fnIsWow64Process) {
		goto out;
	}
	if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		goto out;
	ret = bIsWow64;
out:
	if (hKer)
		FreeLibrary(hKer);
	return ret;
#else
	return FALSE;
#endif
}

void DestroyDevInfo(HDEVINFO info)
{
	if (info)
		SetupDiDestroyDeviceInfoList(info);
}

HDEVINFO GetDevInfoFromDeviceId(SP_DEVINFO_DATA *dev_info_data, CHAR *device_id)
{
	HDEVINFO dev_info;
	SP_DEVINFO_DATA data;
	UINT i;
	BOOL found;
	CHAR *buffer;
	UINT buffer_size = 8092;
	DWORD required_size;
	DWORD data_type;

	dev_info = SetupDiGetClassDevsEx(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT, NULL, NULL, NULL);
	if (dev_info == NULL)
	{
		return NULL;
	}

#if 0
	SP_DEVINFO_LIST_DETAIL_DATA detail_data;
	memset(&detail_data, 0, sizeof(detail_data));
	detail_data.cbSize = sizeof(detail_data);
	if (SetupDiGetDeviceInfoListDetail(dev_info, &detail_data) == FALSE)
	{
		DestroyDevInfo(dev_info);
		return NULL;
	}
#endif

	memset(&data, 0, sizeof(data));
	data.cbSize = sizeof(data);
	found = FALSE;
	buffer = (LPTSTR)LocalAlloc(LPTR, buffer_size);
	if (!buffer) {
		printf("Alloc: %x\n", GetLastError());
		goto out;
	}
	
	for (i = 0;SetupDiEnumDeviceInfo(dev_info, i, &data); i++) {
		while (!SetupDiGetDeviceRegistryProperty(dev_info,
												&data,
												SPDRP_HARDWAREID,
												&data_type,
												(PBYTE)buffer,
												buffer_size,
												&required_size)) {
			if (ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
				// Change the buffer size.
				if (buffer) {
					LocalFree(buffer);
				}
				// Double the size to avoid problems on 
				// W2k MBCS systems per KB 888609. 
				buffer_size *= 2;
            	buffer = (LPTSTR)LocalAlloc(LPTR, buffer_size);
				if (!buffer) {
					printf("LocalAlloc: %x\n", GetLastError());
					goto out;
				}
			} else {
				// EnumNext
				break;
			}
		}

		if (stricmp(buffer, device_id) == 0) {
			found = TRUE;
		}

		if (found) {
			goto out;
		}

		memset(&data, 0, sizeof(data));
		data.cbSize = sizeof(data);
	}
out:
	if (buffer)
		LocalFree(buffer);
	if (found == FALSE) {
		DestroyDevInfo(dev_info);
		return NULL;
	} else {
		memcpy(dev_info_data, &data, sizeof(data));
		return dev_info;
	}
}

BOOL GetDevStatus(HDEVINFO h, SP_DEVINFO_DATA *dev_info_data, PULONG status, PULONG problem)
{
	SP_DEVINFO_LIST_DETAIL_DATA detail;

	memset(&detail, 0, sizeof(detail));
	detail.cbSize = sizeof(detail);
	
	SetupDiGetDeviceInfoListDetail(h, &detail);
	if (CM_Get_DevNode_Status_Ex(status,
								problem,
								dev_info_data->DevInst,
								0,
								detail.RemoteMachineHandle) != CR_SUCCESS) {
	
		printf("SetupDiGetDeviceInfoListDetail: %x\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Source code from : http://www.codeproject.com/Tips/333559/CreateProcess-and-wait-for-result
// Executes the given command using CreateProcess() and WaitForSingleObject().
// Returns FALSE if the command could not be executed or if the exit code could not be determined.

BOOL executeCommandLine(char *cmdLine, DWORD & exitCode)
{
   PROCESS_INFORMATION processInformation = {0};
   STARTUPINFO startupInfo                = {0};
   startupInfo.cb                         = sizeof(startupInfo);
//   int nStrBuffer                         = cmdLine.GetLength() + 50;
 

   // Create the process
   BOOL result = CreateProcess(NULL, cmdLine, 
                               NULL, NULL, FALSE, 
                               NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 
                               NULL, NULL, &startupInfo, &processInformation);

   if (!result)
   {
      // CreateProcess() failed
      // Get the error from the system
      LPVOID lpMsgBuf;
      DWORD dw = GetLastError();
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
 
      // Display the error
	printf("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n", cmdLine, (LPTSTR) lpMsgBuf);
 
	// Free resources created by the system
	LocalFree(lpMsgBuf);
 
	// We failed.
      return FALSE;
   }
   else
   {
      // Successfully created the process.  Wait for it to finish.
      WaitForSingleObject( processInformation.hProcess, INFINITE );
 
      // Get the exit code.
      result = GetExitCodeProcess(processInformation.hProcess, &exitCode);
 
      // Close the handles.
      CloseHandle( processInformation.hProcess );
      CloseHandle( processInformation.hThread );
 
      if (!result)
      {
         // Could not get exit code.
         printf("Executed command but couldn't get exit code.\nCommand=%s\n", cmdLine);
         return FALSE;
      }
 

      // We succeeded.
      return TRUE;
   }
}

#include <stdio.h>
#include <windows.h>

int rebootSystem(void)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
	// Get a token for this process. 
 
	if (!OpenProcessToken(GetCurrentProcess(), 
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		printf("OpenProcessToken"); 
		return 0;
	}
 
	// Get the LUID for the shutdown privilege. 
 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
			&tkp.Privileges[0].Luid); 
 
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
	// Get the shutdown privilege for this process. 
 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
			(PTOKEN_PRIVILEGES)NULL, 0); 
 
	// Cannot test the return value of AdjustTokenPrivileges. 
 
	if (GetLastError() != ERROR_SUCCESS) 
	{
		printf("AdjustTokenPrivileges"); 
		return 0;
	}

	BOOL b = ExitWindowsEx(	EWX_REBOOT|EWX_FORCE, 
							SHTDN_REASON_MINOR_MAINTENANCE | SHTDN_REASON_FLAG_PLANNED);

	DWORD dwerr = 0;
	dwerr = GetLastError();
	return 0;
}