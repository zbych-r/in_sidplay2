#pragma once

#include "sidplayfp/SidTuneInfo.h"



int CALLBACK InfoDlgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InfoDlgInitDialog(HWND hWnd, SidTuneInfo *tuneInfo);