#include "StdAfx.h"
#include "configDlg.h"
#include "resource.h"
#include "typesdefs.h"
#include <shlobj.h>


PlayerConfig *playerConfig;


void ConfigDlgInitDialog(HWND hWnd)
{
	int val, val1;
	char buf[20];

	playerConfig = new PlayerConfig;
	//PlayerConfig cfg;
	//initialize config with current values
	*playerConfig = sidPlayer->GetCurrentConfig();
	//we dont need current value coz we will load it from file
	playerConfig->songLengthsFile = NULL;
	//now load "real" settings from file (they may differ from those above)
	sidPlayer->LoadConfigFromFile(playerConfig);


	//FREQ
	SendDlgItemMessage(hWnd, IDC_FREQUENCY, CB_ADDSTRING, 0, (LPARAM)L"48000");
	SendDlgItemMessage(hWnd,IDC_FREQUENCY,CB_ADDSTRING,0,(LPARAM)L"44100");
	SendDlgItemMessage(hWnd,IDC_FREQUENCY,CB_ADDSTRING,0,(LPARAM)L"22050");
	SendDlgItemMessage(hWnd,IDC_FREQUENCY,CB_ADDSTRING,0,(LPARAM)L"11025");
	//channels
	SendDlgItemMessage(hWnd,IDC_CHANNELS,CB_ADDSTRING,0,(LPARAM)L"Mono");
	SendDlgItemMessage(hWnd,IDC_CHANNELS,CB_ADDSTRING,0,(LPARAM)L"Stereo");
	//C64 model
	SendDlgItemMessage(hWnd, IDC_C64MODEL, CB_ADDSTRING, 0, (LPARAM)L"PAL");
	SendDlgItemMessage(hWnd, IDC_C64MODEL, CB_ADDSTRING, 0, (LPARAM)L"NTSC");
	SendDlgItemMessage(hWnd, IDC_C64MODEL, CB_ADDSTRING, 0, (LPARAM)L"Old NTSC");
	SendDlgItemMessage(hWnd, IDC_C64MODEL, CB_ADDSTRING, 0, (LPARAM)L"DREAN");

	//sid model
	SendDlgItemMessage(hWnd,IDC_SIDMODEL,CB_ADDSTRING,0,(LPARAM)L"MOS-6581");
	SendDlgItemMessage(hWnd,IDC_SIDMODEL,CB_ADDSTRING,0,(LPARAM)L"MOS-8580");

	//sid2 model
	SendDlgItemMessage(hWnd, IDC_SID2MODEL, CB_ADDSTRING, 0, (LPARAM)L"MOS-6581");
	SendDlgItemMessage(hWnd, IDC_SID2MODEL, CB_ADDSTRING, 0, (LPARAM)L"MOS-8580");

	//set values:
	//freq
	switch (playerConfig->sidConfig.frequency)
	{
	case 48000:
		val = 0;
		break;
	case 44100:
		val = 1;
		break;
	case 22050:
		val = 2;
		break;
	case 11025:
		val = 3;
		break;
	}
	SendDlgItemMessage(hWnd,IDC_FREQUENCY,CB_SETCURSEL,(WPARAM)val,0);

	//channel
	val = (playerConfig->sidConfig.playback == SidConfig::MONO)? val=0 : val=1;
	SendDlgItemMessage(hWnd,IDC_CHANNELS,CB_SETCURSEL,(WPARAM)val,0);
	//C64 model
	SendDlgItemMessage(hWnd,IDC_C64MODEL,CB_SETCURSEL,(WPARAM)playerConfig->sidConfig.defaultC64Model,0);
	//force c64 model
	if (playerConfig->sidConfig.forceC64Model) CheckDlgButton(hWnd, IDC_FORCE_C64MODEL, BST_CHECKED);
	else CheckDlgButton(hWnd, IDC_FORCE_C64MODEL, BST_UNCHECKED);

	//sid model
	SendDlgItemMessage(hWnd, IDC_SIDMODEL, CB_SETCURSEL, (WPARAM)playerConfig->sidConfig.defaultSidModel, 0);
	//force SID model
	if (playerConfig->sidConfig.forceSidModel) CheckDlgButton(hWnd, IDC_FORCE_SID_MODEL, BST_CHECKED);
	else CheckDlgButton(hWnd, IDC_FORCE_SID_MODEL, BST_UNCHECKED);

	if (playerConfig->sidConfig.forceSecondSidModel) CheckDlgButton(hWnd, IDC_FORCE_SID2_MODEL, BST_CHECKED);
	else CheckDlgButton(hWnd, IDC_FORCE_SID2_MODEL, BST_UNCHECKED);

	if(playerConfig->playLimitEnabled) CheckDlgButton(hWnd,IDC_PLAYLIMIT_CHK,BST_CHECKED);
	else CheckDlgButton(hWnd,IDC_PLAYLIMIT_CHK,BST_UNCHECKED);
	SetDlgItemTextA(hWnd,IDC_PLAYLIMITTIME,itoa(playerConfig->playLimitSec,buf,10));

	if(playerConfig->useSongLengthFile)
		CheckDlgButton(hWnd,IDC_ENABLESONGLENDB,BST_CHECKED);
	else 
		CheckDlgButton(hWnd,IDC_ENABLESONGLENDB,BST_UNCHECKED);
	if (playerConfig->songLengthsFile != NULL)
	{
		SetDlgItemTextA(hWnd, IDC_SONGLENGTHFILE, playerConfig->songLengthsFile);
	}
	CheckDlgButton(hWnd,IDC_ENABLESTIL,((playerConfig->useSTILfile)? BST_CHECKED : BST_UNCHECKED));
	SetDlgItemTextA(hWnd,IDC_HVSCDIR,playerConfig->hvscDirectory);

	//pseudo stereo
	CheckDlgButton(hWnd, IDC_PSEUDOSTEREO, ((playerConfig->pseudoStereo) ? BST_CHECKED : BST_UNCHECKED));
	//second sid model (pseudo stereo)
	SendDlgItemMessage(hWnd, IDC_SID2MODEL, CB_SETCURSEL, (WPARAM)playerConfig->sid2Model, 0);

	if (playerConfig->playlistFormat != NULL)
	{
		SetDlgItemTextA(hWnd, IDC_PLAYLIST_FORMAT, playerConfig->playlistFormat);
	}
	else
	{
		SetDlgItemTextA(hWnd, IDC_PLAYLIST_FORMAT, "%t %x / %a / %r / %sn");
	}

	if (playerConfig->subsongFormat != NULL)
	{
		SetDlgItemTextA(hWnd, IDC_SUBSONG_FORMAT, playerConfig->subsongFormat);
	}
	else
	{
		SetDlgItemTextA(hWnd, IDC_SUBSONG_FORMAT, "(Tune %n)");
	}

	/*
	CheckDlgButton(hWnd, IDC_VOICE00, playerConfig->voiceConfig[0][0] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE01, playerConfig->voiceConfig[0][1] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE02, playerConfig->voiceConfig[0][2] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE10, playerConfig->voiceConfig[1][0] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE11, playerConfig->voiceConfig[1][1] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE12, playerConfig->voiceConfig[1][2] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE20, playerConfig->voiceConfig[2][0] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE21, playerConfig->voiceConfig[2][1] ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE22, playerConfig->voiceConfig[2][2] ? BST_UNCHECKED : BST_CHECKED);
	*/
	CheckDlgButton(hWnd, IDC_VOICE00, (playerConfig->voiceConfig[0][0] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE01, (playerConfig->voiceConfig[0][1] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE02, (playerConfig->voiceConfig[0][2] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE10, (playerConfig->voiceConfig[1][0] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE11, (playerConfig->voiceConfig[1][1] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE12, (playerConfig->voiceConfig[1][2] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE20, (playerConfig->voiceConfig[2][0] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE21, (playerConfig->voiceConfig[2][1] == false) ? BST_UNCHECKED : BST_CHECKED);
	CheckDlgButton(hWnd, IDC_VOICE22, (playerConfig->voiceConfig[2][2] == false) ? BST_UNCHECKED : BST_CHECKED);

}

void UpdateConfig(HWND hWnd)
{
	const int MAX_BUFFER_SIZE = 100;
	int val,val1;
	char buf[MAX_BUFFER_SIZE];

	val = SendDlgItemMessage(hWnd,IDC_FREQUENCY,CB_GETCURSEL,0,0);
	switch(val)
	{
	case 0:
		playerConfig->sidConfig.frequency = 48000;
		break;
	case 1:
		playerConfig->sidConfig.frequency = 44100;
		break;
	case 2:
		playerConfig->sidConfig.frequency = 22050;
		break;
	case 3:
		playerConfig->sidConfig.frequency = 11025;
		break;
	}

	//playbacj chanells
	val = SendDlgItemMessage(hWnd,IDC_CHANNELS,CB_GETCURSEL,0,0);
	playerConfig->sidConfig.playback = (val==0)? SidConfig::MONO : SidConfig::STEREO;

	//C64 model
	val = SendDlgItemMessage(hWnd, IDC_C64MODEL, CB_GETCURSEL, 0, 0);
	playerConfig->sidConfig.defaultC64Model = (SidConfig::c64_model_t)val;
	if (IsDlgButtonChecked(hWnd, IDC_FORCE_C64MODEL) == BST_CHECKED)
	{
		playerConfig->sidConfig.forceC64Model = true;
	}
	else
	{
		playerConfig->sidConfig.forceC64Model = false;
	}

	//SID model
	val = SendDlgItemMessage(hWnd,IDC_SIDMODEL,CB_GETCURSEL,0,0);
	playerConfig->sidConfig.defaultSidModel = (SidConfig::sid_model_t) val;
	if (IsDlgButtonChecked(hWnd, IDC_FORCE_SID_MODEL) == BST_CHECKED)
	{
		playerConfig->sidConfig.forceSidModel = true;
	}
	else
	{
		playerConfig->sidConfig.forceSidModel = false;
	}

	playerConfig->sidConfig.forceSecondSidModel = (IsDlgButtonChecked(hWnd, IDC_FORCE_SID2_MODEL) == BST_CHECKED) ? true : false;
	
	if(IsDlgButtonChecked(hWnd,IDC_PLAYLIMIT_CHK) == BST_CHECKED)
	{
		GetDlgItemTextA(hWnd,IDC_PLAYLIMITTIME,buf,20);
		playerConfig->playLimitEnabled = true;
		playerConfig->playLimitSec = atoi(buf);
	}
	else
	{
		playerConfig->playLimitEnabled = false;
	}
	
	if(IsDlgButtonChecked(hWnd,IDC_ENABLESONGLENDB) == BST_CHECKED)
		playerConfig->useSongLengthFile = true;
	else
		playerConfig->useSongLengthFile = false;

	playerConfig->useSTILfile = (IsDlgButtonChecked(hWnd,IDC_ENABLESTIL) == BST_CHECKED)? true : false;

	//voice configuration
	playerConfig->voiceConfig[0][0] = (IsDlgButtonChecked(hWnd, IDC_VOICE00) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[0][1] = (IsDlgButtonChecked(hWnd, IDC_VOICE01) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[0][2] = (IsDlgButtonChecked(hWnd, IDC_VOICE02) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[1][0] = (IsDlgButtonChecked(hWnd, IDC_VOICE10) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[1][1] = (IsDlgButtonChecked(hWnd, IDC_VOICE11) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[1][2] = (IsDlgButtonChecked(hWnd, IDC_VOICE12) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[2][0] = (IsDlgButtonChecked(hWnd, IDC_VOICE20) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[2][1] = (IsDlgButtonChecked(hWnd, IDC_VOICE21) == BST_CHECKED) ? true : false;
	playerConfig->voiceConfig[2][2] = (IsDlgButtonChecked(hWnd, IDC_VOICE22) == BST_CHECKED) ? true : false;

	//pseudostereo
	playerConfig->pseudoStereo = (IsDlgButtonChecked(hWnd, IDC_PSEUDOSTEREO) == BST_CHECKED) ? true : false;

	//SID model
	val = SendDlgItemMessage(hWnd, IDC_SID2MODEL, CB_GETCURSEL, 0, 0);
	playerConfig->sid2Model = (SidConfig::sid_model_t) val;

	//playlist format
	GetDlgItemTextA(hWnd, IDC_PLAYLIST_FORMAT, buf, MAX_BUFFER_SIZE);
	if (strlen(buf) == 0)
	{
		playerConfig->playlistFormat = "";
	}
	else
	{
		playerConfig->playlistFormat = new char[strlen(buf) + 1];
		strcpy(playerConfig->playlistFormat, buf);
	}
	//subsong format
	GetDlgItemTextA(hWnd, IDC_SUBSONG_FORMAT, buf, MAX_BUFFER_SIZE);
	if (strlen(buf) == 0)
	{
		playerConfig->subsongFormat = "";
	}
	else
	{
		playerConfig->subsongFormat = new char[strlen(buf) + 1];
		strcpy(playerConfig->subsongFormat, buf);
	}
}

void SelectHvscFile(HWND hWnd)
{
	wchar_t path[MAX_PATH];
	size_t pathLen;


	if (GetFileNameFromBrowse(hWnd, path, MAX_PATH, L"c:\\", L"txt", L"MD5 song lenght file (*.md5)\0*.md5\0Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0\0",
		L"Select song length db file") != TRUE) return;
	if(playerConfig->songLengthsFile != NULL) delete[] playerConfig->songLengthsFile;
	pathLen = wcslen(path)+1;
	playerConfig->songLengthsFile = new char[pathLen];
	wcstombs(playerConfig->songLengthsFile,path,pathLen);
	SetDlgItemTextA(hWnd,IDC_SONGLENGTHFILE,playerConfig->songLengthsFile);

}

void SelectHvscDirectory(HWND hWnd)
{
	wchar_t path[MAX_PATH];
	size_t pathLen;
	LPITEMIDLIST idlRoot = NULL;

    BROWSEINFO bi = { 0 };

    bi.lpszTitle = L"Select HVSC directory";
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
        // get the name of the folder and put it in path
        SHGetPathFromIDList ( pidl, path );
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free(pidl);
            imalloc->Release ( );
        }
    
		if(playerConfig->hvscDirectory != NULL) delete[] playerConfig->hvscDirectory;
		pathLen = wcslen(path)+1;
		playerConfig->hvscDirectory = new char[pathLen];
		wcstombs(playerConfig->hvscDirectory,path,pathLen);
		SetDlgItemTextA(hWnd,IDC_HVSCDIR,playerConfig->hvscDirectory);
	}
}


int CALLBACK ConfigDlgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch(wmId)
			{
			case IDOK:
				UpdateConfig(hWnd);
				sidPlayer->SaveConfigToFile(playerConfig);
				sidPlayer->SetConfig(playerConfig);
				EndDialog(hWnd,wmId);
				break;
			case IDC_BROWSE_BTN:
				SelectHvscFile(hWnd);
				break;
			case IDC_BROWSE_HVSC:
				SelectHvscDirectory(hWnd);
				break;
			case IDCANCEL:
				EndDialog(hWnd,wmId);
				break;
			}
			if (wmId==IDCANCEL)
				EndDialog(hWnd,wmId);
			//else
			//	return FALSE;
		}
		break;

	case WM_DESTROY:
		delete playerConfig;
		break;
	case WM_INITDIALOG:
		{
			playerConfig = reinterpret_cast<PlayerConfig*>(lParam);
			ConfigDlgInitDialog(hWnd);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
