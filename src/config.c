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
#include "../res/resource.h"

char cfg_connection[32] = "dedicated";
char cfg_host[512] = "open.cncnet.org";
int cfg_port = 9000;
char cfg_apiurl[512] = "http://cncnet.cnc-comm.com/api/";
int cfg_extport = 8054;
char cfg_opip[512] = "";
int cfg_opport = 8054;
char cfg_exe[64] = { 0 };
char cfg_args[512] = "-LAN";
int cfg_timeout = 3;
char cfg_autoupdate[32] = "true";
char cfg_extractdll[32] = "true";

extern char *game;

extern HWND itm_settings;
extern HWND itm_dedicated;
extern HWND itm_host;
extern HWND itm_port;
extern HWND itm_p2p;
extern HWND itm_url;
extern HWND itm_extport;
extern HWND itm_direct;
extern HWND itm_opip;
extern HWND itm_opport;
extern HWND itm_exe;
extern HWND itm_args;

#define SECTION "CnCNet3"
#define CONFIG ".\\cncnet.ini"

void config_load()
{
    char buf[512];

    GetPrivateProfileString(SECTION, "Connection", cfg_connection, cfg_connection, sizeof(cfg_connection), CONFIG);
    GetPrivateProfileString(SECTION, "Host", cfg_host, cfg_host, sizeof(cfg_host), CONFIG);
    cfg_port = GetPrivateProfileInt(SECTION, "Port", cfg_port, CONFIG);
    GetPrivateProfileString(SECTION, "ApiURL", cfg_apiurl, cfg_apiurl, sizeof(cfg_apiurl), CONFIG);
    cfg_extport = GetPrivateProfileInt(SECTION, "ExtPort", cfg_extport, CONFIG);
    GetPrivateProfileString(SECTION, "Executable", cfg_exe, cfg_exe, sizeof(cfg_exe), CONFIG);
    GetPrivateProfileString(SECTION, "OpponentHost", cfg_opip, cfg_opip, sizeof(cfg_opip), CONFIG);
    cfg_opport = GetPrivateProfileInt(SECTION, "OpponentPort", cfg_opport, CONFIG);
    GetPrivateProfileString(SECTION, "Arguments", cfg_args, cfg_args, sizeof(cfg_args), CONFIG);
    cfg_timeout = GetPrivateProfileInt(SECTION, "Timeout", cfg_timeout, CONFIG);
    GetPrivateProfileString(SECTION, "AutoUpdate", cfg_autoupdate, cfg_autoupdate, sizeof(cfg_autoupdate), CONFIG);
    GetPrivateProfileString(SECTION, "ExtractDll", cfg_extractdll, cfg_extractdll, sizeof(cfg_extractdll), CONFIG);

    if (cfg_timeout < 0 || cfg_timeout > 30)
    {
        cfg_timeout = 0;
    }

    if (stricmp(cfg_connection, "direct") == 0)
    {
        PostMessage(itm_direct, BM_SETCHECK, BST_CHECKED, 0);
    }
    else if (stricmp(cfg_connection, "p2p") == 0)
    {
        PostMessage(itm_p2p, BM_SETCHECK, BST_CHECKED, 0);
    }
    else
    {
        PostMessage(itm_dedicated, BM_SETCHECK, BST_CHECKED, 0);
    }

    SetWindowText(itm_host, cfg_host);
    sprintf(buf, "%d", cfg_port);
    SetWindowText(itm_port, buf); 
    SetWindowText(itm_url, cfg_apiurl);
    SetWindowText(itm_opip, cfg_opip);
    sprintf(buf, "%d", cfg_opport);
    SetWindowText(itm_opport, buf); 
    SetWindowText(itm_exe, cfg_exe);
    sprintf(buf, "%d", cfg_extport);
    SetWindowText(itm_extport, buf); 
    SetWindowText(itm_args, cfg_args); 
}

void config_save()
{
    char buf[512];

    if (SendMessage(itm_direct, BM_GETCHECK, 0, 0) == BST_CHECKED)
    {
        strcpy(cfg_connection, "direct");
    } 
    else if (SendMessage(itm_p2p, BM_GETCHECK, 0, 0) == BST_CHECKED)
    {
        strcpy(cfg_connection, "p2p");
    }
    else
    {
        strcpy(cfg_connection, "dedicated");
    }

    WritePrivateProfileString(SECTION, "Connection", cfg_connection, CONFIG);

    GetWindowText(itm_host, cfg_host, sizeof(cfg_host));
    WritePrivateProfileString(SECTION, "Host", cfg_host, CONFIG);

    GetWindowText(itm_port, buf, sizeof(buf));
    WritePrivateProfileString(SECTION, "Port", buf, CONFIG);
    cfg_port = atoi(buf);

    GetWindowText(itm_url, cfg_apiurl, sizeof(cfg_apiurl));
    WritePrivateProfileString(SECTION, "ApiUrl", cfg_apiurl, CONFIG);

    GetWindowText(itm_extport, buf, sizeof(buf));
    WritePrivateProfileString(SECTION, "ExtPort", buf, CONFIG);
    cfg_extport = atoi(buf);

    GetWindowText(itm_opip, cfg_opip, sizeof(cfg_opip));
    WritePrivateProfileString(SECTION, "OpponentHost", cfg_opip, CONFIG);

    GetWindowText(itm_opport, buf, sizeof(buf));
    WritePrivateProfileString(SECTION, "OpponentPort", buf, CONFIG);
    cfg_opport = atoi(buf);

    GetWindowText(itm_exe, cfg_exe, sizeof(cfg_exe));
    WritePrivateProfileString(SECTION, "Executable", cfg_exe, CONFIG);

    GetWindowText(itm_args, cfg_args, sizeof(cfg_args));
    WritePrivateProfileString(SECTION, "Arguments", cfg_args, CONFIG);

    sprintf(buf, "%d", cfg_timeout);
    WritePrivateProfileString(SECTION, "Timeout", buf, CONFIG);

    WritePrivateProfileString(SECTION, "AutoUpdate", cfg_autoupdate, CONFIG);
    WritePrivateProfileString(SECTION, "ExtractDll", cfg_extractdll, CONFIG);
}
