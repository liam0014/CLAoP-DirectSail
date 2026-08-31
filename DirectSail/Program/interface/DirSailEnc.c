// World-map encounter support uses code from the Direct Sail encounter implementation in Gentlemen of Fortune: Historical Eras II, modified for compatibility with Caribbean Legend: Age of Pirates.
//
// Gentlemen of Fortune: Historical Eras II lists its assets under the Creative Commons Attribution-NonCommercial 4.0 International licence.
//https://chezjfrey.itch.io/gentlemen-of-fortune-historical-eras-module-2

#define DIR_SAIL_WAR_DIST 4000000.0 // 2000 ^ 2
#define DSENC_SECONDS_TIMEOUT 8
//#20220907-01
#define DIR_SEA_GROUP_MARK "ds_"

string totalInfo = "";
string  sQuestSeaCharId = "";
int nTimeout = 0;
int DSENC_TIMEOUT = 90;
aref rRawGroup, encDataForSlot;
int iNumMerchantShips, iNumWarShips;
float RTplayerShipX, RTplayerShipZ, x, ay, z, encounterbearing, dstScale, origX, origZ;
string encID, encStringID, origEncID, altEncID;
bool bShipsSpawned = false;
bool bShipsDesc = false;
bool bSpecialEncounter = false;
bool bStormPrompt = false;
bool bStormPromptResolved = false;
bool bStormPromptTornado = false;
bool bQuestEnterHookApplied = false;
int isShipEncounterType = 0;
int iEncounterType, iRealEncounterType, nDesc;
ref rEncounter;
int origEID, altEID; //For assigned slot
string dirOff, findType;
object grpTrans[2];
int grpTNnum = 0;

bool DirectSail_FindPendingStormPrompt()
{
    int i;
    int nWME;
    aref WME;
    aref rStorm;

    // LaunchDirSailEnc() transfers control into the interface before the
    // caller can reliably invoke evtDirSailStorm().  The storm encounter is
    // already marked for consumption by checkWMEnctr(), so use that persistent
    // world-map state as the pre-launch handoff marker.
    if(!CheckCharacterItem(pchar, "stop_storm")) return false;

    makearef(WME, worldMap.encounters);
    nWME = GetAttributesNum(WME);

    for(i = 0; i < nWME; i++)
    {
        rStorm = GetAttributeN(WME, i);

        if(!CheckAttribute(rStorm, "type")) continue;
        if(rStorm.type != "Storm") continue;
        if(!CheckAttribute(rStorm, "needDelete")) continue;
        if(rStorm.needDelete != "Reload delete storm encounter") continue;

        bStormPrompt = true;
        bStormPromptResolved = false;
        bStormPromptTornado = false;

        if(CheckAttribute(rStorm, "isTornado") && sti(rStorm.isTornado) != 0)
        {
            bStormPromptTornado = true;
        }

        trace("DS STORM DIVERTER PREINIT: tornado=" + bStormPromptTornado);
        return true;
    }

    return false;
}

void InitInterface(string iniName)
{
	EngineLayersOffOn(true);
	SetTimeScale(0.0);
	PlaySound("_EvShip");
    bShipsSpawned = false;
    bShipsDesc = false;
    bSpecialEncounter = false;
    bStormPrompt = false;
    bStormPromptResolved = false;
    bStormPromptTornado = false;
    bQuestEnterHookApplied = false;
    nTimeout = 0;

    isShipEncounterType = 0;
    altEID = -1;
    altEncID = "";
	bQuestCheckProcessFreeze = true;

    GameInterface.title = "title_map";

    // Direct Sail uses its own single-button Watchman layout.
    iniName = "RESOURCE\INI\INTERFACES\DirSailEnc.ini";
    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

    SetFormatedText("MAP_CAPTION", XI_ConvertString("title_map"));

    if(DirectSail_FindPendingStormPrompt())
    {
        // Storm diverter: restore the two-button layout before the first frame.
        SetNodePosition("B_OK", 270, 432, 395, 464);
        SetNodePosition("B_CANCEL", 405, 432, 530, 464);
        SetNodeUsing("B_OK", true);
        SetSelectable("B_OK", true);
        SetNodeUsing("B_CANCEL", true);
        SetSelectable("B_CANCEL", true);
        SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, "B_OK", 0, "#" + XI_ConvertString("StopStorm"));
        SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, "B_CANCEL", 0, "#" + XI_ConvertString("StartStorm"));
        SetCurrentNode("B_OK");

        trace("DS STORM DIVERTER UI READY: tornado=" + bStormPromptTornado);
    }
    else
    {
        SetNodeUsing("B_OK", true);
        SetSelectable("B_OK", true);
        SetNodeUsing("B_CANCEL", false);
        SetSelectable("B_CANCEL", false);
    }


	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);
	SetEventHandler("frame","IProcessFrame",0);

	EI_CreateFrame("INFO_BORDERS", 250,152,550,342);

	DSENC_TIMEOUT = 60 * DSENC_SECONDS_TIMEOUT;
	grpTNnum = 0;
}

// Compatibility fallback for callers which can hand storm state into an
// already-open Direct Sail interface.  Normal storm prompts are now detected
// during InitInterface() from the persistent storm marker instead.
void evtDirSailStorm(bool bIsTornado)
{
    bStormPrompt = true;
    bStormPromptResolved = false;
    bStormPromptTornado = bIsTornado;
    nTimeout = 0;

    // The normal layout parks B_CANCEL off-screen. Restore the two-button
    // layout only for the storm diverter, where both choices are meaningful.
    SetNodePosition("B_OK", 270, 432, 395, 464);
    SetNodePosition("B_CANCEL", 405, 432, 530, 464);
    SetNodeUsing("B_OK", true);
    SetSelectable("B_OK", true);
    SetNodeUsing("B_CANCEL", true);
    SetSelectable("B_CANCEL", true);
    SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, "B_OK", 0, "#" + XI_ConvertString("StopStorm"));
    SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, "B_CANCEL", 0, "#" + XI_ConvertString("StartStorm"));
    SetCurrentNode("B_OK");

    trace("DS STORM DIVERTER UI READY: tornado=" + bStormPromptTornado);
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

void IProcessFrame() //Interface screens cannot use event delay
{
    nTimeout ++;
    if(nTimeout > DSENC_TIMEOUT) {
        ProcessCancelExit();
        return;
    }

    if(bStormPrompt)
    {
        if(!bShipsDesc && nTimeout > 1)
        {
            bShipsDesc = true;
            totalInfo = XI_ConvertString("StormRightAhead");
            SetFormatedText("INFO_TEXT", totalInfo);
            SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);
            SetFormatedText("INFO_TEXT_QUESTION", "");
            SetCurrentNode("B_OK");
        }
        return;
    }
    else {
        // Ordinary DirectSail encounters are unavoidable once intercepted.
        // B_CANCEL exists only so the storm diverter can reuse this interface.
        // Keep it both non-selectable and physically off-screen: TEXTBUTTON2
        // can remain rendered even when SetNodeUsing(false) is applied.
        SetNodeUsing("B_CANCEL", false);
        SetSelectable("B_CANCEL", false);
        SetNodePosition("B_CANCEL", -1000, -1000, -875, -968);
        SetNodePosition("B_OK", 335, 432, 465, 464);

        if(!bShipsDesc && nTimeout > 1) {
            nDesc = nTimeout;
            bShipsDesc = true;
            ReleaseMapEncounters();
            if(CheckAttribute(rRawGroup, "type")) {
                if(rRawGroup.type == "Warring" || rRawGroup.type == "Attacked") {
                    if(rRawGroup.type == "Warring")
                        findType = "Attacked";
                    else
                        findType = "Warring";
                    findWarring(findType);
                }
            }
            grpTrans[0].grpID = "";
            grpTrans[0].grpIDTrn = "";
            grpTrans[1].grpID = "";
            grpTrans[1].grpIDTrn = "";
            assignByID(origEncID);
            grpTNnum = 0;
            origEID = doDescribe(grpTNnum);
            GetBearing();

            if(altEncID != "") {
                assignByID(altEncID);
                grpTNnum++;
                altEID = doDescribe(grpTNnum);
            }
            if (isShipEncounterType > 1)
            {
               totalInfo = XI_ConvertString("battle on course") + totalInfo;
            }
            else
            {
                if(bSpecialEncounter)
                {
                    totalInfo = XI_ConvertString("SpecialSituation") + totalInfo;

                    if(iRealEncounterType == ENCOUNTER_TYPE_BARREL)
                    {
                        SetNewPicture("INFO_PICTURE", "loading\polundra.tga");
                    }
                    if(iRealEncounterType == ENCOUNTER_TYPE_BOAT)
                    {
                        SetNewPicture("INFO_PICTURE", "loading\flplndra.tga");
                    }

                    SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE,
                                "B_OK", 0, "#" + XI_ConvertString("GetItemToBort"));
                }
                else
                {
                    totalInfo = XI_ConvertString("someone sails") + totalInfo;
                }
            }
            SetFormatedText("INFO_TEXT",totalInfo);
            SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5); //align
            SetFormatedText("INFO_TEXT_QUESTION", dirOff);
            SetCurrentNode("B_OK");
        }
        else {
            if (!bShipsSpawned && bShipsDesc && nTimeout > nDesc) {
                assignByID(origEncID);

                // Vanilla map.c calls wdmEnterSeaQuest() before reloading to
                // tactical sea. Direct Sail is already in tactical sea, so
                // apply the same quest hook before locDirSail() creates the
                // quest group and decides its relation/task. This is essential
                // for friendly quest contacts such as ordinary Sharp meetings.
                if(sQuestSeaCharId != "" && !bQuestEnterHookApplied)
                {
                    wdmEnterSeaQuest(sQuestSeaCharId);
                    bQuestEnterHookApplied = true;
                }

                for(int i = 0; i < 2; i++) {
                    grpTrans[i].grpIDTrn = uniqueGName(grpTrans[i].grpID);
                }
                //trace("locDirSail will spawn ships " + origEID);
                locDirSail(origEID);
                if(altEID > -1) {
                    //trace("locDirSail will spawn battle " + altEID);
                    assignByID(altEncID);
                    locDirSail(altEID);
                }
                //Need CheckStartPosition
                AISea.isDone = "";
                DoQuestCheckDelay("NationUpdate", 0.1);
            }
        }
    }
}
//extern call from Islands_loader.c
void evtDirSail(aref rRawG, aref encDataForS, int iNumMerchantS, int iNumWarS,
                float playerShipX, float playerShipZ, float locx, float locay, float locz,
                float dst, float fScale, string eID)
{
    if(eID == "") ProcessCancelExit();
    rRawGroup = rRawG;
    encDataForSlot = encDataForS;
    iNumMerchantShips = iNumMerchantS;
    iNumWarShips = iNumWarS;
    //trace("evtDirSail " + eID);
    //trace("evtDirSail #Merch " + iNumMerchantShips);
    //trace("evtDirSail #War " + iNumWarShips);
    RTplayerShipX = playerShipX;
    RTplayerShipZ = playerShipZ;
    x = locx;
    ay = locay;
    z = locz;
    origX = x;
    origZ = z;
    encID = eID;
    origEncID = eID;
    dstScale = fScale;
    isShipEncounterType++;
}

void assignByID(string eID)
{
    encID = eID;
    makearef(rRawGroup, worldmap.encounters.(eID));
    makearef(encDataForSlot, rRawGroup.encdata);

    if(CheckAttribute(encDataForSlot, "NumMerchantShips"))
    {
        iNumMerchantShips = sti(encDataForSlot.NumMerchantShips);
    }
    if(CheckAttribute(encDataForSlot, "NumWarShips"))
    {
        iNumWarShips = sti(encDataForSlot.NumWarShips);
    }
    //trace("assignByID " + eID);
    //trace("assignByID #Merch " + iNumMerchantShips);
    //trace("assignByID #War " + iNumWarShips);
    x = stf(rRawGroup.x) * dstScale;
    z = stf(rRawGroup.z)* dstScale;
    ay = stf(rRawGroup.ay);
}

int doDescribe(int gNum)
{
    int mapEncSlot = FindFreeMapEncounterSlot();
    if(mapEncSlot < 0) return;
    rEncounter = GetMapEncounterRef(mapEncSlot);
    CopyAttributes(rEncounter, encDataForSlot);

    if (!CheckAttribute(rEncounter, "RealEncounterType"))
    {
        ProcessCancelExit();
        return -1;
    }
    //trace("doDescribe #Merch " + iNumMerchantShips);
    //trace("doDescribe #War " + iNumWarShips);
    //trace("doDescribe RealEncounterType " + rEncounter.RealEncounterType);
    grpTrans[gNum].grpID = DIR_SEA_GROUP_MARK + rEncounter.GroupName;
    grpTrans[gNum].grpIDTrn = DIR_SEA_GROUP_MARK + rEncounter.GroupName;
    iEncounterType = sti(rEncounter.RealEncounterType);
    iRealEncounterType = iEncounterType;
    //trace ("RealEncounterType is " + iEncounterType);

    //Get description
    if (isShipEncounterType > 1 && gNum > 0)
    {
        totalInfo = totalInfo + XI_ConvertString("But in the same way");
    }
    if (CheckAttribute(rEncounter, "CharacterID"))
    {
        int nEncChar = GetCharacterIndex(rEncounter.CharacterID);
        if (nEncChar != -1)
        {
            //#20191015-01
            sQuestSeaCharId = characters[nEncChar].id;
            if (CheckAttribute(&characters[nEncChar], "mapEnc.Name"))
            {
                totalInfo = totalInfo + characters[nEncChar].mapEnc.Name;
            }
            else
            {
                totalInfo = totalInfo + "'" + characters[nEncChar].ship.name + "'.";
            }
        }
    }
    else
    {
        if(iRealEncounterType <= ENCOUNTER_TYPE_MERCHANT_LARGE)
        {
            totalInfo = totalInfo + GetTextOnShipsQuantity(iNumMerchantShips) + XI_ConvertString("of traders");
        }
        if(iRealEncounterType >= ENCOUNTER_TYPE_MERCHANT_GUARD_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_MERCHANT_GUARD_LARGE)
        {
            totalInfo = totalInfo + GetTextOnShipsQuantity(iNumMerchantShips) + XI_ConvertString("merchants in accompaniment") + GetTextOnSecondShipsQuantity(iNumWarShips) + XI_ConvertString("guards");
        }
        if(iRealEncounterType >= ENCOUNTER_TYPE_ESCORT_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_ESCORT_LARGE)
        {
            totalInfo = totalInfo + XI_ConvertString("Trade caravan") + GetTextOnShipsQuantity(iNumMerchantShips) + XI_ConvertString("merchants in accompaniment") + GetTextOnSecondShipsQuantity(iNumWarShips) + XI_ConvertString("guards");
        }
        if(iRealEncounterType >= ENCOUNTER_TYPE_PATROL_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_PATROL_LARGE)
        {
            totalInfo = totalInfo + XI_ConvertString("Patrol") + GetTextOnShipsQuantity(iNumWarShips);
        }
        if(iRealEncounterType >= ENCOUNTER_TYPE_PIRATE_SMALL && iRealEncounterType <= ENCOUNTER_TYPE_PIRATE_LARGE)
        {
            totalInfo = totalInfo + XI_ConvertString("Pirates") + GetTextOnShipsQuantity(iNumMerchantShips + iNumWarShips);
        }
        if(iRealEncounterType >= ENCOUNTER_TYPE_SQUADRON && iRealEncounterType <= ENCOUNTER_TYPE_ARMADA)
        {
            totalInfo = totalInfo + XI_ConvertString("Naval squadron") + GetTextOnShipsQuantity(iNumWarShips);
        }
        if(iRealEncounterType == ENCOUNTER_TYPE_PUNITIVE_SQUADRON)
        {
            totalInfo = totalInfo + XI_ConvertString("Punitive expedition") + GetTextOnShipsQuantity(iNumWarShips);
        }
        if(iRealEncounterType == ENCOUNTER_TYPE_BARREL)
        {
            totalInfo = totalInfo + XI_ConvertString("SailingItems");
            bSpecialEncounter = true;
        }
        if(iRealEncounterType == ENCOUNTER_TYPE_BOAT)
        {
            totalInfo = totalInfo + XI_ConvertString("ShipWreck");
            bSpecialEncounter = true;
        }
    }

    // Special encounters are physical objects rather than ships, so do not
    // append a national flag description to their Watchman text.
    if(iRealEncounterType != ENCOUNTER_TYPE_BARREL &&
       iRealEncounterType != ENCOUNTER_TYPE_BOAT)
    {
        if(sti(rEncounter.Nation) < 0)
        {
            totalInfo = totalInfo + "Error: rEncounter.Nation < 0.";
        }
        switch(sti(rEncounter.Nation))
        {
            case ENGLAND:
                totalInfo = totalInfo + XI_ConvertString("under english flag");
            break;
            case FRANCE:
                totalInfo = totalInfo + XI_ConvertString("under french flag");
            break;
            case SPAIN:
                totalInfo = totalInfo + XI_ConvertString("under spanish flag");
            break;
            case HOLLAND:
                totalInfo = totalInfo + XI_ConvertString("under dutch flag");
            break;
            case PIRATE:
                totalInfo = totalInfo + ".";
            break;
        }
    }
    return mapEncSlot;
}

void GetBearing()
{
    encounterbearing = GetAngleY(x - RTplayerShipX, z - RTplayerShipZ);
	float offShip = encounterbearing - stf(pchar.Ship.Ang.y);

	int nBear = ClosestDirE(offShip);
	switch(nBear)
    {
    case DIR_PORTBOW:
        dirOff = XI_ConvertString("PortBow");
        break;
    case DIR_FORWARD:
        dirOff = XI_ConvertString("Bow");
        break;
   case DIR_STARBOW:
        dirOff = XI_ConvertString("StarBow");
        break;
    case DIR_STAR:
        dirOff = XI_ConvertString("StarSide");
        break;
    case DIR_PORT:
        dirOff = XI_ConvertString("PortSide");
        break;
    case DIR_PORTSTERN:
        dirOff = XI_ConvertString("PortStern");
        break;
    case DIR_ABAFT:
        dirOff = XI_ConvertString("Abaft");
        break;
    case DIR_STARSTERN:
        dirOff = XI_ConvertString("StarStern");
        break;
    }
}
//For GetRelation returning similar group names as friends
string uniqueGName(string gname)
{
    bool bFound = true;
    string sTemp = gname;
    while(bFound){
        bFound = canFindGroup(sTemp);
        if(bFound)
            sTemp = gname + rand(50) + rand(50) + rand(50);
        else
            break;
    }
    return sTemp;
}

bool canFindGroup(string gname)
{
    int nFnd = 	Group_FindGroup(gname);
    if(nFnd > -1) return true;

    return false;
}

void locDirSail(int evtID)
{
    int i, j, nCheckShipCnt;
    bShipsSpawned = true;

    ref rCharacter, rGroup, rFantom;
    rEncounter = GetMapEncounterRef(evtID);
    CopyAttributes(rEncounter, encDataForSlot);
    //#20191123-03
    if (!CheckAttribute(rEncounter, "RealEncounterType") || CheckAttribute(rRawGroup, "dirSailEnc"))
    {
        return;
    }
    iEncounterType = sti(rEncounter.RealEncounterType);
    iRealEncounterType = iEncounterType;

    //From SeaLogin() WdmAddEncountersData()
    // login encounters
	object oResult;
	int iFantomIndex;
    int iAloneCharIndex = -1;

    x = stf(pchar.Ship.Pos.x) + 2500 * sin(encounterbearing);
    z = stf(pchar.Ship.Pos.z) + 2500 * cos(encounterbearing);

    encStringID = "encounters." + encID;
    if(!CheckAttribute(&worldMap, encStringID + ".quest"))
    {
        worldMap.(encStringID).needDelete = "Reload delete non quest encounter";
    }
    Sea_FreeTaskList();

    int iCompanionsQ;
    int cn;
    string sGName = "";
    bool bQuestGroupLogged = false;
    nCheckShipCnt = iNumShips;
    if (iEncounterType == ENCOUNTER_TYPE_ALONE)
    {
        iAloneCharIndex = GetCharacterIndex(rEncounter.CharacterID);
        if (iAloneCharIndex < 0)
        {
            ProcessCancelExit();
            return;
        }
        sGName = "Sea_" + rEncounter.CharacterID;
        rGroup = Group_GetGroupByID(sGName);
        iCompanionsQ = GetCompanionQuantity(&Characters[iAloneCharIndex]);
        nCheckShipCnt += iCompanionsQ;
        //#20191015-01
        if (CheckAttribute(rGroup, "AlreadyLoaded") || nCheckShipCnt > MAX_SHIPS_IN_LOCATION)
        {
            sQuestSeaCharId = "";
            ProcessCancelExit();
            return;
        }
        Group_AddCharacter(sGName, rEncounter.CharacterID);

        if(iCompanionsQ > 1)
        {
            for(int k = 1; k < COMPANION_MAX; k++)
            {
                cn = GetCompanionIndex(&characters[iAloneCharIndex], k);
                if (cn != -1)
                {
                    Group_AddCharacter(sGName, characters[cn].id);
                }
            }
        }
        Group_SetGroupCommander(sGName, characters[iAloneCharIndex].id);
        if(GetNationRelation2MainCharacter(sti(characters[iAloneCharIndex].nation)) == RELATION_ENEMY)
        {
            Group_SetTaskAttack(sGName, PLAYER_GROUP);
            Group_LockTask(sGName);
        }
        rEncounter.qID = sGName;
    }
    else
    {
        nCheckShipCnt += iNumWarShips;
        nCheckShipCnt += iNumMerchantShips;
        if (nCheckShipCnt > MAX_SHIPS_IN_LOCATION)
        {
            sQuestSeaCharId = "";
            ProcessCancelExit();
            return;
        }
        for(i = 0; i < 2; i++) {
            string chkG = DIR_SEA_GROUP_MARK + rEncounter.GroupName;
            if(grpTrans[i].grpID == chkG) {
                sGName = grpTrans[i].grpIDTrn;
                break;
            }
        }
        //sGName = rEncounter.GroupName;
    }
    //#20191123-03
    rRawGroup.dirSailEnc = true;
    // check for Quest fantom
    if (CheckAttribute(rEncounter, "qID"))
    {
        Trace("checkWMEnctr: Login quest encounter " + rEncounter.qID);
        Group_SetAddressNone(rEncounter.qID);
        Group_SetXZ_AY(rEncounter.qID, x, z, ay);
        Sea_LoginGroup(rEncounter.qID);
        bQuestGroupLogged = true;

        // Quest encounters intentionally remain live after tactical entry, so
        // unlike ordinary encounters they never acquire needDelete. Stop the
        // Watchman explicitly once the tracked quest group has actually logged
        // into tactical sea.
        DirectSail_WatchmanQuestEncounterStarted(encID);

    }
    if(sGName != "" && !bQuestGroupLogged) {
        Sea_AddGroup2TaskList(sGName);

        rGroup = Group_FindOrCreateGroup(sGName);

        Group_SetXZ_AY(sGName, x, z, ay);
        Group_SetType(sGName, rEncounter.Type);
        Group_DeleteAtEnd(sGName);

        // copy task attributes from map encounter to fantom group
        if (CheckAttribute(rEncounter, "Task"))
        {
            rGroup.Task = rEncounter.Task;
        }
        if (CheckAttribute(rEncounter, "Task.Target"))
        {
            rGroup.Task.Target = rEncounter.Task.Target;
        }
        if (CheckAttribute(rEncounter, "Task.Pos"))
        {
            rGroup.Task.Target.Pos.x = rEncounter.Task.Pos.x;
            rGroup.Task.Target.Pos.z = rEncounter.Task.Pos.z;
        }
        if (CheckAttribute(rEncounter, "Lock") && sti(rEncounter.Lock)) { Group_LockTask(sGName); }

        int iNation = sti(rEncounter.Nation);
        int iNumFantomShips = Fantom_GenerateEncounterExt(sGName, &oResult, iEncounterType, iNumWarShips, iNumMerchantShips, iNation);

        // CLAOP's normal SeaLogin path treats BARREL and BOAT as "empty
        // fantoms": the encounter generator reserves a character slot, then
        // EmptyFantom_DropGoodsToSea() creates the actual floating object.
        if(iEncounterType == ENCOUNTER_TYPE_BARREL)
        {
            iFantomIndex = FANTOM_CHARACTERS + iNumFantoms;
            rFantom = &Characters[iFantomIndex];
            rFantom.id = "EncBarrel_" + iFantomIndex;
            rFantom.index = iFantomIndex;
            rFantom.Nation = PIRATE;
            rFantom.EncType = "pirate";
            rFantom.RealEncounterType = iEncounterType;
            rFantom.reputation = 5;
            rFantom.EncGroupName = sGName;
            rFantom.MainCaptanId = Characters[iFantomIndex].id;
            rFantom.location = sIslandID;
            rGroup.EmptyFantom = true;
            SetFantomParamFromRank(rFantom, 1, false);
            rFantom.SeaAI.Group.Name = sGName;
            Group_AddCharacter(sGName, rFantom.id);

            trace("DS SPECIAL SPAWN: Barrel encounter=" + encID +
                  " fantom=" + iFantomIndex +
                  " x=" + x + " z=" + z);

            EmptyFantom_DropGoodsToSea(rFantom, iEncounterType);
            return;
        }

        if(iEncounterType == ENCOUNTER_TYPE_BOAT)
        {
            iFantomIndex = FANTOM_CHARACTERS + iNumFantoms;
            rFantom = &Characters[iFantomIndex];
            rFantom.id = iFantomIndex;
            rFantom.index = iFantomIndex;
            rFantom.Nation = PIRATE;
            rFantom.EncType = "pirate";
            rFantom.RealEncounterType = iEncounterType;
            rFantom.EncGroupName = sGName;
            rFantom.MainCaptanId = Characters[iFantomIndex].id;
            rFantom.location = sIslandID;
            rGroup.EmptyFantom = true;
            rFantom.sex = "man";
            rFantom.model.animation = "man";
            SetCaptanModelByEncType(rFantom, rFantom.EncType);
            SetRandomNameToCharacter(rFantom);
            SetSeaFantomParam(rFantom, rEncounter.Type);
            rFantom.SeaAI.Group.Name = sGName;
            Group_AddCharacter(sGName, rFantom.id);

            trace("DS SPECIAL SPAWN: Boat encounter=" + encID +
                  " fantom=" + iFantomIndex +
                  " x=" + x + " z=" + z);

            EmptyFantom_DropGoodsToSea(rFantom, iEncounterType);
            return;
        }

        // CLAOP Direct Sail: sea.c line-layout optimisation omitted here.
        // load ship to sea
        if (iNumFantomShips)
        {
            for (j=0; j<iNumFantomShips; j++)
            {
                iFantomIndex = FANTOM_CHARACTERS + iNumFantoms - iNumFantomShips + j;
                rFantom = &Characters[iFantomIndex];
                DeleteAttribute(rFantom, "items");
                rFantom.id = "fenc_" + iFantomIndex;
                rFantom.location = sIslandID;
                // set commander to group
                if (j==0) { Group_SetGroupCommander(sGName, Characters[iFantomIndex].id); }

                // set random character and ship names, face id
                rFantom.sex = "man";
                rFantom.model.animation = "man";
                rFantom.Nation = rEncounter.Nation;
                rFantom.reputation = 5+rand(84);
                rFantom.EncType = rEncounter.Type; // war, trade pirate
                rFantom.RealEncounterType = iEncounterType;
                rFantom.EncGroupName = sGName;
                rFantom.MainCaptanId = Characters[iFantomIndex - j].id;
                rFantom.WatchFort = true;
                rFantom.AnalizeShips = true;
                DeleteAttribute(rFantom, "surrendered");

                if (CheckAttribute(rFantom, "Ship.Mode"))
                {
                    SetCaptanModelByEncType(rFantom, rFantom.Ship.Mode);
                }
                else
                {
                    SetCaptanModelByEncType(rFantom, rEncounter.Type);
                }
                SetRandomNameToCharacter(rFantom);
                SetRandomNameToShip(rFantom);

                SetSeaFantomParam(rFantom, rEncounter.Type);

                Fantom_SetCannons(rFantom, rEncounter.Type);
                Fantom_SetSails(rFantom, rEncounter.Type);
                rFantom.SeaAI.Group.Name = sGName;
                rFantom.Experience = 0;
                rFantom.Skill.FreeSkill = 0;

                DeleteAttribute(rFantom, "ShipSails.gerald_name");
                if (j == 0 || GetCharacterShipClass(rFantom) == 1)
                {
                    SetRandGeraldSail(rFantom, sti(rFantom.Nation));
                }
                // add fantom
                Group_AddCharacter(sGName, rFantom.id);

                Ship_Add2Sea(iFantomIndex, 0, rEncounter.Type, true);
            }
        }
    }
	// set tasks 2 all groups
	for (i=0; i<GetArraySize(&sTaskList)-2; i++)
	{
		string sGroupID = sTaskList[i];
		rGroup = Group_GetGroupByID(sGroupID);
		string sTranTrg = "";
		if(CheckAttribute(rGroup, "Task.Target")) {
            sTranTrg = rGroup.Task.Target;
            for(j = 0; j < 2; j++) {
                if(grpTrans[j].grpID == sTranTrg) {
                    sTranTrg = grpTrans[j].grpIDTrn;
                    break;
                }
            }
		}
		// set task
		switch (sti(rGroup.Task))
		{
			case AITASK_RUNAWAY:
				Group_SetTaskRunAway(sGroupID, sTranTrg); //rGroup.Task.Target);
			break;
			case AITASK_ATTACK:
				Group_SetTaskAttack(sGroupID, sTranTrg); //rGroup.Task.Target);
			break;
			case AITASK_MOVE:
				if (CheckAttribute(rGroup, "Task.Target.Pos"))
				{
				    Group_SetTaskMove(sGroupID, stf(rGroup.Task.Target.Pos.x), stf(rGroup.Task.Target.Pos.z));
				}
				else
				{
					x = 10000.0 * sin(stf(rGroup.Pos.ay));
					z = 10000.0 * cos(stf(rGroup.Pos.ay));
					Group_SetTaskMove(sGName, x, z);
				}
			break;
		}
		rCharacter = Group_GetGroupCommanderR(rGroup);
		int iRelation = GetRelation(nMainCharacterIndex, sti(rCharacter.index));

		// set relations to all characters in this group
		int qq = 0;
		while (true)
		{
			int iCharacterIndex = Group_GetCharacterIndexR(rGroup, qq); qq++;
			if (iCharacterIndex < 0) { break; }
			SetCharacterRelationBoth(iCharacterIndex, nMainCharacterIndex, iRelation);
		}
	}
}


void ProcessBreakExit()
{
    if(bStormPrompt && !bStormPromptResolved)
    {
        DirectSail_StormPromptAgainst();
        return;
    }
	IDoExit( RC_INTERFACE_MAP_EXIT );
}

void ProcessCancelExit()
{
    if(bStormPrompt && !bStormPromptResolved)
    {
        DirectSail_StormPromptAgainst();
        return;
    }
	IDoExit( RC_INTERFACE_MAP_EXIT );
}

void IDoExit(int exitCode)
{
    ReleaseMapEncounters();
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("frame","IProcessFrame");

	if (sQuestSeaCharId != "" && !bQuestEnterHookApplied)
    {
        wdmEnterSeaQuest(sQuestSeaCharId);
        bQuestEnterHookApplied = true;
    }
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

    if(bStormPrompt)
    {
        if(comName == "activate")
        {
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

        if(comName == "click")
        {
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
    }

	if(nodName == "B_OK")
	{
		if(comName == "activate" || comName == "click")
		{
			IDoExit(RC_INTERFACE_MAP_EXIT);
		}
	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}
//Same as Islands_loader.c to find next group
void findWarring(string fType)
{
    aref WME;
    makearef(WME, worldmap.encounters);
    int nWME = GetAttributesNum(WME);
    int i;
    int tShips, nShips;
    float dist;

    nShips = iNumShips + iNumMerchantShips + iNumWarShips;
    for(i = 0; i < nWME; i++)
    {
        tShips = nShips;
        iNumWarShips = 0;
        iNumMerchantShips = 0;
        rRawGroup = GetAttributeN(WME, i);
        encID = GetAttributeName(rRawGroup);
        if(!CheckAttribute(rRawGroup, "type") || rRawGroup.type != fType) continue;
        if(encID == origEncID) continue;

        if(!CheckAttribute(rRawGroup, "encdata")) continue;
        if(CheckAttribute(rRawGroup, "needDelete")) continue;

        makearef(encDataForSlot, rRawGroup.encdata);

        if(CheckAttribute(encDataForSlot, "NumMerchantShips"))
        {
            iNumMerchantShips = sti(encDataForSlot.NumMerchantShips);
            tShips += iNumMerchantShips;
        }
        if(CheckAttribute(encDataForSlot, "NumWarShips"))
        {
            iNumWarShips = sti(encDataForSlot.NumWarShips);
            tShips += iNumWarShips;
        }
        if(tShips > MAX_SHIPS_IN_LOCATION) continue;
        x = stf(rRawGroup.x) * dstScale;
		z = stf(rRawGroup.z)* dstScale;
		ay = stf(rRawGroup.ay);
        dist = GetDistance2DRel(origX, origZ, x, z);

        if(dist > DIR_SAIL_WAR_DIST)
            continue;

        altEncID = encID;
        isShipEncounterType++;
        break;
    }
}
