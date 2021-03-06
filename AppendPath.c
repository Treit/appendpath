#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <strsafe.h>
#include "AppendPath.h"

/////////////////////////////////////////////////////////////////////////
//
//   Name:      AppendPath
//   Purpose:   Modifies the system path environment variable by adding
//              the specified text to the end of the existing path.
//   Created:   5/14/2000
//   By:        mtreit
//
/////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    #define BUFFERSIZE 1024 * 4

    LONG lResult;
    DWORD dwBufSize = BUFFERSIZE;
    DWORD dwReturnValue;
    LPTSTR lpszSearchResult;
    LPTSTR lpszArg;
    LPTSTR lpszCurrPath;
    LPTSTR lpszNewPath;
    LPTSTR lpszKeyName = _TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
    LPTSTR lpszValueName = _TEXT("Path");

    TCHAR **arglist;

    HKEY hkRoot = HKEY_LOCAL_MACHINE;
    HKEY hk;

    TCHAR szPath[BUFFERSIZE];
    TCHAR argBuffer[BUFFERSIZE];

    if (argc < 2)
    {
        printf("Missing argument.");
        exit(1);
    }

    #ifdef _UNICODE
        arglist = CommandLineToArgvW(GetCommandLine(), &argc);
    #else
        arglist = argv;
    #endif

    lpszArg = concatArgs(argBuffer, BUFFERSIZE, argc, arglist);

    if (_tcscmp(lpszArg,_TEXT("/?")) == 0)
    {
        printf("\nAppends the specified string to the system Path environment variable.\n\n");
        printf("Examples:   %s D:\\Program Files\\Tools\n",argv[0]);
        printf("            %s F:\\Tools\n",argv[0]);
        exit(0);
    }

    // Read the current path from the registry.
    lResult = RegOpenKeyEx(
        hkRoot,
        lpszKeyName,
        0,
        KEY_ALL_ACCESS,
        &hk);

    if (!(lResult == ERROR_SUCCESS))
    {
        printf("Error opening the registry. The error code was %i.", lResult);
        lResult = RegCloseKey(hk);
        exit(1);
    }

    // Get the current path.
    lResult = RegQueryValueEx(
        hk,
        lpszValueName,
        NULL,
        NULL,
        (LPBYTE) szPath,
        &dwBufSize);

    if (!(lResult == ERROR_SUCCESS))
    {
        printf("Error reading the path value. The error code was %i.",lResult);
        lResult = RegCloseKey(hk);
        exit(1);
    }

    lpszCurrPath = szPath;

    // Check if the argument is already part of the Path
    lpszSearchResult = stristrW(lpszCurrPath, lpszArg);
    
    if (!(lpszSearchResult == NULL))
    {
        printf("\nThe parameter is already part of the path and will not be added.\n");
        lResult = RegCloseKey(hk);
        exit(0);
    }

    // Check if we need to prepend an apostrophe before adding the new string.
    if (lpszCurrPath[_tcslen(lpszCurrPath) - 1] == _TEXT(';'))
    {
        // Found a trailing apostrophe, so just concatenate the strings.
        StringCchCat(lpszCurrPath, BUFFERSIZE, lpszArg);
    }
    else
    {
        // Did not find a trailing apostrophe, so add one.
        StringCchCat(lpszCurrPath, BUFFERSIZE, _TEXT(";"));
        StringCchCat(lpszCurrPath, BUFFERSIZE, lpszArg);
    }

    // Update the key in the registry.
    lResult = 0;

    lResult = RegSetValueEx(
        hk,
        lpszValueName,
        0,
        REG_EXPAND_SZ,
        (LPBYTE)lpszCurrPath,
        _tcslen(lpszCurrPath)  * sizeof(TCHAR));

    if (!(lResult == ERROR_SUCCESS))
    {
        printf("Error writing the registry value. The error code was %i.", lResult);
        lResult = RegCloseKey(hk);
        exit(1);
    }

    _tprintf(_TEXT("\nAppended %s to the path environment variable.\n"), lpszArg);

    lResult = RegCloseKey(hk);

    // Broadcast a message informing the system of the change.
    lResult = SendMessageTimeout(
        HWND_BROADCAST,
        WM_SETTINGCHANGE,
        0,
        (LPARAM) _TEXT("Environment"),
        SMTO_ABORTIFHUNG,
        5000,
        &dwReturnValue);

    return 0;
}

TCHAR *concatArgs(TCHAR *pBuffer, int bufferSize, int iArgs, TCHAR **pArgs)
{
    // Concatenate all parameters into one string, so the user doesn't need to use quotes.
    int i;

    // Prefill the buffer with nulls.
    _tcsncpy(pBuffer, _TEXT("\0"), bufferSize - 1);

    // For each argument, add it to the buffer.
    for (i = 1; i < iArgs; i++)
    {
        if (i > 1)
        {
            // Add a space between arguments if this is not the first one.
            StringCchCat(pBuffer, bufferSize, _TEXT(" "));
        }

        int ret = StringCchCat(pBuffer, bufferSize, pArgs[i]);
    }

    return pBuffer;
}
