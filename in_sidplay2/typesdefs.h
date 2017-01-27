#pragma once
#include "sidplayfp/SidConfig.h"

typedef enum PlayerStatus_t
{
	SP_RUNNING,
	SP_PAUSED,
	SP_STOPPED
} PlayerStatus_t;


typedef struct PlayerConfig
{
	SidConfig sidConfig;
	bool playLimitEnabled;
	int playLimitSec;
	char* songLengthsFile;
	char* hvscDirectory;
	bool useSongLengthFile;
	bool useSTILfile;
	/**
	Voice configuration whitch voice is enabled/disabled on which SID, first index is SID number second voice numerber
	*/
	bool voiceConfig[3][3];
	bool pseudoStereo;
	SidConfig::sid_model_t sid2Model;
	char* playlistFormat;
	char* subsongFormat;
} PlayerConfig;
