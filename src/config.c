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

char cfg_url[512] = "http://cncnet.cnc-comm.com/api/";
char cfg_exe[32] = { 0 };
int cfg_port = 8054;

extern char *game;

extern HWND itm_url;
extern HWND itm_exe;
extern HWND itm_port;

#define CONFIG ".\\cncnet.ini"

void config_load()
{
    char buf[512];

    GetPrivateProfileString("CnCNet", "Url", cfg_url, cfg_url, sizeof(cfg_url), CONFIG);
    GetPrivateProfileString("CnCNet", "Exe", cfg_exe, cfg_exe, sizeof(cfg_exe), CONFIG);
    cfg_port = GetPrivateProfileInt("CnCNet", "Port", cfg_port, CONFIG);

    SetWindowText(itm_url, cfg_url);
    SetWindowText(itm_exe, cfg_exe);
    sprintf(buf, "%d", cfg_port);
    SetWindowText(itm_port, buf); 
}

void config_save()
{
    char buf[512];

    GetWindowText(itm_url, cfg_url, sizeof(cfg_url));
    GetWindowText(itm_exe, cfg_exe, sizeof(cfg_exe));
    GetWindowText(itm_port, buf, sizeof(buf));

    WritePrivateProfileString("CnCNet", "Url", cfg_url, CONFIG);
    WritePrivateProfileString("CnCNet", "Exe", cfg_exe, CONFIG);
    WritePrivateProfileString("CnCNet", "Port", buf, CONFIG);

    cfg_port = atoi(buf);
}
