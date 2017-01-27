// PlayerTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "sidplayfp/sidplayfp.h"
#include "sidtune/SidTuneMod.h"
#include "residfp.h"

const char RESIDFP_ID[] = "ReSIDfp";

int main()
{
	sidplayfp engine;
	SidConfig cfg;

	cfg = engine.config();
	sidbuilder* sb = cfg.sidEmulation;
	if (sb != NULL)
	{
		delete sb;
	}
	cfg.sidEmulation = NULL;
	cfg.defaultSidModel = SidConfig::MOS6581;
	bool bok = engine.config(cfg);
	cfg.sidEmulation = new ReSIDfpBuilder(RESIDFP_ID);
	bok = engine.config(cfg);


	SidTuneMod* sidTune;
	sidTune = new SidTuneMod(NULL);
	sidTune->load("d:\\Proriad.sid");
	const SidTuneInfo* info = sidTune->getInfo();
	//printf("%s", info->getLoadAddress());
	//cfg.sidEmulation = new ReSIDfpBuilder();

	//sidbuilder* baseBuilder;
	
    return 0;
}

