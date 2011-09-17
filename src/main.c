/*
 * Copyright (c) 2011 Toni Spets <toni.spets@iki.fi>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <windows.h>
#include <stdio.h>
#include "../res/dialog.h"
#include "http.h"
#include "register.h"
#include <commctrl.h>

HWND hwnd_status;
HWND hwnd_settings;
HWND itm_settings;
HWND itm_ok;
HWND itm_cancel;
HANDLE open_settings;

HWND itm_url;
HWND itm_exe;
HWND itm_port;

void config_load();
void config_save();

/* guessed from game directory */
char *game = NULL;

extern char cfg_url[512];
extern char cfg_exe[32];
extern int cfg_port;

bool cncnet_parse_response(char *response, char *url, int *interval)
{
    return sscanf(response, "\"%512[^\"]\",%d", url, interval) == 2;
}

char *cncnet_build_request(const char *path, const char *method, const char *game, int port)
{
    static char request[512];
    snprintf(request, sizeof(request), "%s?type=dumb&method=%s&params[]=%s&params[]=%d", path, method, game, port);
    return request;
}

void CenterWindow(HWND hWnd)
{
    RECT rc;
    GetWindowRect(hWnd, &rc);
    SetWindowPos(hWnd, NULL, GetSystemMetrics(SM_CXSCREEN) / 2 - (rc.right - rc.left) / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - (rc.bottom - rc.top) / 2, rc.right - rc.left, rc.bottom - rc.top, 0);
}

DWORD WINAPI cncnet_connect(int ctx)
{
    HWND itm_status = GetDlgItem(hwnd_status, ITM_STATUS);
    int i;
    char response[512] = { 0 };
    PROCESS_INFORMATION pInfo;
    STARTUPINFOA sInfo;
    char url[512];
    int interval = 0;

    CenterWindow(hwnd_status);
    ShowWindow(hwnd_status, SW_SHOW);
    SetForegroundWindow(hwnd_status);

    config_load();

    /* give time to open settings */
    if (WaitForSingleObject(open_settings, 0) != WAIT_OBJECT_0)
    {
        for (i = 3; i > 0; i--)
        {
            char buf[128];
            sprintf(buf, "Connecting to CnCNet in %d seconds...", i);
            SetWindowText(itm_status, buf);

            if (WaitForSingleObject(open_settings, 1000) == WAIT_OBJECT_0)
            {
                break;
            }
        }

        EnableWindow(itm_settings, FALSE);

        if (WaitForSingleObject(open_settings, 0) == WAIT_OBJECT_0)
        {
            CenterWindow(hwnd_settings);
            ShowWindow(hwnd_status, SW_HIDE);
            ShowWindow(hwnd_settings, SW_SHOW);
            SetForegroundWindow(hwnd_settings);
            return 0;
        }
    }

    SetWindowText(itm_status, "Connecting to CnCNet...");

    http_init();

    if (http_download_mem(cncnet_build_request(cfg_url, "launch", game, cfg_port), response, sizeof(response)) && cncnet_parse_response(response, url, &interval) && url != NULL && interval > 0)
    {
        SetWindowText(itm_status, "Connected! Launching game...");

        /* launch gaem */
        ZeroMemory(&sInfo, sizeof(STARTUPINFO));
        sInfo.cb = sizeof(sInfo);
        ZeroMemory(&pInfo, sizeof(PROCESS_INFORMATION));

        snprintf(response, sizeof(response), "%s -LAN %s", cfg_exe, url);
        if (CreateProcess(NULL, response, NULL, NULL, TRUE, 0, NULL, NULL, &sInfo, &pInfo) != 0)
        {
            ShowWindow(hwnd_status, SW_HIDE);

            while (WaitForSingleObject(pInfo.hProcess, interval * 60 * 1000) != WAIT_OBJECT_0)
            {
                http_download_mem(cncnet_build_request(cfg_url, "ping", game, cfg_port), response, sizeof(response));
            }

            SetWindowText(itm_status, "Logging out...");
            CenterWindow(hwnd_status);
            ShowWindow(hwnd_status, SW_SHOW);
            SetForegroundWindow(hwnd_status);

            http_download_mem(cncnet_build_request(cfg_url, "logout", game, cfg_port), response, sizeof(response));
            PostMessage(hwnd_status, WM_COMMAND, 0, 0);
        }
        else
        {
            snprintf(response, sizeof(response), "Failed to launch %s", cfg_exe);
            SetWindowText(itm_status, response);
            http_download_mem(cncnet_build_request(cfg_url, "logout", game, cfg_port), response, sizeof(response));
        }
    }
    else
    {
        SetWindowText(itm_status, "Error connecting to CnCNet :-(");
    }

    http_release();

    return 0;
}

INT_PTR CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        return TRUE;
    }

    if (uMsg == WM_COMMAND && (lParam == 0 || lParam == (LPARAM)itm_cancel))
    {
        PostQuitMessage(0);
    }

    if (uMsg == WM_COMMAND && lParam == (LPARAM)itm_settings)
    {
        SetEvent(open_settings);
    } 

    if (uMsg == WM_COMMAND && lParam == (LPARAM)itm_ok)
    {
        ShowWindow(hwnd, SW_HIDE);
        config_save();
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cncnet_connect, 0, 0, NULL);
    }

    return FALSE;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char path[MAX_PATH];
    char *dir, *exe = NULL, *dll = "wsock32.dll";
    WIN32_FIND_DATA file;
    HANDLE find;
    BOOL ret;
    MSG msg;
    hwnd_status = CreateDialog(NULL, MAKEINTRESOURCE(DLG_STATUS), NULL, DialogProc);
    hwnd_settings = CreateDialog(NULL, MAKEINTRESOURCE(DLG_SETTINGS), NULL, DialogProc);
    itm_settings = GetDlgItem(hwnd_status, ITM_SETTINGS);
    itm_url = GetDlgItem(hwnd_settings, ITM_URL);
    itm_exe = GetDlgItem(hwnd_settings, ITM_EXE);
    itm_port = GetDlgItem(hwnd_settings, ITM_PORT);
    itm_ok = GetDlgItem(hwnd_settings, ITM_OK);
    itm_cancel = GetDlgItem(hwnd_settings, ITM_CANCEL);

    open_settings = CreateEvent(NULL, TRUE, FALSE, NULL);

    SendMessage(hwnd_status, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)LoadIcon(hInstance, "small"));
    SendMessage(hwnd_status, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(hInstance, "large"));
    SendMessage(hwnd_settings, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)LoadIcon(hInstance, "small"));
    SendMessage(hwnd_settings, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(hInstance, "large"));

    /* set initial focus */
    SendMessage(hwnd_status, WM_NEXTDLGCTL, (WPARAM)itm_settings, TRUE);
    SendMessage(hwnd_settings, WM_NEXTDLGCTL, (WPARAM)itm_ok, TRUE);

    /* fix working directory */
    GetModuleFileName(NULL, path, MAX_PATH);
    dir = GetDirectory(path);
    if (dir)
    {
        SetCurrentDirectoryA(dir);
    }

    /* make a good guess which game we are going to run */
    if (FileExists("C&C95.EXE"))
    {
        exe = "C&C95.EXE";
        game = "cnc95";
        dll = "thipx32.dll";
    }
    else if (FileExists("RA95.DAT"))
    {
        exe = "RA95.DAT";
        game = "ra95";
    }
    else if (FileExists("RA95.EXE"))
    {
        exe = "RA95.EXE";
        game = "ra95";
    }
    else if (FileExists("DTA.DAT"))
    {
        exe = "DTA.DAT";
        game = "ts"; /* give them their own protocol? */
    }
    else if (FileExists("SUN.EXE"))
    {
        exe = "GAME.EXE";
        game = "ts";
    }
    else if (FileExists("RA2.EXE"))
    {
        exe = "GAME.EXE";
        game = "ra2";
    }

    if (game == NULL)
    {
        MessageBox(NULL, "Couldn't find any compatible game in current directory. Sorry :-(", "CnCNet", MB_OK|MB_ICONERROR);
        return 1;
    }

    strcpy(cfg_exe, exe);

    /* populate exe selection */
    find = FindFirstFile("*.EXE", &file);
    if (find)
    {
        do {
            SendMessage(itm_exe, CB_ADDSTRING, 0, (WPARAM)file.cFileName);
        } while (FindNextFile(find, &file));
        FindClose(find);
    }

    snprintf(path, MAX_PATH-1, "%s%s", dir, exe);

    /* makes v3 full work! */
    protocol_register(game, path);

    if (FileExists("cncnet.dll"))
    {
        if (FileExists(dll))
        {
            DeleteFile(dll);
        }
        MoveFile("cncnet.dll", dll);
    }

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cncnet_connect, 0, 0, NULL);

    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0) 
    { 
        if (ret == -1)
        {
            break;
        }
        else if ((!IsWindow(hwnd_status) || !IsDialogMessage(hwnd_status, &msg)) && (!IsWindow(hwnd_settings) || !IsDialogMessage(hwnd_settings, &msg)))
        { 
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        } 
    } 

    return 0;
}
