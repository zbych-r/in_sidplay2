#pragma once


#include <shlobj.h>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <vector>

#include "residfp.h"
#include "sidplayfp/sidplayfp.h"
#include "sidplayfp/SidTuneInfo.h"
#include "sidplayfp/SidTune.h"
#include "utils/SidDatabase.h"

#include "StillBlock.h"

#include "in2.h"
#include "typesdefs.h"
//#include <hash_map>

#define WM_WA_MPEG_EOF WM_USER+2

#define PLAYBACK_BIT_PRECISION 16

using namespace std;

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class CThreadSidPlayer
{
private:
	//stdext::hash_map<const char*,char*> m;//,hash<const char*>,eqstr> m;
	map<const char*, char*, ltstr> m;//,hash<const char*>,eqstr> m;
	/**! Map - maps file path to vector of subsongs (usually 1) vector contains stuctures with STILL info:	
	TITLE:
	NAME:
	ARTIST:
	AUTHOR:
	COMMENT:
	*/
	map<const char*, vector<StillBlock*>, ltstr> m_stillMap2;//,hash<const char*>,eqstr> m;

    sidplayfp *m_engine;    
	SidTune m_tune;
	PlayerConfig m_playerConfig;
	HANDLE m_threadHandle;
	PlayerStatus_t m_playerStatus;
	unsigned __int64 m_decodedSampleCount;
	unsigned __int64 m_playTimems; //int 
	char* m_decodeBuf;
	int m_decodeBufLen;
	int m_currentTuneLength;
private:
	static DWORD __stdcall Run(void* thisparam);
	void AssignConfigValue(PlayerConfig *conf, string token, string value);
	SidDatabase m_sidDatabase;
	void ReadLine(char* buf,FILE *file,const int maxBuf);
protected:
	In_Module* m_inmod;
	int m_seekNeedMs;	
protected:
	void DoSeek();
	void FixPath(string& path);
	void FillSTILData();
	void FillSTILData2();
	void ClearSTILData(void);
public:	
	int maxLantency;
	CThreadSidPlayer(In_Module& inWAmod);
	~CThreadSidPlayer(void);
	void Init(void);
	void Play(void);
	void Pause(void);
	void Stop(void);
	void LoadTune(const char* name);
	PlayerStatus_t GetPlayerStatus() { return m_playerStatus;}
	int CurrentSubtune(void);
	void PlaySubtune(int subTune);
	const SidTuneInfo* GetTuneInfo(void);
	int GetPlayTime(void);
	bool LoadConfigFromFile(PlayerConfig *conf);
	bool LoadConfigFromFile(PlayerConfig *conf, wchar_t* fileName);
	void SaveConfigToFile(PlayerConfig *conf);
	void SaveConfigToFile(PlayerConfig *conf, wchar_t* fileName);
	const PlayerConfig& GetCurrentConfig();
	void SetConfig(PlayerConfig* newConfig);
	int GetSongLength(SidTune &tune);
	int GetSongLength();
	//! Moves emulation time pointer to given time
	void SeekTo(int timeMs);
	const char* GetSTILData(const char* filePath);
	const StillBlock* GetSTILData2(const char* filePath, int subsong);
};
