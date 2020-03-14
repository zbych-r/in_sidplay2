// in_sidplay2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "in2.h"
#include "ThreadSidplayer.h"
#include "resource.h"
#include "aboutdlg.h"
#include "configdlg.h"
#include "infodlg.h"
//#include "subsongdlg.h"
#include "wa_ipc.h"
#include "ipc_pe.h"
#include "helpers.h"

#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <string>
#include <vector>

In_Module inmod;
CThreadSidPlayer *sidPlayer = NULL;
HANDLE gUpdaterThreadHandle = 0;
HANDLE gMutex = 0;

DWORD AddSubsongsThreadProc(void* params);

/** Structure for passing parameters to worker thread for adding subsong
*/
typedef struct tAddSubsongParams
{
	int numSubsongs;
	int foundIndex;
	int startSong;
	char fileName[512];
};

void config(HWND hwndParent)
{
	DialogBoxParam(inmod.hDllInstance,MAKEINTRESOURCE(IDD_CONFIG_DLG),hwndParent,&ConfigDlgWndProc,NULL);
	SetFocus(hwndParent);
}

void about(HWND hwndParent)
{
	DialogBox(inmod.hDllInstance,MAKEINTRESOURCE(IDD_ABOUTDLG),hwndParent,&AboutDlgWndProc);
	SetFocus(hwndParent);
}

void OpenConsole()
{
	AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
}

void init() 
{ 
	//OpenConsole();

	/* any one-time initialization goes here (configuration reading, etc) */ 
	sidPlayer = new CThreadSidPlayer(inmod);
	sidPlayer->Init();

	gMutex = CreateMutex(NULL, FALSE, NULL);
}

void quit() { 
	/* one-time deinit, such as memory freeing */ 
	if (gMutex != 0)
	{
		CloseHandle(gMutex);
	}
	if (gUpdaterThreadHandle != 0)
	{
		CloseHandle(gUpdaterThreadHandle);
	}
	if(sidPlayer != NULL)
	{
		sidPlayer->Stop();
		delete sidPlayer;
	}
}

int isourfile(const char *fn) { 
// used for detecting URL streams.. unused here. 
// return !strncmp(fn,"http://",7); to detect HTTP streams, etc
	return 0; 
} 


// called when winamp wants to play a file
int play(const char *fn) 
{ 
	const SidTuneInfo* si;
	char buf[20];
	PlayerConfig cfg;
	std::string strFilename;
	std::string str;
	int i,j;
	int subsongIndex = 1;
	if(sidPlayer == NULL) init();

	strFilename.assign(fn);

	i = strFilename.find('}');
	if(i > 0) 
	{
		//assume char '{' will never occur in name unless its our subsong sign
		j = strFilename.find('{');
		str = strFilename.substr(j+1,i -j -1);
		subsongIndex = atoi(str.c_str());
		strFilename = strFilename.substr(i+1);
		sidPlayer->LoadTune(strFilename.c_str());
		sidPlayer->PlaySubtune(subsongIndex);

	}
	else 
	{
		sidPlayer->LoadTune(strFilename.c_str());
		const SidTuneInfo* tuneInfo = sidPlayer->GetTuneInfo();
		if (tuneInfo == NULL)
		{
			return -1;
		}
		sidPlayer->PlaySubtune(tuneInfo->startSong());
		// m_tune.getInfo()->startSong();
		//sidPlayer->PlaySubtune(subsongIndex);

	}



	return 0; 
}

// standard pause implementation
void pause() 
{
	sidPlayer->Pause();
}

void unpause() 
{
	sidPlayer->Play();
}

int ispaused() 
{ 
	return (sidPlayer->GetPlayerStatus() == SP_PAUSED)? 1 : 0;
}


// stop playing.
void stop() 
{ 
	sidPlayer->Stop();
}


// returns length of playing track
int getlength() 
{
	//return (sidPlayer->GetNumSubtunes()-1)*1000;
	return sidPlayer->GetSongLength()*1000;
}


// returns current output position, in ms.
// you could just use return mod.outMod->GetOutputTime(),
// but the dsp plug-ins that do tempo changing tend to make
// that wrong.
int getoutputtime() 
{ 
	//return sidPlayer->CurrentSubtune()*1000;
	//return inmod.outMod->GetOutputTime();
	return sidPlayer->GetPlayTime();
}


// called when the user releases the seek scroll bar.
// usually we use it to set seek_needed to the seek
// point (seek_needed is -1 when no seek is needed)
// and the decode thread checks seek_needed.
void setoutputtime(int time_in_ms) 
{ 
	//sidPlayer->PlaySubtune((time_in_ms / 1000)+1);
	sidPlayer->SeekTo(time_in_ms);
}


// standard volume/pan functions
void setvolume(int volume) { inmod.outMod->SetVolume(volume); }
void setpan(int pan) { inmod.outMod->SetPan(pan); }

// this gets called when the use hits Alt+3 to get the file info.
// if you need more info, ask me :)

int infoDlg(const char *fn, HWND hwnd)
{
	HWND wnd;
	const SidTuneInfo* info;
	SidTune tune(0);
	int i;
	std::string strfilename;

	strfilename.assign(fn);
	i = strfilename.find('}');
	if(i > 0) 
	{
		strfilename = strfilename.substr(i+1);
	}
	tune.load(strfilename.c_str());
	info = tune.getInfo();
	DialogBoxParam(inmod.hDllInstance,MAKEINTRESOURCE(IDD_FILEINFODLG),hwnd,InfoDlgWndProc,(LPARAM)info);
	//ShowWindow(wnd,SW_SHOW);
	return 0;
}

/**
	Replaces occurences of string %{...} containing tokens separated by | by checking which
	token is empty. Example: %{sr|a} it means if %sr is empty then use %a (if artist from stil is empty use
	artist from SID file
*/
void conditionsReplace(std::string& formatString, const StilBlock* stilBlock, const SidTuneInfo* tuneInfo)
{
	const int BUF_SIZE = 30;
	std::string conditionToken;
	int tokenBeginPos = 0;
	int tokenEndPos = 0;
	vector<string> tokens;
	char toReplaceToken[BUF_SIZE];

	while ((tokenBeginPos = formatString.find("%{", tokenBeginPos)) >= 0)
	{
		tokenEndPos = formatString.find('}', tokenBeginPos);
		if (tokenEndPos < 0)
		{
			break;
		}
		conditionToken = formatString.substr(tokenBeginPos + 2, tokenEndPos - tokenBeginPos - 2);
		sprintf(toReplaceToken, "%%{%s}", conditionToken.c_str());

		if (!conditionToken.empty())
		{
			tokens = split(conditionToken, '|');
			for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
			{
				
				if ((*it).compare("f") == 0)
				{
					replaceAll(formatString, toReplaceToken, "%f");
					break;
				}
				if (((*it).compare("t") == 0)&&(strlen(tuneInfo->infoString(0)) > 0))
				{
					replaceAll(formatString, toReplaceToken, "%t");
					break;
				}
				if (((*it).compare("a") == 0) && (strlen(tuneInfo->infoString(1)) > 0))
				{
					replaceAll(formatString, toReplaceToken, "%a");
					break;
				}
				if (((*it).compare("r") == 0) && (strlen(tuneInfo->infoString(2)) > 0))
				{
					replaceAll(formatString, toReplaceToken, "%r");
					break;
				}
				if ((*it).compare("x") == 0)
				{
					replaceAll(formatString, toReplaceToken, "%x");
					break;
				}

				if (((*it).compare("sr") == 0) && (stilBlock != NULL) && (!stilBlock->ARTIST.empty()))
				{
					replaceAll(formatString, toReplaceToken, "%sr");
					break;
				}
				if (((*it).compare("st") == 0) && (stilBlock != NULL) && (!stilBlock->TITLE.empty()))
				{
					replaceAll(formatString, toReplaceToken, "%st");
					break;
				}
				if (((*it).compare("sa") == 0) && (stilBlock != NULL) && (!stilBlock->AUTHOR.empty()))
				{
					replaceAll(formatString, toReplaceToken, "%sa");
					break;
				}
				if (((*it).compare("sn") == 0) && (stilBlock != NULL) && (!stilBlock->NAME.empty()))
				{
					replaceAll(formatString, toReplaceToken, "%sn");
					break;
				}
			}
			//check if condition was replaced by token, if not then make final token empty
			if (conditionToken.at(0) != '%')
			{
				conditionToken.clear();
			}
		}

		++tokenBeginPos;
	}
}


// this is an odd function. it is used to get the title and/or
// length of a track.
// if filename is either NULL or of length 0, it means you should
// return the info of lastfn. Otherwise, return the information
// for the file in filename.
// if title is NULL, no title is copied into it.
// if length_in_ms is NULL, no length is copied into it.
void getfileinfo(const char *filename, char *title, int *length_in_ms)
{
	const SidTuneInfo* info;
	std::string str;
	std::string strFilename;
	int length;
	SidTune tune(0);
	int i;
	int foundindex;
	int subsongIndex = 1;
	char buf[20];
	int plLength;
	char *plfilename;
	HWND h;
	char* foundChar;
	bool firstSong = true;

	if((filename == NULL) || (strlen(filename) == 0))
	{
		//get current song info
		info = sidPlayer->GetTuneInfo();
		if (info == NULL)
		{
			return;
		}
		length = sidPlayer->GetSongLength();
		subsongIndex = info->currentSong();//.currentSong;
		strFilename.assign(info->path());
		strFilename.append(info->dataFileName());
		subsongIndex = sidPlayer->CurrentSubtune();
	}
	else
	{
		subsongIndex = 1;
		strFilename.assign(filename);
		if(strFilename[0] == '{') 
		{		
			firstSong = false;
			//assume char '{' will never occur in name unless its our subsong sign
			i = strFilename.find('}');
			str = strFilename.substr(1,i -1);
			subsongIndex = atoi(str.c_str());
			strFilename = strFilename.substr(i+1);
			//get info from other file if we got real name
			tune.load(strFilename.c_str());
		}
		else
		{
			tune.load(strFilename.c_str());
			info = tune.getInfo();
			if (info == NULL)
			{
				return;
			}
			subsongIndex = tune.getInfo()->startSong();
		}
		info = tune.getInfo();
		//tune.selectSong(info.startSong);
		tune.selectSong(subsongIndex);
		length = sidPlayer->GetSongLength(tune);
	}
	
	//check if we got correct tune info
	//if (info.c64dataLen == 0) return;
	if (info->c64dataLen() == 0)
	{
		return;
	}
	if( length <0) length =0;
	length *= 1000;
	if(length_in_ms != NULL) *length_in_ms = length;
	
	/* build file title from template:
	%f - filename
	%t - song title from sid file
	%a - artist
	%r - release year and publisher
	%x - subsong string
	
	%n - subsong number in subsong string

	%sr - artist from STIL file
	%st - title from STIL file
	%sa - author from STIL file
	%sn - name from STIL file
	*/
	//info->dataFileName

	std::string fileNameOnly(strFilename);
	int cutStart = fileNameOnly.find_last_of("\\");
	int cutEnd = fileNameOnly.find_last_of(".");
	fileNameOnly = fileNameOnly.substr(cutStart + 1, cutEnd - cutStart-1);


	//std::string titleTemplate("%f / %a / %x %sn");
	std::string titleTemplate(sidPlayer->GetCurrentConfig().playlistFormat);
	std::string subsongTemplate(sidPlayer->GetCurrentConfig().subsongFormat);


	//fill STIL data if necessary
	const StilBlock* sb = NULL;
	if (sidPlayer->GetCurrentConfig().useSTILfile == true)
	{
		sb = sidPlayer->GetSTILData2(strFilename.c_str(), subsongIndex - 1);
	}
	else
	{
		sb = NULL;
	}
	conditionsReplace(titleTemplate, sb, info);

	replaceAll(titleTemplate, "%f", fileNameOnly.c_str());
	replaceAll(titleTemplate, "%t", info->infoString(0));
	replaceAll(titleTemplate, "%a", info->infoString(1));
	replaceAll(titleTemplate, "%r", info->infoString(2));
	if(info->songs() > 1) 
	{
		sprintf(buf,"%02d", subsongIndex);
		//itoa(subsongIndex, buf, 10);
		//replaceAll(subsongTemplate, "%n", _itoa(subsongIndex, buf, 10));
		replaceAll(subsongTemplate, "%n", buf);
		replaceAll(titleTemplate, "%x", subsongTemplate.c_str());
	}
	else
	{
		replaceAll(titleTemplate, "%x", "");
	}

	//fill STIL data if necessary
	if (sb == NULL)
	{
		replaceAll(titleTemplate, "%sr", "");
		replaceAll(titleTemplate, "%sa", "");
		replaceAll(titleTemplate, "%st", "");
		replaceAll(titleTemplate, "%sn", "");
	}
	else
	{			
		replaceAll(titleTemplate, "%sr", sb->ARTIST.c_str());
		replaceAll(titleTemplate, "%st", sb->TITLE.c_str());
		replaceAll(titleTemplate, "%sa", sb->AUTHOR.c_str());
		replaceAll(titleTemplate, "%sn", sb->NAME.c_str());
	}

	if(title != NULL) 
		strcpy(title, titleTemplate.c_str());

	if ((info->songs() == 1) || (firstSong == false))
	{
		return;
	}

	//we have subsongs...
	if (WaitForSingleObject(gMutex, 0))
	{
		return;
	}
	if (gUpdaterThreadHandle != 0)
	{
		CloseHandle(gUpdaterThreadHandle);
		gUpdaterThreadHandle = 0;
	}

	plLength = (int)SendMessage(inmod.hMainWindow,WM_WA_IPC,0,IPC_GETLISTLENGTH);
	//check if we have already added subsongs
	for(i=0; i<plLength;++i)
	{
		plfilename = (char*)SendMessage(inmod.hMainWindow,WM_WA_IPC,i,IPC_GETPLAYLISTFILE);
		if((plfilename == NULL)||(plfilename[0] != '{')) continue;		
		foundChar = strchr(plfilename,'}');
		if(strcmp(foundChar+1,strFilename.c_str()) == 0)
		{
			//subtunes were added no point to do it again
			ReleaseMutex(gMutex);
			return;
		}
	}	


	//first get entry index after which we will add subsong entry
	for(i=0; i<plLength;++i)
	{
		plfilename = (char*)SendMessage(inmod.hMainWindow,WM_WA_IPC,i,IPC_GETPLAYLISTFILE);
		if(plfilename == NULL) continue;
		if(strcmp(plfilename,strFilename.c_str()) == 0)
		{
			foundindex = i;
			break;
		}
	}	

	//run another thread for adding subsongs
	tAddSubsongParams *threadParams = new tAddSubsongParams();
	threadParams->foundIndex = foundindex;
	threadParams->numSubsongs = info->songs();
	threadParams->startSong = info->startSong();
	strcpy(threadParams->fileName, strFilename.c_str());
	gUpdaterThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AddSubsongsThreadProc, (void*)threadParams, 0, NULL);
	ReleaseMutex(gMutex);
	return;
}

DWORD AddSubsongsThreadProc(void* params)
{
	WaitForSingleObject(gMutex, INFINITE);

	fileinfo *fi = new fileinfo;
	COPYDATASTRUCT *cds = new COPYDATASTRUCT;
	std::string strFilename;
	char buf[20];
	tAddSubsongParams* threadParams = reinterpret_cast<tAddSubsongParams*>(params);
	int foundindex = threadParams->foundIndex;
	//get HWND of playlist window
	HWND h = (HWND)SendMessage(inmod.hMainWindow, WM_WA_IPC, IPC_GETWND_PE, IPC_GETWND);
	for (int i = 1; i <= threadParams->numSubsongs; ++i)
	{
		//first entry in playlist will be the startSong that is why we don't add it
		if (i == threadParams->startSong)
		{
			continue;
		}
		++foundindex;
		sprintf(buf, "{%d}", i);
		strFilename.assign(buf);
		strFilename.append(threadParams->fileName);
		ZeroMemory(fi, sizeof(fileinfo));
		strcpy(fi->file, strFilename.c_str());
		fi->index = foundindex;
		ZeroMemory(cds, sizeof(COPYDATASTRUCT));
		cds->dwData = IPC_PE_INSERTFILENAME;
		cds->lpData = fi;
		cds->cbData = sizeof(fileinfo);
		SendMessage(h, WM_COPYDATA, 0, (LPARAM)cds);
	}
	delete fi;
	delete cds;
	delete threadParams;

	ReleaseMutex(gMutex);
	return 0;
}

void eq_set(int on, char data[10], int preamp) 
{ 
	// most plug-ins can't even do an EQ anyhow.. I'm working on writing
	// a generic PCM EQ, but it looks like it'll be a little too CPU 
	// consuming to be useful :)
	// if you _CAN_ do EQ with your format, each data byte is 0-63 (+20db <-> -20db)
	// and preamp is the same. 
}


// module definition.
extern In_Module inmod = 
{
	IN_VER,	// defined in IN2.H
	"Winamp SIDPlayer (libsidplayfp) v2.2.0.0"
	// winamp runs on both alpha systems and x86 ones. :)
/*#ifdef __alpha
	"(AXP)"
#else
	"(x86)"
#endif*/
	,
	0,	// hMainWindow (filled in by winamp)
	0,  // hDllInstance (filled in by winamp)
	"SID\0Sid File (*.sid)\0"
	// this is a double-null limited list. "EXT\0Description\0EXT\0Description\0" etc.
	,
	1,	// is_seekable
	1,	// uses output plug-in system
	config,
	about,
	init,
	quit,
	getfileinfo,
	infoDlg,
	isourfile,
	play,
	pause,
	unpause,
	ispaused,
	stop,
	
	getlength,
	getoutputtime,
	setoutputtime,

	setvolume,
	setpan,

	0,0,0,0,0,0,0,0,0, // visualization calls filled in by winamp

	0,0, // dsp calls filled in by winamp

	eq_set,

	NULL,		// setinfo call filled in by winamp

	0 // out_mod filled in by winamp

};



extern "C"
__declspec(dllexport)
In_Module* winampGetInModule2()
{
   return &inmod;
}
