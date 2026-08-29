// ============================================================================
// Direct Sail - automatic Watchman alerts for quest contacts
//
// Notifies the player immediately when a quest encounter crosses into Watchman
// range, then once per game hour while it remains in range. Leaving range clears
// the marker so a later re-entry produces a fresh immediate alert.
// ============================================================================

void DirectSail_WatchmanQuestRangeUpdate()
{
    aref WME;
    aref rEnc;

    int i;
    int iNum;
    int iScale;
    int iDay;
    int iHour;

    float fPlayerX;
    float fPlayerZ;
    float fDx;
    float fDz;
    float fDistSq;

    bool bInRange;
    bool bShouldAlert;

    string sLabel;
    string sID;
    string sReason;

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

    iDay = GetDataDay();
    iHour = MakeInt(GetHour());

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
            if(CheckAttribute(rEnc, "dsWatchmanQuestLastAlertDay"))
                DeleteAttribute(rEnc, "dsWatchmanQuestLastAlertDay");
            if(CheckAttribute(rEnc, "dsWatchmanQuestLastAlertHour"))
                DeleteAttribute(rEnc, "dsWatchmanQuestLastAlertHour");

            continue;
        }

        bShouldAlert = false;
        sReason = "";

        // First frame after the contact enters Watchman range.
        if(!CheckAttribute(rEnc, "dsWatchmanQuestInRange"))
        {
            rEnc.dsWatchmanQuestInRange = true;
            bShouldAlert = true;
            sReason = "entry";
        }
        else
        {
            // Repeat once when the game clock enters a new hour while the
            // quest contact remains inside Watchman range.
            if(!CheckAttribute(rEnc, "dsWatchmanQuestLastAlertDay") ||
               !CheckAttribute(rEnc, "dsWatchmanQuestLastAlertHour"))
            {
                bShouldAlert = true;
                sReason = "hourly-missing-timestamp";
            }
            else
            {
                if(sti(rEnc.dsWatchmanQuestLastAlertDay) != iDay ||
                   sti(rEnc.dsWatchmanQuestLastAlertHour) != iHour)
                {
                    bShouldAlert = true;
                    sReason = "hourly";
                }
            }
        }

        if(!bShouldAlert) continue;

        rEnc.dsWatchmanQuestLastAlertDay = iDay;
        rEnc.dsWatchmanQuestLastAlertHour = iHour;

        sID = GetAttributeName(rEnc);
        sLabel = DirectSail_WatchmanContactLabel(rEnc);

        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("QuestShipSpotted") + " " +
            sLabel + " - " +
            DirectSail_WatchmanDirection(fDx, fDz) + " - " +
            DirectSail_WatchmanRange(fDistSq) + "."
        );

        trace("DS WATCHMAN QUEST SPOTTED: reason=" + sReason +
              " id=" + sID +
              " label=" + sLabel +
              " day=" + iDay +
              " hour=" + iHour +
              " distSq=" + fDistSq);
    }
}
