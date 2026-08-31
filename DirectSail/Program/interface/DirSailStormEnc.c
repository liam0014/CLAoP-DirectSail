// Dedicated Direct Sail storm-diverter interface.
//
// Ordinary and quest Watchman encounters remain on DirSailEnc.c. This file
// exists only so the storm detector can keep the known-working two-button
// layout without fighting the single-button Watchman layout.

#define DSENC_SECONDS_TIMEOUT 8

string totalInfo = "";
int nTimeout = 0;
int DSENC_TIMEOUT = 90;
bool bStormPromptResolved = false;
bool bStormPromptTornado = false;
bool bStormTextReady = false;

void InitInterface(string iniName)
{
	EngineLayersOffOn(true);
	SetTimeScale(0.0);
	PlaySound("_EvShip");

	bStormPromptResolved = false;
	bStormPromptTornado = false;
	bStormTextReady = false;
	nTimeout = 0;
	bQuestCheckProcessFreeze = true;

	// LaunchDirSailEnc() stores the tornado state before procInterfacePrepare().
	// Consume that transport marker immediately so no later Watchman interface
	// can inherit storm state.
	if(CheckAttribute(pchar, "DirectSail.StormPromptTornado"))
	{
		bStormPromptTornado = (sti(pchar.DirectSail.StormPromptTornado) != 0);
	}
	DeleteAttribute(pchar, "DirectSail.StormPrompt");
	DeleteAttribute(pchar, "DirectSail.StormPromptTornado");

	GameInterface.title = "title_map";
	iniName = "RESOURCE\\INI\\INTERFACES\\DirSailStormEnc.ini";
	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	SetFormatedText("MAP_CAPTION", XI_ConvertString("title_map"));
	SetNodeUsing("B_OK", true);
	SetSelectable("B_OK", true);
	SetNodeUsing("B_CANCEL", true);
	SetSelectable("B_CANCEL", true);
	SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE,
				"B_OK", 0, "#" + XI_ConvertString("StopStorm"));
	SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE,
				"B_CANCEL", 0, "#" + XI_ConvertString("StartStorm"));
	SetCurrentNode("B_OK");

	SetEventHandler("InterfaceBreak", "ProcessBreakExit", 0);
	SetEventHandler("exitCancel", "ProcessCancelExit", 0);
	SetEventHandler("ievnt_command", "ProcCommand", 0);
	SetEventHandler("evntDoPostExit", "DoPostExit", 0);
	SetEventHandler("frame", "IProcessFrame", 0);

	EI_CreateFrame("INFO_BORDERS", 250, 152, 550, 342);
	DSENC_TIMEOUT = 60 * DSENC_SECONDS_TIMEOUT;

	trace("DS STORM DIVERTER UI READY: tornado=" + bStormPromptTornado);
}

// Compatibility with the existing islands_loader.c call which follows
// LaunchDirSailEnc(). The dialogue no longer depends on this callback, but if
// execution does return to the caller it can still refresh the tornado flag.
void evtDirSailStorm(bool bIsTornado)
{
	trace("DS STORM DIVERTER CALLBACK ENTER: tornado=" + bIsTornado);
	bStormPromptTornado = bIsTornado;
	nTimeout = 0;
}

void DirectSail_StormPromptAvoid()
{
	if(bStormPromptResolved) return;

	trace("DS STORM DIVERTER: avoid");
	bStormPromptResolved = true;
	IDoExit(RC_INTERFACE_MAP_EXIT);
}

void DirectSail_StormPromptAgainst()
{
	if(bStormPromptResolved) return;

	bStorm = true;
	bTornado = bStormPromptTornado;
	WeatherParams.Tornado = bStormPromptTornado;
	WeatherParams.Storm = true;
	iStormLockSeconds = 60 + rand(120);
	Seafoam.storm = "true";
	Whr_UpdateWeather();
	SetSchemeForSea();

	trace("DS STORM DIVERTER: against tornado=" + bStormPromptTornado);
	bStormPromptResolved = true;
	IDoExit(RC_INTERFACE_MAP_EXIT);
}

void IProcessFrame()
{
	nTimeout++;

	if(nTimeout > DSENC_TIMEOUT)
	{
		DirectSail_StormPromptAgainst();
		return;
	}

	if(!bStormTextReady && nTimeout > 1)
	{
		bStormTextReady = true;
		totalInfo = XI_ConvertString("StormRightAhead");
		SetFormatedText("INFO_TEXT", totalInfo);
		SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "INFO_TEXT", 5);
		SetFormatedText("INFO_TEXT_QUESTION", "");
		SetCurrentNode("B_OK");
	}
}

void ProcessBreakExit()
{
	if(!bStormPromptResolved)
	{
		DirectSail_StormPromptAgainst();
		return;
	}
	IDoExit(RC_INTERFACE_MAP_EXIT);
}

void ProcessCancelExit()
{
	if(!bStormPromptResolved)
	{
		DirectSail_StormPromptAgainst();
		return;
	}
	IDoExit(RC_INTERFACE_MAP_EXIT);
}

void IDoExit(int exitCode)
{
	ReleaseMapEncounters();
	DelEventHandler("InterfaceBreak", "ProcessBreakExit");
	DelEventHandler("exitCancel", "ProcessCancelExit");
	DelEventHandler("ievnt_command", "ProcCommand");
	DelEventHandler("evntDoPostExit", "DoPostExit");
	DelEventHandler("frame", "IProcessFrame");

	SetTimeScale(1.0);
	TimeScaleCounter = 0;
	if(IsPerkIntoList("TimeSpeed"))
	{
		DelPerkFromActiveList("TimeSpeed");
	}

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
	PostEvent("StopQuestCheckProcessFreeze", 100);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	if(comName != "activate" && comName != "click") return;

	if(nodName == "B_OK")
	{
		DirectSail_StormPromptAvoid();
		return;
	}

	if(nodName == "B_CANCEL")
	{
		DirectSail_StormPromptAgainst();
		return;
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}
