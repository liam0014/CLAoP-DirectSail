// ============================================================================
// Direct Sail - automatic Watchman alerts for quest contacts
//
// Notifies the player once when a quest encounter crosses into Watchman range.
// The marker is cleared after it leaves range, allowing a later re-entry alert.
// ============================================================================

void DirectSail_WatchmanQuestRangeUpdate()
{
    aref WME;
    aref rEnc;

    int i;
    int iNum;
    int iScale;

    float fPlayerX;
    float fPlayerZ;
    float fDx;
    float fDz;
    float fDistSq;

    bool bInRange;

    string sLabel;
    string sID;

    if(!CheckAttribute(&worldMap, "island") ||
       !CheckAttribute(&worldMap, "zeroX") ||
       !CheckAttribute(&worldMap, "zeroZ") ||
       !CheckAttribute(pchar, "Ship.Pos.x") ||
       !CheckAttribute(pchar, "Ship.Pos.z"))
    {
        return;
    }

    if(!CheckAttribute(&worldMap, "encounters")) return;

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0) return;

    fPlayerX = stf(worldMap.zeroX) +
               stf(pchar.Ship.Pos.x) / makefloat(iScale);

    fPlayerZ = stf(worldMap.zeroZ) +
               stf(pchar.Ship.Pos.z) / makefloat(iScale);

    makearef(WME, worldMap.encounters);
    iNum = GetAttributesNum(WME);

    for(i = 0; i < iNum; i++)
    {
        rEnc = GetAttributeN(WME, i);

        // Match the Watchman's own quest-contact classification.
        if(!CheckAttribute(rEnc, "quest")) continue;

        bInRange = false;

        if(DirectSail_WatchmanIsContact(rEnc) &&
           CheckAttribute(rEnc, "x") &&
           CheckAttribute(rEnc, "z"))
        {
            fDx = stf(rEnc.x) - fPlayerX;
            fDz = stf(rEnc.z) - fPlayerZ;
            fDistSq = fDx * fDx + fDz * fDz;

            if(fDistSq <= DS_WATCHMAN_RADIUS_SQ)
                bInRange = true;
        }

        if(!bInRange)
        {
            if(CheckAttribute(rEnc, "dsWatchmanQuestInRange"))
                DeleteAttribute(rEnc, "dsWatchmanQuestInRange");

            continue;
        }

        if(CheckAttribute(rEnc, "dsWatchmanQuestInRange")) continue;

        rEnc.dsWatchmanQuestInRange = true;

        sID = GetAttributeName(rEnc);
        sLabel = DirectSail_WatchmanContactLabel(rEnc);

        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("QuestShipSpotted") + " " +
            sLabel + " - " +
            DirectSail_WatchmanDirection(fDx, fDz) + " - " +
            DirectSail_WatchmanRange(fDistSq) + "."
        );

        trace("DS WATCHMAN QUEST SPOTTED: id=" + sID +
              " label=" + sLabel +
              " distSq=" + fDistSq);
    }
}
