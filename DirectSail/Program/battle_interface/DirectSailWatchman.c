// ============================================================================
// Direct Sail - Watchman
// Beta release-candidate implementation.
//
// Watchman exposes the shared persistent world-map encounter population while
// sailing in Direct Sail. Contacts can be selected and tracked across sea-zone
// transitions, with hourly bearing/range reports.
// ============================================================================

#define DS_WATCHMAN_MAX_CONTACTS             8
#define DS_WATCHMAN_RADIUS_SQ                62500.0
#define DS_WATCHMAN_NEAR_SQ                   8100.0
#define DS_WATCHMAN_MODERATE_SQ              28900.0

#define DS_WATCHMAN_TEXTURE                      5

// User-provided DirectSail Watchman atlas: 16 x 2, 64x64 cells.
// Selected/highlighted row:
#define DS_WATCHMAN_ICON_SPAIN_SEL               0
#define DS_WATCHMAN_ICON_FRANCE_SEL              1
#define DS_WATCHMAN_ICON_ENGLAND_SEL             2
#define DS_WATCHMAN_ICON_HOLLAND_SEL             3
#define DS_WATCHMAN_ICON_PIRATE_SEL              4
#define DS_WATCHMAN_ICON_LOOKOUT_SEL             5

// Normal row:
#define DS_WATCHMAN_ICON_SPAIN                  16
#define DS_WATCHMAN_ICON_FRANCE                 17
#define DS_WATCHMAN_ICON_ENGLAND                18
#define DS_WATCHMAN_ICON_HOLLAND                19
#define DS_WATCHMAN_ICON_PIRATE                 20
#define DS_WATCHMAN_ICON_LOOKOUT                21

// Native list_icons.tga pairs.
#define DS_WATCHMAN_ICON_BATTLE_SEL             33
#define DS_WATCHMAN_ICON_BATTLE                 49
#define DS_WATCHMAN_ICON_SPECIAL_SEL            38
#define DS_WATCHMAN_ICON_SPECIAL                54
#define DS_WATCHMAN_ICON_QUEST_SEL              34
#define DS_WATCHMAN_ICON_QUEST                  50

// Equivalent to DirSailEnc.c's 2000 tactical-unit Warring pairing radius.
#define DS_WATCHMAN_WARRING_TACTICAL_RADIUS_SQ  4000000.0

string DirectSail_WatchmanText(string sKey)
{
    int iFile;
    string sText;

    iFile = LanguageOpenFile("DirectSailWatchman.txt");
    if(iFile < 0) return sKey;

    sText = LanguageConvertString(iFile, sKey);
    LanguageCloseFile(iFile);

    return sText;
}

string DirectSail_WatchmanDirection(float fDx, float fDz)
{
    float fAbsX = fDx;
    float fAbsZ = fDz;

    if(fAbsX < 0.0) fAbsX = -fAbsX;
    if(fAbsZ < 0.0) fAbsZ = -fAbsZ;

    if(fAbsZ > fAbsX * 2.0)
    {
        if(fDz >= 0.0) return DirectSail_WatchmanText("DirectionNorth");
        return DirectSail_WatchmanText("DirectionSouth");
    }

    if(fAbsX > fAbsZ * 2.0)
    {
        if(fDx >= 0.0) return DirectSail_WatchmanText("DirectionEast");
        return DirectSail_WatchmanText("DirectionWest");
    }

    if(fDx >= 0.0 && fDz >= 0.0) return DirectSail_WatchmanText("DirectionNorthEast");
    if(fDx >= 0.0 && fDz < 0.0) return DirectSail_WatchmanText("DirectionSouthEast");
    if(fDx < 0.0 && fDz >= 0.0) return DirectSail_WatchmanText("DirectionNorthWest");
    return DirectSail_WatchmanText("DirectionSouthWest");
}

string DirectSail_WatchmanRange(float fDistSq)
{
    if(fDistSq <= DS_WATCHMAN_NEAR_SQ)
        return DirectSail_WatchmanText("RangeNearby");

    if(fDistSq <= DS_WATCHMAN_MODERATE_SQ)
        return DirectSail_WatchmanText("RangeModerate");

    return DirectSail_WatchmanText("RangeDistant");
}

int DirectSail_WatchmanNation(aref rEnc)
{
    if(CheckAttribute(rEnc, "encdata.Nation"))
        return sti(rEnc.encdata.Nation);

    if(CheckAttribute(rEnc, "Nation"))
        return sti(rEnc.Nation);

    return -1;
}

string DirectSail_WatchmanNationTextByID(int iNation)
{
    switch(iNation)
    {
        case SPAIN:   return DirectSail_WatchmanText("NationSpanish"); break;
        case FRANCE:  return DirectSail_WatchmanText("NationFrench"); break;
        case ENGLAND: return DirectSail_WatchmanText("NationEnglish"); break;
        case HOLLAND: return DirectSail_WatchmanText("NationDutch"); break;
        case PIRATE:  return DirectSail_WatchmanText("NationPirate"); break;
    }

    return DirectSail_WatchmanText("NationUnknown");}

string DirectSail_WatchmanBattleNationText(int iNation)
{
    switch(iNation)
    {
        case SPAIN:   return DirectSail_WatchmanText("BattleNationSpain"); break;
        case FRANCE:  return DirectSail_WatchmanText("BattleNationFrance"); break;
        case ENGLAND: return DirectSail_WatchmanText("BattleNationEngland"); break;
        case HOLLAND: return DirectSail_WatchmanText("BattleNationHolland"); break;
        case PIRATE:  return DirectSail_WatchmanText("BattleNationPirates"); break;
    }

    return DirectSail_WatchmanText("NationUnknown");
}

int DirectSail_WatchmanFlagPicNormal(aref rEnc)
{
    int iNation = DirectSail_WatchmanNation(rEnc);

    switch(iNation)
    {
        case SPAIN:   return DS_WATCHMAN_ICON_SPAIN; break;
        case FRANCE:  return DS_WATCHMAN_ICON_FRANCE; break;
        case ENGLAND: return DS_WATCHMAN_ICON_ENGLAND; break;
        case HOLLAND: return DS_WATCHMAN_ICON_HOLLAND; break;
        case PIRATE:  return DS_WATCHMAN_ICON_PIRATE; break;
    }

    return DS_WATCHMAN_ICON_PIRATE;
}

int DirectSail_WatchmanFlagPicSelected(aref rEnc)
{
    int iNation = DirectSail_WatchmanNation(rEnc);

    switch(iNation)
    {
        case SPAIN:   return DS_WATCHMAN_ICON_SPAIN_SEL; break;
        case FRANCE:  return DS_WATCHMAN_ICON_FRANCE_SEL; break;
        case ENGLAND: return DS_WATCHMAN_ICON_ENGLAND_SEL; break;
        case HOLLAND: return DS_WATCHMAN_ICON_HOLLAND_SEL; break;
        case PIRATE:  return DS_WATCHMAN_ICON_PIRATE_SEL; break;
    }

    return DS_WATCHMAN_ICON_PIRATE_SEL;
}

bool DirectSail_WatchmanIsWarring(aref rEnc)
{
    if(CheckAttribute(rEnc, "type") && rEnc.type == "Warring")
        return true;

    if(CheckAttribute(rEnc, "dsGeneratedKind") && rEnc.dsGeneratedKind == "Warring")
        return true;

    return false;
}

int DirectSail_WatchmanSpecialRealType(aref rEnc)
{
    if(!CheckAttribute(rEnc, "encdata.Type")) return -1;
    if(rEnc.encdata.Type != "special") return -1;
    if(!CheckAttribute(rEnc, "encdata.RealEncounterType")) return -1;

    return sti(rEnc.encdata.RealEncounterType);
}

bool DirectSail_WatchmanIsSpecialContact(aref rEnc)
{
    int iType = DirectSail_WatchmanSpecialRealType(rEnc);

    if(iType == ENCOUNTER_TYPE_BARREL) return true;
    if(iType == ENCOUNTER_TYPE_BOAT) return true;

    return false;
}

// -----------------------------------------------------------------------------
// TEMPORARY DIAGNOSTIC: roaming quest encounters
// -----------------------------------------------------------------------------
// Return the quest's own world-map display name where possible. This mirrors
// DirSailEnc.c so named quest ships are recognisable in the Watchman.
string DirectSail_WatchmanQuestLabel(aref rEnc)
{
    int nEncChar;
    string sChrID;

    sChrID = "";

    if(CheckAttribute(rEnc, "encdata.CharacterID"))
        sChrID = rEnc.encdata.CharacterID;
    else
    {
        if(CheckAttribute(rEnc, "quest.chrID"))
            sChrID = rEnc.quest.chrID;
    }

    if(sChrID == "") return "";

    nEncChar = GetCharacterIndex(sChrID);
    if(nEncChar != -1)
    {
        if(CheckAttribute(&characters[nEncChar], "mapEnc.Name"))
            return characters[nEncChar].mapEnc.Name;

        if(CheckAttribute(&characters[nEncChar], "Ship.Name"))
            return characters[nEncChar].Ship.Name;
    }

    return sChrID;
}

// Log quest requests waiting to become world-map encounters and fully-created
// persistent quest encounters once per game hour. Position data is diagnostic
// only; this test deliberately does not move quest fleets in Direct Sail.

// -----------------------------------------------------------------------------
// Quest encounter promotion while Direct Sail is active.
//
// The vanilla handler expects an active world-map entity. Calling it from sea
// can treat creation failure as quest-arrival success and clear the queue, so
// Direct Sail constructs the persistent encounter shell directly instead.
//
// Trader encounters use their vanilla route origin/destination.
// Warrior/coolwarrior encounters are created around the player, matching
// vanilla's wdmCreateRealFollowShipByIndex() behaviour, but outside the
// immediate Direct Sail interception radius and within Watchman range.
// -----------------------------------------------------------------------------
int DirectSail_QuestPromoteQueuedEncounters()
{
    aref rQueuedRoot;
    aref rQueued;
    aref rLiveRoot;
    aref rLive;
    ref rGenerated;
    ref rRouteColony;
    ref rRouteLocation;
    aref rNew;
    aref rShellSource;
    aref rIslands;
    aref rIsland;
    aref rAnchor;

    int i;
    int j;
    int k;
    int iNum;
    int iLiveNum;
    int iMapSlot;
    int iScale;
    int iFree;
    int iPromoted;
    int iTimeOut;
    int iCurrentDay;
    int iCurrentHour;
    int iRouteColony;
    int iRouteLocation;

    float fPlayerX;
    float fPlayerZ;
    float fAngle;
    float fRadius;
    float fStartX;
    float fStartZ;
    float fDestX;
    float fDestZ;
    float fTimeOutSec;
    float fDaysPerSec;
    float fRouteDx;
    float fRouteDz;
    float fRouteDistSq;

    bool bAlreadyLive;
    bool bStartResolved;
    bool bDestResolved;
    bool bHideQuestContact;

    string sSlot;
    string sQueuePath;
    string sChr;
    string sKind;
    string sRootType;
    string sQuestEvent;
    string sNewID;
    string sNewPath;
    string sSourceID;
    string sRouteStart;
    string sRouteDest;
    string sStartAlias;
    string sDestAlias;
    string sRouteIsland;
    string sRouteCity;
    string sRouteAlias;
    string sRoutePath;
    string sAnchorName;

    bool bIslandHasPos;
    bool bAnchorHasPos;
    bool bStartNameMatch;
    bool bDestNameMatch;
    bool bStartAnchorMatch;
    bool bDestAnchorMatch;
    bool bRoutePathHasPos;
    bool bRouteHasIslandAlias;
    bool bRouteUnresolved;

    iPromoted = 0;
    iCurrentDay = GetDataDay();
    iCurrentHour = MakeInt(GetHour());

    if(!CheckAttribute(&worldMap, "addQuestEncounters")) return 0;
    if(!CheckAttribute(&worldMap, "island") ||
       !CheckAttribute(&worldMap, "zeroX") ||
       !CheckAttribute(&worldMap, "zeroZ") ||
       !CheckAttribute(pchar, "Ship.Pos.x") ||
       !CheckAttribute(pchar, "Ship.Pos.z"))
    {
        trace("DS QUEST PROMOTE: player/world position unavailable; queue retained");
        return 0;
    }

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0)
    {
        trace("DS QUEST PROMOTE: invalid island scale; queue retained");
        return 0;
    }

    fPlayerX = stf(worldMap.zeroX) +
               stf(pchar.Ship.Pos.x) / makefloat(iScale);
    fPlayerZ = stf(worldMap.zeroZ) +
               stf(pchar.Ship.Pos.z) / makefloat(iScale);

    makearef(rQueuedRoot, worldMap.addQuestEncounters);
    iNum = GetAttributesNum(rQueuedRoot);

    // Iterate backwards because successful promotion deletes the queue child.
    for(i = iNum - 1; i >= 0; i--)
    {
        rQueued = GetAttributeN(rQueuedRoot, i);
        if(!CheckAttribute(rQueued, "characterID")) continue;
        if(!CheckAttribute(rQueued, "type")) continue;

        sSlot = GetAttributeName(rQueued);
        sQueuePath = "addQuestEncounters." + sSlot;
        sChr = rQueued.characterID;
        sKind = rQueued.type;

        // A queue entry which failed to resolve is retried once per game hour,
        // not every BI frame. New queue entries still promote immediately.
        if(CheckAttribute(rQueued, "DirectSailRetryDay") &&
           CheckAttribute(rQueued, "DirectSailRetryHour"))
        {
            if(sti(rQueued.DirectSailRetryDay) == iCurrentDay &&
               sti(rQueued.DirectSailRetryHour) == iCurrentHour)
            {
                continue;
            }
        }

        // Avoid creating a duplicate if this character already has a genuine
        // live quest encounter for any reason.
        bAlreadyLive = false;
        if(CheckAttribute(&worldMap, "encounters"))
        {
            makearef(rLiveRoot, worldMap.encounters);
            iLiveNum = GetAttributesNum(rLiveRoot);
            for(j = 0; j < iLiveNum; j++)
            {
                rLive = GetAttributeN(rLiveRoot, j);
                if(!CheckAttribute(rLive, "quest.chrID")) continue;
                if(rLive.quest.chrID == sChr)
                {
                    bAlreadyLive = true;
                    break;
                }
            }
        }

        if(bAlreadyLive)
        {
            trace("DS QUEST PROMOTE: chr=" + sChr +
                  " already live; removing duplicate queue slot=" + sSlot);
            DeleteAttribute(&worldMap, sQueuePath);
            continue;
        }

        sRootType = "";
        sQuestEvent = "";

        if(sKind == "trader")
        {
            sRootType = "Merchant";
            sQuestEvent = "Map_TraderSucces";
        }
        else
        {
            if(sKind == "warrior" || sKind == "coolwarrior")
            {
                sRootType = "Follow";
                sQuestEvent = "Map_WarriorEnd";
            }
        }

        if(sRootType == "")
        {
            trace("DS QUEST PROMOTE: unsupported kind=" + sKind +
                  " chr=" + sChr + "; queue retained");
            rQueued.DirectSailRetryDay = iCurrentDay;
            rQueued.DirectSailRetryHour = iCurrentHour;
            continue;
        }

        // Resolve the strategic placement before consuming the queue.
        // Traders use the same route origin/destination supplied to vanilla.
        // Real followers are intentionally created around the player: vanilla
        // wdmCreateRealFollowShipByIndex() ignores the queued beginLocator.
        bStartResolved = false;
        bDestResolved = false;
        bHideQuestContact = false;
        fStartX = 0.0;
        fStartZ = 0.0;
        fDestX = 0.0;
        fDestZ = 0.0;

        if(sKind == "trader")
        {
            if(CheckAttribute(rQueued, "XZGoto") &&
               CheckAttribute(rQueued, "x1") &&
               CheckAttribute(rQueued, "z1") &&
               CheckAttribute(rQueued, "x2") &&
               CheckAttribute(rQueued, "z2"))
            {
                fStartX = stf(rQueued.x1);
                fStartZ = stf(rQueued.z1);
                fDestX = stf(rQueued.x2);
                fDestZ = stf(rQueued.z2);
                bStartResolved = true;
                bDestResolved = true;

            }
            else
            {
                // RC5.5: resolve route tokens inline. The previous helper
                // returned false without entering its body in this script
                // runtime, so keep all resolution in this known-executing
                // promotion function.
                sRouteStart = "";
                sRouteDest = "";
                sStartAlias = "";
                sDestAlias = "";

                if(CheckAttribute(rQueued, "beginlocator"))
                {
                    sRouteStart = rQueued.beginlocator;
                    sStartAlias = sRouteStart + "_town";
                }

                if(CheckAttribute(rQueued, "endLocator"))
                {
                    sRouteDest = rQueued.endLocator;
                    sDestAlias = sRouteDest + "_town";
                }

                // Vanilla uses "none" as an unspecified trader origin for
                // some incoming quest fleets. Direct Sail has no moving world-
                // map engine to choose that origin, so place the contact at a
                // normal player-relative strategic spawn distance. The original
                // "none" token is retained for a later native world-map handoff.
                if(sRouteStart == "none")
                {
                    fAngle = frnd() * PIm2;
                    fRadius = (2500.0 / makefloat(iScale)) + 30.0 + makefloat(rand(100));
                    fStartX = fPlayerX + fRadius * sin(fAngle);
                    fStartZ = fPlayerZ + fRadius * cos(fAngle);
                    bStartResolved = true;

                    trace("DS RC5.13 ROUTE START NONE: chr=" + sChr +
                          " x=" + fStartX +
                          " z=" + fStartZ +
                          " radius=" + fRadius);
                }

                trace("DS RC5.13 ROUTE INLINE START: chr=" + sChr +
                      " start=" + sRouteStart +
                      " startAlias=" + sStartAlias +
                      " dest=" + sRouteDest +
                      " destAlias=" + sDestAlias);

                // First resolve exact world-map island/anchor names and the
                // common <colony>_town aliases in one pass.
                if(CheckAttribute(&worldMap, "islands"))
                {
                    makearef(rIslands, worldMap.islands);

                    for(j = 0; j < GetAttributesNum(rIslands); j++)
                    {
                        rIsland = GetAttributeN(rIslands, j);

                        bIslandHasPos = false;
                        if(CheckAttribute(rIsland, "position.x"))
                        {
                            if(CheckAttribute(rIsland, "position.z"))
                            {
                                bIslandHasPos = true;
                            }
                        }

                        if(bIslandHasPos)
                        {
                            sAnchorName = GetAttributeName(rIsland);

                            if(!bStartResolved)
                            {
                                bStartNameMatch = false;
                                if(sAnchorName == sRouteStart) bStartNameMatch = true;
                                if(sAnchorName == sStartAlias) bStartNameMatch = true;

                                if(bStartNameMatch)
                                {
                                    fStartX = stf(rIsland.position.x);
                                    fStartZ = stf(rIsland.position.z);
                                    bStartResolved = true;

                                    trace("DS RC5.13 ROUTE START ISLAND MATCH: " +
                                          sAnchorName +
                                          " x=" + fStartX +
                                          " z=" + fStartZ);
                                }
                            }

                            if(!bDestResolved)
                            {
                                bDestNameMatch = false;
                                if(sAnchorName == sRouteDest) bDestNameMatch = true;
                                if(sAnchorName == sDestAlias) bDestNameMatch = true;

                                if(bDestNameMatch)
                                {
                                    fDestX = stf(rIsland.position.x);
                                    fDestZ = stf(rIsland.position.z);
                                    bDestResolved = true;

                                    trace("DS RC5.13 ROUTE DEST ISLAND MATCH: " +
                                          sAnchorName +
                                          " x=" + fDestX +
                                          " z=" + fDestZ);
                                }
                            }
                        }

                        for(k = 0; k < GetAttributesNum(rIsland); k++)
                        {
                            rAnchor = GetAttributeN(rIsland, k);

                            bAnchorHasPos = false;
                            if(CheckAttribute(rAnchor, "position.x"))
                            {
                                if(CheckAttribute(rAnchor, "position.z"))
                                {
                                    bAnchorHasPos = true;
                                }
                            }

                            if(!bAnchorHasPos) continue;

                            sAnchorName = GetAttributeName(rAnchor);

                            if(!bStartResolved)
                            {
                                bStartAnchorMatch = false;
                                if(sAnchorName == sRouteStart) bStartAnchorMatch = true;
                                if(sAnchorName == sStartAlias) bStartAnchorMatch = true;

                                if(bStartAnchorMatch)
                                {
                                    fStartX = stf(rAnchor.position.x);
                                    fStartZ = stf(rAnchor.position.z);
                                    bStartResolved = true;

                                    trace("DS RC5.13 ROUTE START ANCHOR MATCH: " +
                                          sAnchorName +
                                          " x=" + fStartX +
                                          " z=" + fStartZ);
                                }
                            }

                            if(!bDestResolved)
                            {
                                bDestAnchorMatch = false;
                                if(sAnchorName == sRouteDest) bDestAnchorMatch = true;
                                if(sAnchorName == sDestAlias) bDestAnchorMatch = true;

                                if(bDestAnchorMatch)
                                {
                                    fDestX = stf(rAnchor.position.x);
                                    fDestZ = stf(rAnchor.position.z);
                                    bDestResolved = true;

                                    trace("DS RC5.13 ROUTE DEST ANCHOR MATCH: " +
                                          sAnchorName +
                                          " x=" + fDestX +
                                          " z=" + fDestZ);
                                }
                            }

                            if(bStartResolved)
                            {
                                if(bDestResolved) break;
                            }
                        }

                        if(bStartResolved)
                        {
                            if(bDestResolved) break;
                        }
                    }
                }

                trace("DS RC5.13 ROUTE AFTER SCAN: chr=" + sChr +
                      " startOK=" + bStartResolved +
                      " start=" + fStartX + "," + fStartZ +
                      " destOK=" + bDestResolved +
                      " dest=" + fDestX + "," + fDestZ);

                // Colony IDs are the common fallback. Some quests instead
                // supply a full location ID (for example PuertoPrincipe_port or
                // LeFransua_port), so if FindColony() fails derive the owning
                // city with the game's own GetCityNameByLocation() routine.
                if(!bStartResolved)
                {
                    if(sRouteStart != "")
                    {
                        iRouteColony = FindColony(sRouteStart);

                        if(iRouteColony < 0)
                        {
                            iRouteLocation = FindLocation(sRouteStart);
                            trace("DS RC5.13 ROUTE START LOCATION LOOKUP: token=" +
                                  sRouteStart + " index=" + iRouteLocation);

                            if(iRouteLocation >= 0)
                            {
                                rRouteLocation = &locations[iRouteLocation];
                                sRouteCity = GetCityNameByLocation(rRouteLocation);
                                trace("DS RC5.13 ROUTE START LOCATION CITY: token=" +
                                      sRouteStart + " city=" + sRouteCity);

                                if(sRouteCity != "")
                                {
                                    if(sRouteCity != "none")
                                    {
                                        iRouteColony = FindColony(sRouteCity);
                                    }
                                }
                            }
                        }

                        trace("DS RC5.13 ROUTE START COLONY LOOKUP: token=" +
                              sRouteStart + " index=" + iRouteColony);

                        if(iRouteColony >= 0)
                        {
                            rRouteColony = GetColonyByIndex(iRouteColony);
                            sRouteIsland = rRouteColony.island;
                            sRouteAlias = rRouteColony.id + "_town";

                            trace("DS RC5.13 ROUTE START COLONY DATA: island=" +
                                  sRouteIsland + " townAlias=" + sRouteAlias +
                                  " from_sea=" + rRouteColony.from_sea);

                            // Prefer the canonical <colony>_town world-map
                            // anchor. This is important for colonies whose land
                            // port location is named *_port but whose strategic
                            // anchor remains *_town.
                            bRouteHasIslandAlias = false;
                            if(sRouteIsland != "")
                            {
                                if(sRouteAlias != "")
                                {
                                    bRouteHasIslandAlias = true;
                                }
                            }

                            if(bRouteHasIslandAlias)
                            {
                                sRoutePath = "islands." + sRouteIsland +
                                             "." + sRouteAlias;

                                bRoutePathHasPos = false;
                                if(CheckAttribute(&worldMap, sRoutePath + ".position.x"))
                                {
                                    if(CheckAttribute(&worldMap, sRoutePath + ".position.z"))
                                    {
                                        bRoutePathHasPos = true;
                                    }
                                }

                                if(bRoutePathHasPos)
                                {
                                    fStartX = stf(worldMap.(sRoutePath).position.x);
                                    fStartZ = stf(worldMap.(sRoutePath).position.z);
                                    bStartResolved = true;

                                    trace("DS RC5.13 ROUTE START TOWN PATH MATCH: " +
                                          sRoutePath +
                                          " x=" + fStartX +
                                          " z=" + fStartZ);
                                }
                            }

                            // Non-standard colonies such as FortOrange use a
                            // shore or other from_sea anchor instead.
                            if(!bStartResolved)
                            {
                                sRouteAlias = rRouteColony.from_sea;
                                bRouteHasIslandAlias = false;
                                if(sRouteIsland != "")
                                {
                                    if(sRouteAlias != "")
                                    {
                                        bRouteHasIslandAlias = true;
                                    }
                                }

                                if(bRouteHasIslandAlias)
                                {
                                    sRoutePath = "islands." + sRouteIsland +
                                                 "." + sRouteAlias;

                                    trace("DS RC5.13 ROUTE START FROM_SEA PATH: " + sRoutePath);

                                    bRoutePathHasPos = false;
                                    if(CheckAttribute(&worldMap, sRoutePath + ".position.x"))
                                    {
                                        if(CheckAttribute(&worldMap, sRoutePath + ".position.z"))
                                        {
                                            bRoutePathHasPos = true;
                                        }
                                    }

                                    if(bRoutePathHasPos)
                                    {
                                        fStartX = stf(worldMap.(sRoutePath).position.x);
                                        fStartZ = stf(worldMap.(sRoutePath).position.z);
                                        bStartResolved = true;

                                        trace("DS RC5.13 ROUTE START FROM_SEA MATCH: x=" +
                                              fStartX + " z=" + fStartZ);
                                    }
                                }
                            }

                            if(!bStartResolved)
                            {
                                if(sRouteIsland != "")
                                {
                                    sRoutePath = "islands." + sRouteIsland;

                                    bRoutePathHasPos = false;
                                    if(CheckAttribute(&worldMap, sRoutePath + ".position.x"))
                                    {
                                        if(CheckAttribute(&worldMap, sRoutePath + ".position.z"))
                                        {
                                            bRoutePathHasPos = true;
                                        }
                                    }

                                    if(bRoutePathHasPos)
                                    {
                                        fStartX = stf(worldMap.(sRoutePath).position.x);
                                        fStartZ = stf(worldMap.(sRoutePath).position.z);
                                        bStartResolved = true;

                                        trace("DS RC5.13 ROUTE START ISLAND FALLBACK: " +
                                              sRouteIsland +
                                              " x=" + fStartX +
                                              " z=" + fStartZ);
                                    }
                                }
                            }
                        }
                    }
                }

                if(!bDestResolved)
                {
                    if(sRouteDest != "")
                    {
                        iRouteColony = FindColony(sRouteDest);

                        if(iRouteColony < 0)
                        {
                            iRouteLocation = FindLocation(sRouteDest);
                            trace("DS RC5.13 ROUTE DEST LOCATION LOOKUP: token=" +
                                  sRouteDest + " index=" + iRouteLocation);

                            if(iRouteLocation >= 0)
                            {
                                rRouteLocation = &locations[iRouteLocation];
                                sRouteCity = GetCityNameByLocation(rRouteLocation);
                                trace("DS RC5.13 ROUTE DEST LOCATION CITY: token=" +
                                      sRouteDest + " city=" + sRouteCity);

                                if(sRouteCity != "")
                                {
                                    if(sRouteCity != "none")
                                    {
                                        iRouteColony = FindColony(sRouteCity);
                                    }
                                }
                            }
                        }

                        trace("DS RC5.13 ROUTE DEST COLONY LOOKUP: token=" +
                              sRouteDest + " index=" + iRouteColony);

                        if(iRouteColony >= 0)
                        {
                            rRouteColony = GetColonyByIndex(iRouteColony);
                            sRouteIsland = rRouteColony.island;
                            sRouteAlias = rRouteColony.id + "_town";

                            trace("DS RC5.13 ROUTE DEST COLONY DATA: island=" +
                                  sRouteIsland + " townAlias=" + sRouteAlias +
                                  " from_sea=" + rRouteColony.from_sea);

                            bRouteHasIslandAlias = false;
                            if(sRouteIsland != "")
                            {
                                if(sRouteAlias != "")
                                {
                                    bRouteHasIslandAlias = true;
                                }
                            }

                            if(bRouteHasIslandAlias)
                            {
                                sRoutePath = "islands." + sRouteIsland +
                                             "." + sRouteAlias;

                                bRoutePathHasPos = false;
                                if(CheckAttribute(&worldMap, sRoutePath + ".position.x"))
                                {
                                    if(CheckAttribute(&worldMap, sRoutePath + ".position.z"))
                                    {
                                        bRoutePathHasPos = true;
                                    }
                                }

                                if(bRoutePathHasPos)
                                {
                                    fDestX = stf(worldMap.(sRoutePath).position.x);
                                    fDestZ = stf(worldMap.(sRoutePath).position.z);
                                    bDestResolved = true;

                                    trace("DS RC5.13 ROUTE DEST TOWN PATH MATCH: " +
                                          sRoutePath +
                                          " x=" + fDestX +
                                          " z=" + fDestZ);
                                }
                            }

                            if(!bDestResolved)
                            {
                                sRouteAlias = rRouteColony.from_sea;
                                bRouteHasIslandAlias = false;
                                if(sRouteIsland != "")
                                {
                                    if(sRouteAlias != "")
                                    {
                                        bRouteHasIslandAlias = true;
                                    }
                                }

                                if(bRouteHasIslandAlias)
                                {
                                    sRoutePath = "islands." + sRouteIsland +
                                                 "." + sRouteAlias;

                                    trace("DS RC5.13 ROUTE DEST FROM_SEA PATH: " + sRoutePath);

                                    bRoutePathHasPos = false;
                                    if(CheckAttribute(&worldMap, sRoutePath + ".position.x"))
                                    {
                                        if(CheckAttribute(&worldMap, sRoutePath + ".position.z"))
                                        {
                                            bRoutePathHasPos = true;
                                        }
                                    }

                                    if(bRoutePathHasPos)
                                    {
                                        fDestX = stf(worldMap.(sRoutePath).position.x);
                                        fDestZ = stf(worldMap.(sRoutePath).position.z);
                                        bDestResolved = true;

                                        trace("DS RC5.13 ROUTE DEST FROM_SEA MATCH: x=" +
                                              fDestX + " z=" + fDestZ);
                                    }
                                }
                            }

                            if(!bDestResolved)
                            {
                                if(sRouteIsland != "")
                                {
                                    sRoutePath = "islands." + sRouteIsland;

                                    bRoutePathHasPos = false;
                                    if(CheckAttribute(&worldMap, sRoutePath + ".position.x"))
                                    {
                                        if(CheckAttribute(&worldMap, sRoutePath + ".position.z"))
                                        {
                                            bRoutePathHasPos = true;
                                        }
                                    }

                                    if(bRoutePathHasPos)
                                    {
                                        fDestX = stf(worldMap.(sRoutePath).position.x);
                                        fDestZ = stf(worldMap.(sRoutePath).position.z);
                                        bDestResolved = true;

                                        trace("DS RC5.13 ROUTE DEST ISLAND FALLBACK: " +
                                              sRouteIsland +
                                              " x=" + fDestX +
                                              " z=" + fDestZ);
                                    }
                                }
                            }
                        }
                    }
                }

                trace("DS RC5.13 ROUTE FINAL: chr=" + sChr +
                      " startOK=" + bStartResolved +
                      " start=" + fStartX + "," + fStartZ +
                      " destOK=" + bDestResolved +
                      " dest=" + fDestX + "," + fDestZ);
            }

            // Sharp sometimes uses a zero-length trader route as a
            // quest-state placeholder while he is already at the clue city.
            // Detect that from the resolved positions, but do not generalise
            // the exception to other quests which may deliberately use a
            // same-point trader encounter.
            if(sChr == "Sharp")
            {
                if(bStartResolved)
                {
                    if(bDestResolved)
                    {
                        fRouteDx = fDestX - fStartX;
                        fRouteDz = fDestZ - fStartZ;
                        fRouteDistSq = fRouteDx * fRouteDx + fRouteDz * fRouteDz;
                        if(fRouteDistSq < 0.01) bHideQuestContact = true;
                    }
                }
            }

            bRouteUnresolved = false;
            if(!bStartResolved) bRouteUnresolved = true;
            if(!bDestResolved) bRouteUnresolved = true;

            if(bRouteUnresolved)
            {
                sRouteStart = "<missing>";
                sRouteDest = "<missing>";

                if(CheckAttribute(rQueued, "beginlocator"))
                    sRouteStart = rQueued.beginlocator;
                if(CheckAttribute(rQueued, "endLocator"))
                    sRouteDest = rQueued.endLocator;

                trace("DS QUEST ROUTE UNRESOLVED: chr=" + sChr +
                      " start=" + sRouteStart +
                      " dest=" + sRouteDest +
                      " startOK=" + bStartResolved +
                      " destOK=" + bDestResolved +
                      "; queue retained");

                rQueued.DirectSailRetryDay = iCurrentDay;
                rQueued.DirectSailRetryHour = iCurrentHour;
                continue;
            }
        }
        else
        {
            // Vanilla real-follow encounters are created relative to the
            // player's current world-map position rather than beginLocator.
            fAngle = frnd() * PIm2;
            fRadius = (2500.0 / makefloat(iScale)) + 30.0 + makefloat(rand(50));
            fStartX = fPlayerX + fRadius * sin(fAngle);
            fStartZ = fPlayerZ + fRadius * cos(fAngle);
            bStartResolved = true;
        }

        iMapSlot = -1;
        if(!GenerateMapEncounter_Alone(sChr, &iMapSlot))
        {
            trace("DS QUEST PROMOTE FAILED: GenerateMapEncounter_Alone chr=" +
                  sChr + "; queue retained");
            ReleaseMapEncounters();
            continue;
        }

        rGenerated = GetMapEncounterRef(iMapSlot);

        // Find an unused persistent encounter ID.
        iFree = -1;
        for(j = 0; j < 1000; j++)
        {
            sNewID = "enc_" + j;
            sNewPath = "encounters." + sNewID;
            if(!CheckAttribute(&worldMap, sNewPath))
            {
                iFree = j;
                break;
            }
        }

        if(iFree < 0)
        {
            trace("DS QUEST PROMOTE FAILED: no free persistent ID chr=" +
                  sChr + "; queue retained");
            ReleaseMapEncounters();
            continue;
        }

        // Prefer cloning an existing non-quest physical shell. DirectSail's
        // persistent population normally guarantees one is available.
        sSourceID = "";
        if(CheckAttribute(&worldMap, "encounters"))
        {
            makearef(rLiveRoot, worldMap.encounters);
            iLiveNum = GetAttributesNum(rLiveRoot);
            for(j = 0; j < iLiveNum; j++)
            {
                rLive = GetAttributeN(rLiveRoot, j);
                if(CheckAttribute(rLive, "quest")) continue;
                if(CheckAttribute(rLive, "needDelete")) continue;
                if(!CheckAttribute(rLive, "encdata")) continue;
                if(!CheckAttribute(rLive, "type")) continue;
                if(rLive.type == "Storm") continue;

                sSourceID = GetAttributeName(rLive);
                break;
            }
        }

        sNewID = "enc_" + iFree;
        sNewPath = "encounters." + sNewID;
        worldMap.(sNewPath) = "";
        makearef(rNew, worldMap.(sNewPath));

        if(sSourceID != "")
        {
            makearef(rShellSource, worldMap.encounters.(sSourceID));
            CopyAttributes(rNew, rShellSource);

            DeleteAttribute(rNew, "needDelete");
            DeleteAttribute(rNew, "dirSailEnc");
            DeleteAttribute(rNew, "quest");
            DeleteAttribute(rNew, "encdata");
            DeleteAttribute(rNew, "attacked");
            DeleteAttribute(rNew, "dsGenerated");
            DeleteAttribute(rNew, "dsGeneratedKind");
            DeleteAttribute(rNew, "dsGeneratedSource");
        }
        else
        {
            // Minimal fallback shell, matching the fields DirectSail uses for
            // its own persistent generated contacts.
            rNew.brnAlpha = 1;
            rNew.deleteAlpha = 1;
            rNew.y = 0.0;
            rNew.ax = 0.0;
            rNew.az = 0.0;
            rNew.dltAx = 0.8;
            rNew.dltAz = 1.2;
            rNew.speed = 0.0;
            rNew.turnspd = 0.0;
            rNew.kMaxSpeed = 1.0;
            rNew.rspeed_x = 0.0;
            rNew.rspeed_y = 0.0;
            rNew.rspeed_z = 0.0;
            rNew.slope = 0.0;
        }

        // Never inherit another encounter's route from the cloned shell.
        DeleteAttribute(rNew, "gotoX");
        DeleteAttribute(rNew, "gotoZ");

        rNew.x = fStartX;
        rNew.z = fStartZ;
        rNew.type = sRootType;

        if(sKind == "trader")
        {
            rNew.gotoX = fDestX;
            rNew.gotoZ = fDestZ;
            rNew.ay = GetAngleY(fDestX - fStartX, fDestZ - fStartZ);
        }
        else
        {
            rNew.ay = fAngle;
        }

        // Copy the generated quest encounter data using the game's native
        // world-map helper. GetMapEncounterRef() returns a ref, not an aref;
        // using it as an aref left encdata empty in V2/V3.
        WdmCopyEncounterData(rGenerated, sNewID);

        // Do not consume the queued quest unless the persistent shell contains
        // the fields which DirectSail's encounter scanner and DirSailEnc expect.
        if(!CheckAttribute(rNew, "encdata.GroupName") ||
           !CheckAttribute(rNew, "encdata.CharacterID") ||
           !CheckAttribute(rNew, "encdata.RealEncounterType"))
        {
            trace("DS QUEST PROMOTE FAILED DATA: chr=" + sChr +
                  " id=" + sNewID +
                  " group=" + CheckAttribute(rNew, "encdata.GroupName") +
                  " character=" + CheckAttribute(rNew, "encdata.CharacterID") +
                  " realType=" + CheckAttribute(rNew, "encdata.RealEncounterType") +
                  "; queue retained");

            DeleteAttribute(&worldMap, sNewPath);
            ReleaseMapEncounters();
            continue;
        }

        trace("DS QUEST PROMOTED ENCDATA: chr=" + sChr +
              " id=" + sNewID +
              " group=" + rNew.encdata.GroupName +
              " character=" + rNew.encdata.CharacterID +
              " realType=" + rNew.encdata.RealEncounterType +
              " nation=" + rNew.encdata.Nation);

        if(CheckAttribute(rGenerated, "Nation"))
            rNew.Nation = rGenerated.Nation;

        rNew.quest.event = sQuestEvent;
        rNew.quest.chrID = sChr;
        rNew.dsQuestPromoted = true;
        rNew.dsQuestQueueSlot = sSlot;
        rNew.dsQuestQueueKind = sKind;

        // Preserve the original queue description so a surviving Direct Sail
        // shadow can be returned to the native world-map engine later without
        // creating a second logical quest encounter.
        if(CheckAttribute(rQueued, "beginlocator"))
            rNew.dsQuestBeginLocator = rQueued.beginlocator;
        if(CheckAttribute(rQueued, "endLocator"))
            rNew.dsQuestEndLocator = rQueued.endLocator;
        if(CheckAttribute(rQueued, "XZGoto"))
            rNew.dsQuestXZGoto = rQueued.XZGoto;
        if(CheckAttribute(rQueued, "x1")) rNew.dsQuestX1 = rQueued.x1;
        if(CheckAttribute(rQueued, "z1")) rNew.dsQuestZ1 = rQueued.z1;
        if(CheckAttribute(rQueued, "x2")) rNew.dsQuestX2 = rQueued.x2;
        if(CheckAttribute(rQueued, "z2")) rNew.dsQuestZ2 = rQueued.z2;

        // Sharp can briefly be represented by a zero-length trader route while
        // the quest state says he is already at the current clue city. Keep
        // that placeholder alive for vanilla timeout handling, but do not let
        // Direct Sail turn it into an artificial interception.
        if(bHideQuestContact)
        {
            rNew.dsQuestHidden = true;
            trace("DS QUEST HIDDEN ZERO ROUTE: chr=" + sChr +
                  " id=" + sNewID +
                  " x=" + fStartX +
                  " z=" + fStartZ);
        }

        // Preserve vanilla timeout semantics. A TimeOut of -1 means no timed
        // expiry, so an indefinite quest encounter has no livetime attribute
        // for Direct Sail or vanilla to decrement.
        iTimeOut = 5;
        if(CheckAttribute(rQueued, "TimeOut")) iTimeOut = sti(rQueued.TimeOut);
        rNew.dsQuestTimeOut = iTimeOut;

        fTimeOutSec = -1.0;
        if(iTimeOut >= 0)
        {
            fTimeOutSec = makefloat(iTimeOut);
            if(CheckAttribute(&worldMap, "date.hourPerSec"))
            {
                fDaysPerSec = 24.0 / stf(worldMap.date.hourPerSec);
                fTimeOutSec = fDaysPerSec * makefloat(iTimeOut);
            }
            rNew.livetime = fTimeOutSec;
        }
        else
        {
            DeleteAttribute(rNew, "livetime");
        }

        rNew.sec = MakeInt(GetSecond());
        rNew.min = MakeInt(GetMinute());
        rNew.hour = MakeInt(GetHour());
        rNew.day = GetDataDay();
        rNew.month = GetDataMonth();
        rNew.year = GetDataYear();

        trace("DS QUEST PROMOTED: slot=" + sSlot +
              " chr=" + sChr +
              " kind=" + sKind +
              " id=" + sNewID +
              " type=" + sRootType +
              " label=" + DirectSail_WatchmanQuestLabel(rNew) +
              " x=" + rNew.x +
              " z=" + rNew.z +
              " timeoutSec=" + fTimeOutSec +
              " shell=" + sSourceID);

        if(sKind == "trader")
            trace("DS QUEST ROUTE: chr=" + sChr +
                  " start=" + fStartX + "," + fStartZ +
                  " dest=" + fDestX + "," + fDestZ);

        // Only consume the queue entry after the persistent live object exists.
        DeleteAttribute(&worldMap, sQueuePath);
        iPromoted++;

        ReleaseMapEncounters();
    }

    if(iPromoted > 0)
        trace("DS QUEST PROMOTE SUMMARY: promoted=" + iPromoted);

    return iPromoted;
}

// Age live quest encounters while the player remains outside the native world
// map. Vanilla wdmUpdateAllEncounterLivetime() is an off-map reconciliation
// shortcut: it strips quest.event and calls Map_TraderSucces_quest() directly.
// That bypasses higher-level arrival handlers such as Gold Fleet RouteGoldFleet(),
// SiegeProgress(), and Map_WarriorEnd. Direct Sail therefore advances only the
// lifetime here; an expired encounter is deleted through vanilla
// wdmDeleteLoginEncounter(), which posts the encounter's original quest.event.
void DirectSail_QuestUpdateLifetimeClock()
{
    aref rEncounters;
    aref rEnc;

    ref rQuestGroup;

    bool bFlushDeferred;
    bool bQuestActive;

    int iElapsedHours;
    int i;
    int iNum;
    int iExpired;
    int iDeferred;
    int iQuestGroupIndex;

    float fHourPerSec;
    float fElapsedLife;
    float fRemaining;

    string sID;
    string sChr;
    string sEvent;
    string sQuestGroup;

    if(!CheckAttribute(pchar, "WordMapEncounters_DailyUpdate.control_year"))
    {
        SaveCurrentQuestDateParam("WordMapEncounters_DailyUpdate");
        return;
    }

    bFlushDeferred = false;
    if(CheckAttribute(pchar, "DirectSail.QuestExpiryDeferred"))
        bFlushDeferred = true;

    iElapsedHours = GetQuestPastTimeParam("WordMapEncounters_DailyUpdate");
    if(iElapsedHours < 1)
    {
        // Normally there is nothing to do until another game hour passes.
        // A deferred expiry is the exception: as soon as its tactical quest
        // group is no longer loaded, finish the timeout without waiting for
        // another strategic hour.
        if(!bFlushDeferred) return;
    }

    fElapsedLife = 0.0;
    if(iElapsedHours > 0)
    {
        if(!CheckAttribute(&worldMap, "date.hourPerSec"))
        {
            trace("DS QUEST LIFETIME: missing worldMap.date.hourPerSec; clock reset");
            SaveCurrentQuestDateParam("WordMapEncounters_DailyUpdate");
            return;
        }

        fHourPerSec = stf(worldMap.date.hourPerSec);
        if(fHourPerSec <= 0.0)
        {
            trace("DS QUEST LIFETIME: invalid hourPerSec=" + fHourPerSec + "; clock reset");
            SaveCurrentQuestDateParam("WordMapEncounters_DailyUpdate");
            return;
        }

        fElapsedLife = makefloat(iElapsedHours) / fHourPerSec;
        trace("DS QUEST LIFETIME TICK: elapsedHours=" + iElapsedHours +
              " elapsedLife=" + fElapsedLife);
    }

    iExpired = 0;
    iDeferred = 0;

    if(CheckAttribute(&worldMap, "encounters"))
    {
        makearef(rEncounters, worldMap.encounters);
        iNum = GetAttributesNum(rEncounters);

        // Expiration can delete the current encounter immediately while the
        // native world-map entity is absent, so walk the collection backwards.
        for(i = iNum - 1; i >= 0; i--)
        {
            rEnc = GetAttributeN(rEncounters, i);

            if(!CheckAttribute(rEnc, "quest.chrID")) continue;
            if(!CheckAttribute(rEnc, "livetime")) continue;
            if(CheckAttribute(rEnc, "needDelete")) continue;

            sID = GetAttributeName(rEnc);
            sChr = rEnc.quest.chrID;
            sEvent = "";
            if(CheckAttribute(rEnc, "quest.event")) sEvent = rEnc.quest.event;

            // RC5.9/RC5.10 used vanilla's off-map updater, which removes
            // quest.event from surviving encounters. Repair such existing save
            // state before relying on the native deletion/event path.
            if(sEvent == "")
            {
                if(CheckAttribute(rEnc, "dsQuestQueueKind"))
                {
                    if(rEnc.dsQuestQueueKind == "trader") sEvent = "Map_TraderSucces";
                    if(rEnc.dsQuestQueueKind == "warrior") sEvent = "Map_WarriorEnd";
                    if(rEnc.dsQuestQueueKind == "coolwarrior") sEvent = "Map_WarriorEnd";
                }

                if(sEvent == "")
                {
                    if(CheckAttribute(rEnc, "type"))
                    {
                        if(rEnc.type == "Merchant") sEvent = "Map_TraderSucces";
                        if(rEnc.type == "Follow") sEvent = "Map_WarriorEnd";
                    }
                }

                if(sEvent != "")
                {
                    rEnc.quest.event = sEvent;
                    trace("DS QUEST EVENT RESTORED: chr=" + sChr +
                          " id=" + sID +
                          " event=" + sEvent);
                }
                else
                {
                    trace("DS QUEST EVENT MISSING: chr=" + sChr +
                          " id=" + sID);
                }
            }

            // Keep ageing the encounter while it is tactically active so no
            // elapsed game time is lost.  Only the expiry EVENT is deferred.
            // Sea_LoginGroup marks the real Sea_<CharacterID> quest group with
            // AlreadyLoaded; SeaLogin clears that marker on the next sea load.
            if(iElapsedHours > 0)
            {
                fRemaining = stf(rEnc.livetime) - fElapsedLife;
                rEnc.livetime = fRemaining;
            }
            else
            {
                fRemaining = stf(rEnc.livetime);
            }

            if(fRemaining <= 0.0)
            {
                bQuestActive = false;

                if(bSeaActive)
                {
                    sQuestGroup = "Sea_" + sChr;
                    iQuestGroupIndex = Group_FindGroup(sQuestGroup);

                    if(iQuestGroupIndex >= 0)
                    {
                        rQuestGroup = Group_GetGroupByID(sQuestGroup);
                        if(CheckAttribute(rQuestGroup, "AlreadyLoaded"))
                            bQuestActive = true;
                    }
                }

                if(bQuestActive)
                {
                    if(!CheckAttribute(rEnc, "dsQuestExpiryDeferred"))
                    {
                        trace("DS QUEST EXPIRE DEFERRED: chr=" + sChr +
                              " id=" + sID +
                              " group=" + sQuestGroup +
                              " event=" + sEvent);
                    }

                    rEnc.dsQuestExpiryDeferred = true;
                    iDeferred++;
                }
                else
                {
                    if(CheckAttribute(rEnc, "dsQuestExpiryDeferred"))
                    {
                        trace("DS QUEST EXPIRE RESUME: chr=" + sChr +
                              " id=" + sID +
                              " event=" + sEvent);
                        DeleteAttribute(rEnc, "dsQuestExpiryDeferred");
                    }

                    trace("DS QUEST EXPIRE: chr=" + sChr +
                          " id=" + sID +
                          " event=" + sEvent);
                    wdmDeleteLoginEncounter(sID);
                    iExpired++;
                }
            }
            else
            {
                if(CheckAttribute(rEnc, "dsQuestExpiryDeferred"))
                    DeleteAttribute(rEnc, "dsQuestExpiryDeferred");
            }
        }
    }

    if(iDeferred > 0)
    {
        pchar.DirectSail.QuestExpiryDeferred = true;
    }
    else
    {
        if(CheckAttribute(pchar, "DirectSail.QuestExpiryDeferred"))
            DeleteAttribute(pchar, "DirectSail.QuestExpiryDeferred");
    }

    if(iElapsedHours > 0)
        SaveCurrentQuestDateParam("WordMapEncounters_DailyUpdate");

    if(iExpired > 0)
        trace("DS QUEST EXPIRE SUMMARY: count=" + iExpired);
}

// Convert Direct Sail-created quest shadows back into the ordinary queued form
// immediately before the native world map is created. Shipcounter is left
// untouched: this is the same logical encounter changing representation, not a
// new Map_Create* request. The native WorldMap_AddQuestEncounters handler then
// creates the real moving world-map encounter.
int DirectSail_QuestPrepareWorldMapHandoff()
{
    aref rEncounters;
    aref rEnc;
    aref rQueue;

    int i;
    int iNum;
    int iHanded;
    int iTimeOut;
    int iTimeOutDays;

    float fRemaining;
    float fDaysPerSec;
    float fWholeDaysLife;

    string sID;
    string sPath;
    string sChr;
    string sKind;
    string sEvent;

    iHanded = 0;

    if(!CheckAttribute(&worldMap, "encounters")) return 0;

    makearef(rEncounters, worldMap.encounters);
    iNum = GetAttributesNum(rEncounters);

    for(i = iNum - 1; i >= 0; i--)
    {
        rEnc = GetAttributeN(rEncounters, i);

        if(CheckAttribute(rEnc, "quest.chrID"))
        {
            if(!CheckAttribute(rEnc, "quest.event"))
            {
                sEvent = "";

                if(CheckAttribute(rEnc, "dsQuestQueueKind"))
                {
                    if(rEnc.dsQuestQueueKind == "trader") sEvent = "Map_TraderSucces";
                    if(rEnc.dsQuestQueueKind == "warrior") sEvent = "Map_WarriorEnd";
                    if(rEnc.dsQuestQueueKind == "coolwarrior") sEvent = "Map_WarriorEnd";
                }

                if(sEvent == "")
                {
                    if(CheckAttribute(rEnc, "type"))
                    {
                        if(rEnc.type == "Merchant") sEvent = "Map_TraderSucces";
                        if(rEnc.type == "Follow") sEvent = "Map_WarriorEnd";
                    }
                }

                if(sEvent != "")
                {
                    rEnc.quest.event = sEvent;
                    trace("DS QUEST HANDOFF EVENT RESTORED: chr=" + rEnc.quest.chrID +
                          " id=" + GetAttributeName(rEnc) +
                          " event=" + sEvent);
                }
            }
        }

        if(!CheckAttribute(rEnc, "dsQuestPromoted")) continue;
        if(CheckAttribute(rEnc, "needDelete")) continue;
        if(!CheckAttribute(rEnc, "quest.chrID")) continue;
        if(!CheckAttribute(rEnc, "dsQuestQueueKind")) continue;

        sID = GetAttributeName(rEnc);
        sPath = "encounters." + sID;
        sChr = rEnc.quest.chrID;
        sKind = rEnc.dsQuestQueueKind;

        // Defensive finalisation: a quest may have reached zero lifetime while
        // its Sea_<CharacterID> group was still tactically loaded.  The sea-to-
        // map transition is the correct point to deliver that deferred timeout,
        // not to recreate an already-expired encounter on the native map.
        if(CheckAttribute(rEnc, "livetime"))
        {
            fRemaining = stf(rEnc.livetime);
            if(fRemaining <= 0.0)
            {
                trace("DS QUEST HANDOFF EXPIRE: chr=" + sChr +
                      " id=" + sID);
                wdmDeleteLoginEncounter(sID);
                continue;
            }
        }

        rQueue = wdmCreateNewQuestEncDescription();
        rQueue.type = sKind;
        rQueue.characterID = sChr;

        if(sKind == "trader")
        {
            if(CheckAttribute(rEnc, "dsQuestXZGoto"))
            {
                rQueue.XZGoto = true;
                if(CheckAttribute(rEnc, "dsQuestX1")) rQueue.x1 = rEnc.dsQuestX1;
                if(CheckAttribute(rEnc, "dsQuestZ1")) rQueue.z1 = rEnc.dsQuestZ1;
                if(CheckAttribute(rEnc, "dsQuestX2")) rQueue.x2 = rEnc.dsQuestX2;
                if(CheckAttribute(rEnc, "dsQuestZ2")) rQueue.z2 = rEnc.dsQuestZ2;
            }
            else
            {
                if(CheckAttribute(rEnc, "dsQuestBeginLocator"))
                    rQueue.beginlocator = rEnc.dsQuestBeginLocator;
                if(CheckAttribute(rEnc, "dsQuestEndLocator"))
                    rQueue.endLocator = rEnc.dsQuestEndLocator;

                // RC5.8 migration fallback: older promoted saves did not retain
                // the original route tokens. Preserve the same physical route
                // by handing it to vanilla as an XZ trader instead.
                if(!CheckAttribute(rQueue, "beginlocator"))
                {
                    if(CheckAttribute(rEnc, "x"))
                    {
                        if(CheckAttribute(rEnc, "z"))
                        {
                            if(CheckAttribute(rEnc, "gotoX"))
                            {
                                if(CheckAttribute(rEnc, "gotoZ"))
                                {
                                    rQueue.XZGoto = true;
                                    rQueue.x1 = rEnc.x;
                                    rQueue.z1 = rEnc.z;
                                    rQueue.x2 = rEnc.gotoX;
                                    rQueue.z2 = rEnc.gotoZ;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if(CheckAttribute(rEnc, "dsQuestBeginLocator"))
                rQueue.beginlocator = rEnc.dsQuestBeginLocator;
            else
                rQueue.beginlocator = "";
        }

        iTimeOut = 5;
        if(CheckAttribute(rEnc, "dsQuestTimeOut"))
            iTimeOut = sti(rEnc.dsQuestTimeOut);

        if(iTimeOut < 0)
        {
            rQueue.TimeOut = -1;
        }
        else
        {
            fRemaining = 0.0;
            if(CheckAttribute(rEnc, "livetime"))
                fRemaining = stf(rEnc.livetime);

            fDaysPerSec = 1.0;
            if(CheckAttribute(&worldMap, "date.hourPerSec"))
                fDaysPerSec = 24.0 / stf(worldMap.date.hourPerSec);

            iTimeOutDays = makeint(fRemaining / fDaysPerSec);
            fWholeDaysLife = makefloat(iTimeOutDays) * fDaysPerSec;
            if(fWholeDaysLife < fRemaining) iTimeOutDays++;
            if(iTimeOutDays < 1) iTimeOutDays = 1;

            rQueue.TimeOut = iTimeOutDays;
            rQueue.DirectSailRemainingLife = fRemaining;
        }

        if(CheckAttribute(pchar, "DirectSail.Watchman.Target"))
        {
            if(pchar.DirectSail.Watchman.Target == sID)
                DeleteAttribute(pchar, "DirectSail.Watchman");
        }

        trace("DS QUEST HANDOFF TO WORLD MAP: chr=" + sChr +
              " kind=" + sKind +
              " oldID=" + sID +
              " timeout=" + rQueue.TimeOut);

        DeleteAttribute(&worldMap, sPath);
        iHanded++;
    }

    if(iHanded > 0)
    {
        worldMap.addQuestEncounters = "updateQuest";
        trace("DS QUEST HANDOFF SUMMARY: count=" + iHanded);
    }

    return iHanded;
}

bool DirectSail_WatchmanIsContact(aref rEnc)
{
    int iSpecial;

    if(!CheckAttribute(rEnc, "type")) return false;
    if(rEnc.type == "Storm") return false;
    if(CheckAttribute(rEnc, "needDelete")) return false;
    if(CheckAttribute(rEnc, "dsQuestHidden")) return false;
    if(CheckAttribute(rEnc, "dirSailEnc")) return false;

    // Genuine persistent quest encounters are valid Watchman contacts.
    // Replenishment and cleanup still exclude quests elsewhere.
    if(!CheckAttribute(rEnc, "encdata")) return false;
    if(!CheckAttribute(rEnc, "x") || !CheckAttribute(rEnc, "z")) return false;

    iSpecial = DirectSail_WatchmanSpecialRealType(rEnc);
    if(iSpecial >= 0)
    {
        if(iSpecial == ENCOUNTER_TYPE_BARREL) return true;
        if(iSpecial == ENCOUNTER_TYPE_BOAT) return true;
        return false;
    }

    // Warring + Attacked represent one battle opportunity.
    if(rEnc.type == "Attacked") return false;

    if(rEnc.type == "Merchant") return true;
    if(rEnc.type == "Warring") return true;
    if(rEnc.type == "Follow") return true;

    return false;
}

string DirectSail_WatchmanFindWarringPartnerID(aref rEnc)
{
    aref WME;
    aref rCandidate;

    int i;
    int iNum;
    int iScale;

    float fDx;
    float fDz;
    float fDistSq;    float fBestDistSq;
    float fPairMaxSq;

    string sID;
    string sCandidateID;
    string sBestID;

    if(!DirectSail_WatchmanIsWarring(rEnc)) return "";

    if(!CheckAttribute(&worldMap, "island")) return "";

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0) return "";

    fPairMaxSq = DS_WATCHMAN_WARRING_TACTICAL_RADIUS_SQ /
                 makefloat(iScale * iScale);

    fBestDistSq = fPairMaxSq + 1.0;
    sBestID = "";
    sID = GetAttributeName(rEnc);

    makearef(WME, worldMap.encounters);
    iNum = GetAttributesNum(WME);

    for(i = 0; i < iNum; i++)
    {
        rCandidate = GetAttributeN(WME, i);
        sCandidateID = GetAttributeName(rCandidate);

        if(sCandidateID == sID) continue;
        if(!CheckAttribute(rCandidate, "type")) continue;
        if(rCandidate.type != "Attacked") continue;
        if(CheckAttribute(rCandidate, "needDelete")) continue;
        if(!CheckAttribute(rCandidate, "encdata")) continue;
        if(!CheckAttribute(rCandidate, "x") || !CheckAttribute(rCandidate, "z")) continue;

        fDx = stf(rCandidate.x) - stf(rEnc.x);
        fDz = stf(rCandidate.z) - stf(rEnc.z);
        fDistSq = fDx * fDx + fDz * fDz;

        if(fDistSq > fPairMaxSq) continue;

        if(fDistSq < fBestDistSq)
        {
            fBestDistSq = fDistSq;
            sBestID = sCandidateID;
        }
    }

    return sBestID;
}

string DirectSail_WatchmanTypeText(aref rEnc)
{
    int iType;

    if(DirectSail_WatchmanIsWarring(rEnc))
        return DirectSail_WatchmanText("TypeBattle");

    iType = DirectSail_WatchmanSpecialRealType(rEnc);

    if(iType == ENCOUNTER_TYPE_BARREL)
        return DirectSail_WatchmanText("TypeFlotsam");

    if(iType == ENCOUNTER_TYPE_BOAT)
        return DirectSail_WatchmanText("TypeStrandedVessel");

    if(CheckAttribute(rEnc, "encdata.RealEncounterType"))
    {
        iType = sti(rEnc.encdata.RealEncounterType);

        if(iType <= ENCOUNTER_TYPE_MERCHANT_LARGE)
            return DirectSail_WatchmanText("TypeMerchants");

        if(iType >= ENCOUNTER_TYPE_MERCHANT_GUARD_SMALL &&
           iType <= ENCOUNTER_TYPE_MERCHANT_GUARD_LARGE)
            return DirectSail_WatchmanText("TypeMerchantConvoy");

        if(iType >= ENCOUNTER_TYPE_ESCORT_SMALL &&
           iType <= ENCOUNTER_TYPE_ESCORT_LARGE)
            return DirectSail_WatchmanText("TypeTradeCaravan");

        if(iType >= ENCOUNTER_TYPE_PATROL_SMALL &&
           iType <= ENCOUNTER_TYPE_PATROL_LARGE)
            return DirectSail_WatchmanText("TypePatrol");

        if(iType >= ENCOUNTER_TYPE_PIRATE_SMALL &&
           iType <= ENCOUNTER_TYPE_PIRATE_LARGE)
            return DirectSail_WatchmanText("TypePirates");

        if(iType >= ENCOUNTER_TYPE_SQUADRON &&
           iType <= ENCOUNTER_TYPE_ARMADA)
            return DirectSail_WatchmanText("TypeNavalSquadron");

        if(iType == ENCOUNTER_TYPE_PUNITIVE_SQUADRON)
            return DirectSail_WatchmanText("TypePunitiveExpedition");
    }

    // Direct Sail metadata remains a useful fallback for generated shells.
    if(CheckAttribute(rEnc, "dsGeneratedKind"))
    {
        if(rEnc.dsGeneratedKind == "Merchant")
            return DirectSail_WatchmanText("TypeMerchants");

        if(rEnc.dsGeneratedKind == "Follow")
        {
            if(CheckAttribute(rEnc, "type") && rEnc.type == "Follow")
                return DirectSail_WatchmanText("TypePatrol");

            return DirectSail_WatchmanText("TypeFleet");
        }
    }

    if(CheckAttribute(rEnc, "type") && rEnc.type == "Merchant")
        return DirectSail_WatchmanText("TypeMerchants");

    if(CheckAttribute(rEnc, "type") && rEnc.type == "Follow")
        return DirectSail_WatchmanText("TypePatrol");

    return DirectSail_WatchmanText("TypeFleet");}

string DirectSail_WatchmanContactLabel(aref rEnc)
{
    string sPartnerID;
    string sPartnerPath;
    aref rPartner;

    int iNation1;
    int iNation2;
    int iType;

    string sType;
    string sQuestLabel;

    if(CheckAttribute(rEnc, "quest"))
    {
        sQuestLabel = DirectSail_WatchmanQuestLabel(rEnc);
        if(sQuestLabel != "") return sQuestLabel;
    }

    iType = DirectSail_WatchmanSpecialRealType(rEnc);

    if(iType == ENCOUNTER_TYPE_BARREL)
        return DirectSail_WatchmanText("TypeFlotsam");

    if(iType == ENCOUNTER_TYPE_BOAT)
        return DirectSail_WatchmanText("TypeStrandedVessel");

    if(DirectSail_WatchmanIsWarring(rEnc))
    {
        iNation1 = DirectSail_WatchmanNation(rEnc);
        sPartnerID = DirectSail_WatchmanFindWarringPartnerID(rEnc);

        if(sPartnerID != "")
        {
            sPartnerPath = "encounters." + sPartnerID;
            makearef(rPartner, worldMap.(sPartnerPath));
            iNation2 = DirectSail_WatchmanNation(rPartner);

            return DirectSail_WatchmanBattleNationText(iNation1) + " " +
                   DirectSail_WatchmanText("Versus") + " " +
                   DirectSail_WatchmanBattleNationText(iNation2) + " - " +
                   DirectSail_WatchmanText("TypeBattle");
        }

        return DirectSail_WatchmanBattleNationText(iNation1) + " - " +
               DirectSail_WatchmanText("TypeBattle");
    }

    sType = DirectSail_WatchmanTypeText(rEnc);

    // "Pirate pirates" is redundant; for pirate-type encounters the encounter
    // type already conveys the useful information.
    if(CheckAttribute(rEnc, "encdata.RealEncounterType"))
    {
        iType = sti(rEnc.encdata.RealEncounterType);

        if(iType >= ENCOUNTER_TYPE_PIRATE_SMALL &&
           iType <= ENCOUNTER_TYPE_PIRATE_LARGE)
            return sType;
    }

    return DirectSail_WatchmanNationTextByID(
               DirectSail_WatchmanNation(rEnc)
           ) + " " + sType;
}

int DirectSail_WatchmanContactTexture(aref rEnc)
{
    if(CheckAttribute(rEnc, "quest"))
        return BI_ICONS_TEXTURE_COMMAND;

    if(DirectSail_WatchmanIsWarring(rEnc))
        return BI_ICONS_TEXTURE_COMMAND;

    if(DirectSail_WatchmanIsSpecialContact(rEnc))
        return BI_ICONS_TEXTURE_COMMAND;

    return DS_WATCHMAN_TEXTURE;
}

int DirectSail_WatchmanContactPicNormal(aref rEnc)
{
    if(CheckAttribute(rEnc, "quest"))
        return DS_WATCHMAN_ICON_QUEST;

    if(DirectSail_WatchmanIsWarring(rEnc))
        return DS_WATCHMAN_ICON_BATTLE;

    if(DirectSail_WatchmanIsSpecialContact(rEnc))
        return DS_WATCHMAN_ICON_SPECIAL;

    return DirectSail_WatchmanFlagPicNormal(rEnc);
}

int DirectSail_WatchmanContactPicSelected(aref rEnc)
{
    if(CheckAttribute(rEnc, "quest"))
        return DS_WATCHMAN_ICON_QUEST_SEL;

    if(DirectSail_WatchmanIsWarring(rEnc))
        return DS_WATCHMAN_ICON_BATTLE_SEL;

    if(DirectSail_WatchmanIsSpecialContact(rEnc))
        return DS_WATCHMAN_ICON_SPECIAL_SEL;

    return DirectSail_WatchmanFlagPicSelected(rEnc);
}

void DirectSail_WatchmanBuildDirectionIcons()
{
    DeleteAttribute(&BattleInterface, "UserIcons");

    BattleInterface.UserIcons.ui1.enable = true;
    BattleInterface.UserIcons.ui1.selpic = 96;
    BattleInterface.UserIcons.ui1.pic = 112;
    BattleInterface.UserIcons.ui1.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui1.name = "dir_n";
    BattleInterface.UserIcons.ui1.note = LanguageConvertString(iCommandsFile, "sea_SailN");

    BattleInterface.UserIcons.ui2.enable = true;
    BattleInterface.UserIcons.ui2.selpic = 97;
    BattleInterface.UserIcons.ui2.pic = 113;
    BattleInterface.UserIcons.ui2.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui2.name = "dir_ne";
    BattleInterface.UserIcons.ui2.note = LanguageConvertString(iCommandsFile, "sea_SailNE");

    BattleInterface.UserIcons.ui3.enable = true;
    BattleInterface.UserIcons.ui3.selpic = 98;
    BattleInterface.UserIcons.ui3.pic = 114;
    BattleInterface.UserIcons.ui3.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui3.name = "dir_e";
    BattleInterface.UserIcons.ui3.note = LanguageConvertString(iCommandsFile, "sea_SailE");

    BattleInterface.UserIcons.ui4.enable = true;
    BattleInterface.UserIcons.ui4.selpic = 99;
    BattleInterface.UserIcons.ui4.pic = 115;
    BattleInterface.UserIcons.ui4.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui4.name = "dir_se";
    BattleInterface.UserIcons.ui4.note = LanguageConvertString(iCommandsFile, "sea_SailSE");

    BattleInterface.UserIcons.ui5.enable = true;
    BattleInterface.UserIcons.ui5.selpic = 100;
    BattleInterface.UserIcons.ui5.pic = 116;
    BattleInterface.UserIcons.ui5.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui5.name = "dir_s";
    BattleInterface.UserIcons.ui5.note = LanguageConvertString(iCommandsFile, "sea_SailS");

    BattleInterface.UserIcons.ui6.enable = true;
    BattleInterface.UserIcons.ui6.selpic = 101;
    BattleInterface.UserIcons.ui6.pic = 117;
    BattleInterface.UserIcons.ui6.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui6.name = "dir_sw";
    BattleInterface.UserIcons.ui6.note = LanguageConvertString(iCommandsFile, "sea_SailSW");

    BattleInterface.UserIcons.ui7.enable = true;
    BattleInterface.UserIcons.ui7.selpic = 102;
    BattleInterface.UserIcons.ui7.pic = 118;
    BattleInterface.UserIcons.ui7.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui7.name = "dir_w";
    BattleInterface.UserIcons.ui7.note = LanguageConvertString(iCommandsFile, "sea_SailW");

    BattleInterface.UserIcons.ui8.enable = true;
    BattleInterface.UserIcons.ui8.selpic = 103;
    BattleInterface.UserIcons.ui8.pic = 119;
    BattleInterface.UserIcons.ui8.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui8.name = "dir_nw";
    BattleInterface.UserIcons.ui8.note = LanguageConvertString(iCommandsFile, "sea_SailNW");
}

int DirectSail_WatchmanBuildContacts()
{
    aref WME;
    aref rEnc;
    object aSorted[DS_WATCHMAN_MAX_CONTACTS];

    int i;
    int j;
    int k;
    int iNum;
    int iScale;    int iContactLimit;
    int iContacts;
    int iMenuItems;
    int iPic;
    int iSelPic;
    int iTex;

    float fPlayerX;
    float fPlayerZ;
    float fDx;
    float fDz;
    float fDistSq;

    bool bTracking;

    string sID;
    string sPath;
    string sUI;
    string sNote;

    DeleteAttribute(&BattleInterface, "UserIcons");

    bTracking = CheckAttribute(pchar, "DirectSail.Watchman.Target");

    iContactLimit = DS_WATCHMAN_MAX_CONTACTS;
    if(bTracking) iContactLimit = DS_WATCHMAN_MAX_CONTACTS - 1;

    for(i = 0; i < DS_WATCHMAN_MAX_CONTACTS; i++)
    {
        aSorted[i].id = "";
        aSorted[i].dist = 2500000000.0;
    }

    if(!CheckAttribute(&worldMap, "island") ||
       !CheckAttribute(&worldMap, "zeroX") ||
       !CheckAttribute(&worldMap, "zeroZ") ||
       !CheckAttribute(pchar, "Ship.Pos.x") ||
       !CheckAttribute(pchar, "Ship.Pos.z"))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("UnablePosition")
        );
        return 0;
    }

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0)
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("UnablePosition")
        );
        return 0;
    }

    fPlayerX = stf(worldMap.zeroX) +
               stf(pchar.Ship.Pos.x) / makefloat(iScale);

    fPlayerZ = stf(worldMap.zeroZ) +
               stf(pchar.Ship.Pos.z) / makefloat(iScale);

    if(!CheckAttribute(&worldMap, "encounters"))
    {
        if(!bTracking)
        {
            Log_SetStringToLog(
                DirectSail_WatchmanText("Watchman") + ": " +
                DirectSail_WatchmanText("NothingSpotted")
            );
        }
        return 0;
    }

    makearef(WME, worldMap.encounters);
    iNum = GetAttributesNum(WME);

    // First pass: collect only the nearest eligible contacts.
    for(i = 0; i < iNum; i++)
    {
        rEnc = GetAttributeN(WME, i);

        if(!DirectSail_WatchmanIsContact(rEnc)) continue;

        fDx = stf(rEnc.x) - fPlayerX;
        fDz = stf(rEnc.z) - fPlayerZ;
        fDistSq = fDx * fDx + fDz * fDz;

        if(fDistSq > DS_WATCHMAN_RADIUS_SQ) continue;

        sID = GetAttributeName(rEnc);

        for(j = 0; j < iContactLimit; j++)
        {
            if(aSorted[j].id == "" ||
               fDistSq < stf(aSorted[j].dist))
            {
                for(k = iContactLimit - 1; k > j; k--)
                {
                    aSorted[k].id = aSorted[k - 1].id;
                    aSorted[k].dist = aSorted[k - 1].dist;
                }

                aSorted[j].id = sID;
                aSorted[j].dist = fDistSq;
                break;
            }
        }
    }

    // Second pass: build UserIcons nearest-first.
    iContacts = 0;

    for(i = 0; i < iContactLimit; i++)
    {
        sID = aSorted[i].id;
        if(sID == "") break;

        sPath = "encounters." + sID;
        if(!CheckAttribute(&worldMap, sPath)) continue;

        makearef(rEnc, worldMap.(sPath));

        fDistSq = stf(aSorted[i].dist);
        fDx = stf(rEnc.x) - fPlayerX;
        fDz = stf(rEnc.z) - fPlayerZ;

        sUI = "ui" + (iContacts + 1);

        iPic = DirectSail_WatchmanContactPicNormal(rEnc);
        iSelPic = DirectSail_WatchmanContactPicSelected(rEnc);
        iTex = DirectSail_WatchmanContactTexture(rEnc);

        sNote = DirectSail_WatchmanContactLabel(rEnc) + " - " +
                DirectSail_WatchmanDirection(fDx, fDz) + " - " +
                DirectSail_WatchmanRange(fDistSq);
        BattleInterface.UserIcons.(sUI).enable = true;
        BattleInterface.UserIcons.(sUI).selpic = iSelPic;
        BattleInterface.UserIcons.(sUI).pic = iPic;
        BattleInterface.UserIcons.(sUI).tex = iTex;
        BattleInterface.UserIcons.(sUI).name = sID;
        BattleInterface.UserIcons.(sUI).note = sNote;

        iContacts++;
    }

    iMenuItems = iContacts;

    if(bTracking)
    {
        sUI = "ui" + (iMenuItems + 1);

        BattleInterface.UserIcons.(sUI).enable = true;
        BattleInterface.UserIcons.(sUI).selpic = DS_WATCHMAN_ICON_LOOKOUT_SEL;
        BattleInterface.UserIcons.(sUI).pic = DS_WATCHMAN_ICON_LOOKOUT;
        BattleInterface.UserIcons.(sUI).tex = DS_WATCHMAN_TEXTURE;
        BattleInterface.UserIcons.(sUI).name = "DS_WATCHMAN_STOP";
        BattleInterface.UserIcons.(sUI).note =
            DirectSail_WatchmanText("StopTracking");

        iMenuItems++;
    }

    if(iMenuItems == 0)
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("NothingSpotted")
        );

        DirectSail_WatchmanBuildDirectionIcons();
    }

    return iMenuItems;
}

void DirectSail_WatchmanRestoreTargetTimestamp(aref rEnc)
{
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalSec"))
        rEnc.sec = pchar.DirectSail.Watchman.OriginalSec;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalMin"))
        rEnc.min = pchar.DirectSail.Watchman.OriginalMin;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalHour"))
        rEnc.hour = pchar.DirectSail.Watchman.OriginalHour;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalDay"))
        rEnc.day = pchar.DirectSail.Watchman.OriginalDay;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalMonth"))
        rEnc.month = pchar.DirectSail.Watchman.OriginalMonth;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalYear"))
        rEnc.year = pchar.DirectSail.Watchman.OriginalYear;
}

void DirectSail_WatchmanRestorePartnerTimestamp(aref rEnc)
{
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalSec"))
        rEnc.sec = pchar.DirectSail.Watchman.PartnerOriginalSec;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalMin"))
        rEnc.min = pchar.DirectSail.Watchman.PartnerOriginalMin;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalHour"))
        rEnc.hour = pchar.DirectSail.Watchman.PartnerOriginalHour;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalDay"))
        rEnc.day = pchar.DirectSail.Watchman.PartnerOriginalDay;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalMonth"))
        rEnc.month = pchar.DirectSail.Watchman.PartnerOriginalMonth;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalYear"))
        rEnc.year = pchar.DirectSail.Watchman.PartnerOriginalYear;
}

void DirectSail_WatchmanReleaseProtection()
{
    string sID;
    string sPath;
    string sPartnerID;
    string sPartnerPath;

    aref rEnc;
    aref rPartner;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    sPath = "encounters." + sID;

    if(CheckAttribute(pchar, "DirectSail.Watchman.Protected") &&
       CheckAttribute(&worldMap, sPath))
    {
        makearef(rEnc, worldMap.(sPath));

        if(!CheckAttribute(rEnc, "needDelete"))
            DirectSail_WatchmanRestoreTargetTimestamp(rEnc);
    }

    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerProtected") &&
       CheckAttribute(pchar, "DirectSail.Watchman.PartnerID"))
    {
        sPartnerID = pchar.DirectSail.Watchman.PartnerID;
        sPartnerPath = "encounters." + sPartnerID;

        if(CheckAttribute(&worldMap, sPartnerPath))
        {
            makearef(rPartner, worldMap.(sPartnerPath));

            if(!CheckAttribute(rPartner, "needDelete"))
                DirectSail_WatchmanRestorePartnerTimestamp(rPartner);
        }
    }
}

void DirectSail_WatchmanStoreTargetTimestamp(aref rEnc)
{
    if(CheckAttribute(rEnc, "sec"))
        pchar.DirectSail.Watchman.OriginalSec = rEnc.sec;    if(CheckAttribute(rEnc, "min"))
        pchar.DirectSail.Watchman.OriginalMin = rEnc.min;
    if(CheckAttribute(rEnc, "hour"))
        pchar.DirectSail.Watchman.OriginalHour = rEnc.hour;
    if(CheckAttribute(rEnc, "day"))
        pchar.DirectSail.Watchman.OriginalDay = rEnc.day;
    if(CheckAttribute(rEnc, "month"))
        pchar.DirectSail.Watchman.OriginalMonth = rEnc.month;
    if(CheckAttribute(rEnc, "year"))
        pchar.DirectSail.Watchman.OriginalYear = rEnc.year;

    pchar.DirectSail.Watchman.Protected = true;
}

void DirectSail_WatchmanStorePartnerTimestamp(string sPartnerID, aref rEnc)
{
    pchar.DirectSail.Watchman.PartnerID = sPartnerID;

    if(CheckAttribute(rEnc, "sec"))
        pchar.DirectSail.Watchman.PartnerOriginalSec = rEnc.sec;
    if(CheckAttribute(rEnc, "min"))
        pchar.DirectSail.Watchman.PartnerOriginalMin = rEnc.min;
    if(CheckAttribute(rEnc, "hour"))
        pchar.DirectSail.Watchman.PartnerOriginalHour = rEnc.hour;
    if(CheckAttribute(rEnc, "day"))
        pchar.DirectSail.Watchman.PartnerOriginalDay = rEnc.day;
    if(CheckAttribute(rEnc, "month"))
        pchar.DirectSail.Watchman.PartnerOriginalMonth = rEnc.month;
    if(CheckAttribute(rEnc, "year"))
        pchar.DirectSail.Watchman.PartnerOriginalYear = rEnc.year;

    pchar.DirectSail.Watchman.PartnerProtected = true;
}

void DirectSail_WatchmanRefreshEncounterTimestamp(aref rEnc)
{
    rEnc.sec = MakeInt(GetSecond());
    rEnc.min = MakeInt(GetMinute());
    rEnc.hour = MakeInt(GetHour());
    rEnc.day = GetDataDay();
    rEnc.month = GetDataMonth();
    rEnc.year = GetDataYear();
}

void DirectSail_WatchmanRefreshProtection()
{
    string sID;
    string sPath;
    string sPartnerID;
    string sPartnerPath;

    aref rEnc;
    aref rPartner;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    sPath = "encounters." + sID;

    if(!CheckAttribute(&worldMap, sPath)) return;

    makearef(rEnc, worldMap.(sPath));

    if(CheckAttribute(rEnc, "dsGenerated") &&
       !CheckAttribute(rEnc, "needDelete"))
    {
        if(!CheckAttribute(pchar, "DirectSail.Watchman.Protected"))
            DirectSail_WatchmanStoreTargetTimestamp(rEnc);

        DirectSail_WatchmanRefreshEncounterTimestamp(rEnc);
    }

    // A generated Warring encounter needs its paired Attacked half preserved
    // as well; otherwise the target could survive cleanup while its opponent
    // expires before interception.
    if(DirectSail_WatchmanIsWarring(rEnc))
    {
        sPartnerID = DirectSail_WatchmanFindWarringPartnerID(rEnc);

        if(sPartnerID != "")
        {
            sPartnerPath = "encounters." + sPartnerID;

            if(CheckAttribute(&worldMap, sPartnerPath))
            {
                makearef(rPartner, worldMap.(sPartnerPath));

                if(CheckAttribute(rPartner, "dsGenerated") &&
                   !CheckAttribute(rPartner, "needDelete"))
                {
                    if(!CheckAttribute(pchar, "DirectSail.Watchman.PartnerProtected"))
                        DirectSail_WatchmanStorePartnerTimestamp(sPartnerID, rPartner);

                    DirectSail_WatchmanRefreshEncounterTimestamp(rPartner);
                }
            }
        }
    }
}

void DirectSail_WatchmanStopTracking(bool bNotify)
{
    if(!CheckAttribute(pchar, "DirectSail.Watchman")) return;

    DirectSail_WatchmanReleaseProtection();

    if(bNotify)
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("TrackingStopped")
        );
    }

    trace("DS WATCHMAN STOP");
    DeleteAttribute(pchar, "DirectSail.Watchman");
}

void DirectSail_WatchmanPrepareLoad()
{
    // Clear the currently-running game's transient Direct Sail state before
    // LoadEngineState(). This prevents attributes absent from the selected
    // save leaking forward into the newly-loaded runtime.
    DirectSail_WatchmanStopTracking(false);
    if(CheckAttribute(pchar, "DirectSail.QuestExpiryDeferred"))
        DeleteAttribute(pchar, "DirectSail.QuestExpiryDeferred");

    trace("DS WATCHMAN LOAD PREPARE");
}

void DirectSail_WatchmanFinishLoad()
{
    // If the loaded save itself contained an active Watchman target, clear it
    // as transient UI state as well. StopTracking restores any protected
    // random-encounter timestamp before deleting the Watchman attributes.
    DirectSail_WatchmanStopTracking(false);

    // The quest-lifetime timestamp is runtime bookkeeping as well. Reset it to
    // the loaded save's current game time so loading an older save cannot make
    // Direct Sail interpret the previous runtime's date as hundreds of elapsed
    // quest hours.
    SaveCurrentQuestDateParam("WordMapEncounters_DailyUpdate");

    trace("DS WATCHMAN LOAD RESET");
    trace("DS QUEST LIFETIME LOAD RESET");
}

void DirectSail_WatchmanQuestEncounterStarted(string sEncounterID)
{
    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;
    if(pchar.DirectSail.Watchman.Target != sEncounterID) return;

    trace("DS WATCHMAN QUEST ENCOUNTER START: id=" + sEncounterID);
    DirectSail_WatchmanStopTracking(false);
}

void DirectSail_WatchmanCompleteTracking(string sReason)
{    if(!CheckAttribute(pchar, "DirectSail.Watchman")) return;

    // Successful interception consumes the target (and both halves of a
    // Warring pair), so protected timestamps are intentionally not restored.
    trace("DS WATCHMAN COMPLETE: reason=" + sReason);
    DeleteAttribute(pchar, "DirectSail.Watchman");
}

void DirectSail_WatchmanReportTarget(bool bInitial)
{
    string sID;
    string sPath;
    string sPrefix;
    string sLabel;
    string sDirection;
    string sRange;

    aref rEnc;

    int iScale;

    float fPlayerX;
    float fPlayerZ;
    float fDx;
    float fDz;
    float fDistSq;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    if(sID == "") return;

    sPath = "encounters." + sID;

    if(!CheckAttribute(&worldMap, sPath))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("ContactLost")
        );

        trace("DS WATCHMAN LOST: id=" + sID + " reason=missing");
        DirectSail_WatchmanStopTracking(false);
        return;
    }

    makearef(rEnc, worldMap.(sPath));

    if(CheckAttribute(rEnc, "dsQuestHidden"))
    {
        trace("DS WATCHMAN LOST: id=" + sID + " reason=hidden-quest");
        DirectSail_WatchmanStopTracking(false);
        return;
    }

    if(CheckAttribute(rEnc, "needDelete"))
    {
        DirectSail_WatchmanCompleteTracking("needDelete");
        return;
    }

    if(!CheckAttribute(rEnc, "x") || !CheckAttribute(rEnc, "z"))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("ContactLost")
        );

        trace("DS WATCHMAN LOST: id=" + sID + " reason=no-position");
        DirectSail_WatchmanStopTracking(false);
        return;
    }

    if(!CheckAttribute(&worldMap, "island") ||
       !CheckAttribute(&worldMap, "zeroX") ||
       !CheckAttribute(&worldMap, "zeroZ") ||
       !CheckAttribute(pchar, "Ship.Pos.x") ||
       !CheckAttribute(pchar, "Ship.Pos.z"))
    {
        return;
    }

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0) return;

    fPlayerX = stf(worldMap.zeroX) +
               stf(pchar.Ship.Pos.x) / makefloat(iScale);

    fPlayerZ = stf(worldMap.zeroZ) +
               stf(pchar.Ship.Pos.z) / makefloat(iScale);

    fDx = stf(rEnc.x) - fPlayerX;
    fDz = stf(rEnc.z) - fPlayerZ;
    fDistSq = fDx * fDx + fDz * fDz;

    sLabel = DirectSail_WatchmanContactLabel(rEnc);
    sDirection = DirectSail_WatchmanDirection(fDx, fDz);
    sRange = DirectSail_WatchmanRange(fDistSq);

    sPrefix = DirectSail_WatchmanText("Watchman") + ": ";
    if(bInitial)
    {
        sPrefix = DirectSail_WatchmanText("Watchman") + ": " +
                  DirectSail_WatchmanText("Tracking") + " ";
    }

    Log_SetStringToLog(
        sPrefix + sLabel + " - " + sDirection + " - " + sRange + "."
    );

    DirectSail_WatchmanRefreshProtection();
}

void DirectSail_WatchmanSelect(string sID)
{
    string sPath;    aref rEnc;

    if(sID == "")
    {
        DirectSail_WatchmanBuildDirectionIcons();
        return;
    }

    if(sID == "DS_WATCHMAN_STOP")
    {
        DirectSail_WatchmanStopTracking(true);
        DirectSail_WatchmanBuildDirectionIcons();
        return;
    }

    sPath = "encounters." + sID;

    if(!CheckAttribute(&worldMap, sPath))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("ContactLost")
        );

        trace("DS WATCHMAN SELECT: missing id=" + sID);
        DirectSail_WatchmanBuildDirectionIcons();
        return;
    }

    if(CheckAttribute(pchar, "DirectSail.Watchman.Target"))
        DirectSail_WatchmanStopTracking(false);

    pchar.DirectSail.Watchman.Target = sID;
    pchar.DirectSail.Watchman.LastDay = GetDataDay();
    pchar.DirectSail.Watchman.LastHour = MakeInt(GetHour());

    makearef(rEnc, worldMap.(sPath));

    trace("DS WATCHMAN TRACK: id=" + sID +
          " label=" + DirectSail_WatchmanContactLabel(rEnc));

    DirectSail_WatchmanRefreshProtection();
    DirectSail_WatchmanReportTarget(true);
    DirectSail_WatchmanBuildDirectionIcons();
}

void DirectSail_WatchmanHourlyUpdate()
{
    // First age encounters which already existed during the elapsed game time.
    // Then materialise any newly-created quest queue entries so they start with
    // their full timeout rather than inheriting time from before creation.
    DirectSail_QuestUpdateLifetimeClock();
    DirectSail_QuestPromoteQueuedEncounters();

    string sID;
    string sPath;

    aref rEnc;

    int iDay;
    int iHour;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    sPath = "encounters." + sID;

    // needDelete is set when the persistent encounter has successfully become
    // the tactical encounter in front of the player. Clear silently.
    if(CheckAttribute(&worldMap, sPath))
    {
        makearef(rEnc, worldMap.(sPath));

        if(CheckAttribute(rEnc, "needDelete"))
        {
            DirectSail_WatchmanCompleteTracking("encountered");
            return;
        }
    }

    iDay = GetDataDay();
    iHour = MakeInt(GetHour());

    if(!CheckAttribute(pchar, "DirectSail.Watchman.LastDay") ||
       !CheckAttribute(pchar, "DirectSail.Watchman.LastHour"))
    {
        pchar.DirectSail.Watchman.LastDay = iDay;
        pchar.DirectSail.Watchman.LastHour = iHour;
        DirectSail_WatchmanRefreshProtection();
        return;
    }

    if(sti(pchar.DirectSail.Watchman.LastDay) == iDay &&
       sti(pchar.DirectSail.Watchman.LastHour) == iHour)
    {
        return;
    }

    pchar.DirectSail.Watchman.LastDay = iDay;
    pchar.DirectSail.Watchman.LastHour = iHour;

    DirectSail_WatchmanReportTarget(false);
}
