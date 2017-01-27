#include "stdafx.h"
#include "infodlg.h"
#include "resource.h"
#include "threadsidplayer.h"

extern CThreadSidPlayer *sidPlayer;

int CALLBACK InfoDlgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			if ((wmId==IDOK)||(wmId ==IDCANCEL))
				EndDialog(hWnd,wmId);
		}
		break;
	case WM_INITDIALOG:
		{
			InfoDlgInitDialog(hWnd,reinterpret_cast<SidTuneInfo*>(lParam));
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void InfoDlgInitDialog(HWND hWnd, SidTuneInfo *tuneInfo)
{
	char buf[20];
	std::string infoStr;
	int i;

	//SetWindowTextA(hWnd,tuneInfo->dataFileName);
	infoStr.append(tuneInfo->path());
	infoStr.append(tuneInfo->dataFileName());
	SetDlgItemTextA(hWnd,IDC_FILEPATHNAME,infoStr.c_str());

	SetDlgItemTextA(hWnd,IDC_STIL_ED,sidPlayer->GetSTILData(infoStr.c_str()));


	SetDlgItemTextA(hWnd,IDC_TITLE_STC,tuneInfo->infoString(0));
	SetDlgItemTextA(hWnd,IDC_AUTHOR_STC,tuneInfo->infoString(1));
	SetDlgItemTextA(hWnd,IDC_PUBLISHER_STC,tuneInfo->infoString(2));
	SetDlgItemTextA(hWnd,IDC_FORMATSTC,tuneInfo->formatString());
	sprintf(buf,"$%x",tuneInfo->loadAddr());
	SetDlgItemTextA(hWnd,IDC_LOADADDR_STC,buf);
	sprintf(buf,"$%x",tuneInfo->initAddr());
	SetDlgItemTextA(hWnd,IDC_INITADDR_STC,buf);
	sprintf(buf,"$%x",tuneInfo->playAddr());
	SetDlgItemTextA(hWnd,IDC_PLAYADDR_STC,buf);
	SetDlgItemTextA(hWnd,IDC_SUBSONGS_STC,itoa(tuneInfo->songs(),buf,10));

	sprintf(buf,"$%x",tuneInfo->sidChipBase(1));
	SetDlgItemTextA(hWnd,IDC_SID2_ADDR,buf);
	sprintf(buf,"$%x",tuneInfo->sidChipBase(2));
	SetDlgItemTextA(hWnd,IDC_SID3_ADDR,buf);

	switch(tuneInfo->sidModel(0))
	{
	case SidTuneInfo::SIDMODEL_6581: //SID2_MOS6581:
		SetDlgItemTextA(hWnd,IDC_SIDMODEL_STC,"MOS6581");
		break;
		case SidTuneInfo::SIDMODEL_8580:
		SetDlgItemTextA(hWnd,IDC_SIDMODEL_STC,"MOS8580");
		break;
	default:
		SetDlgItemTextA(hWnd,IDC_SIDMODEL_STC,"unknown");
		break;
	}
	//tuneInfo->clockSpeed
	switch (tuneInfo->clockSpeed())
	{
	case SidTuneInfo::CLOCK_PAL:
		SetDlgItemTextA(hWnd, IDC_CLOCKSPEED_STC, "PAL");
		break;
	case SidTuneInfo::CLOCK_NTSC:
		SetDlgItemTextA(hWnd, IDC_CLOCKSPEED_STC, "NTSC");
		break;
	default:
		SetDlgItemTextA(hWnd, IDC_CLOCKSPEED_STC, "ANY");
		break;
	}
	SetDlgItemTextA(hWnd,IDC_FILELENGTH_STC,itoa(tuneInfo->dataFileLen(),buf,10));

	infoStr.clear();
	for(i =0; i< tuneInfo->numberOfInfoStrings();++i)
	{
		if((tuneInfo->infoString(i) == NULL) || (strlen(tuneInfo->infoString(i)) == 0)) continue;
		infoStr.append(tuneInfo->infoString(i));
		infoStr.append("\r\n");
	}
	for(i =0; i< tuneInfo->numberOfCommentStrings();++i)
	{
		if(tuneInfo->commentString(i) == NULL) continue;
		infoStr.append(tuneInfo->commentString(i));
		infoStr.append("\r\n");
	}
	SetDlgItemTextA(hWnd,IDC_INOFBOX_STC,infoStr.c_str());


	//tuneInfo->speedString
	//tuneInfo->dataFileLen
	//tuneInfo->numberOfInfoStrings
}