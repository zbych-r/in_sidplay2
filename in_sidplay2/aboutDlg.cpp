#include "StdAfx.h"
#include "aboutDlg.h"
#include "resource.h"

void ShowAboutDlg()
{
}

int CALLBACK AboutDlgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			if (wmId==IDOK)
				EndDialog(hWnd,wmId);
			else
				return FALSE;
		}
		break;
	case WM_INITDIALOG:
		{
			//char* sidplayInfo = "libsidplay2 info:\r\nSimon White\tlibsidplay2 engine\r\nDag Lem\t\treSID library\r\n";

			SetDlgItemTextA(hWnd,IDC_ABOUTED,"Thanks to:\r\nSimon White\tlibsidplay2 engine\r\nDag Lem\t\treSID library\r\n"
				"Antti Lankila\treSID-fp\r\nLeandro Nini\tlibsidplayfp");
		}
		break;
	default:
		return FALSE;
	}

   return TRUE;
}
