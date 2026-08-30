// ============================================================================
// Direct Sail - persistent quest encounter diagnostics
//
// Compile-log diagnostics intended for normal playtesting.  Newly-seen queue
// entries are reported immediately, and a complete queue/live snapshot is
// written once per game hour while the sea battle interface is active.
// ============================================================================

string DirectSail_QuestDiagCharacterLabel(string sChrID)
{
    int nEncChar;

    if(sChrID == "") return "unknown";

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

string DirectSail_QuestDiagNearestAnchor(float fX, float fZ)
{
    aref rIslands;
    aref rIsland;
    aref rAnchor;

    int i;
    int j;

    float fDx;
    float fDz;
    float fDistSq;
    float fBestDistSq;

    string sIsland;
    string sAnchor;
    string sBest;

    sBest = "unknown";
    fBestDistSq = 2500000000.0;

    if(!CheckAttribute(&worldMap, "islands")) return sBest;

    makearef(rIslands, worldMap.islands);

    for(i = 0; i < GetAttributesNum(rIslands); i++)
    {
        rIsland = GetAttributeN(rIslands, i);
        sIsland = GetAttributeName(rIsland);

        if(CheckAttribute(rIsland, "position.x") &&
           CheckAttribute(rIsland, "position.z"))
        {
            fDx = fX - stf(rIsland.position.x);
            fDz = fZ - stf(rIsland.position.z);
            fDistSq = fDx * fDx + fDz * fDz;

            if(fDistSq < fBestDistSq)
            {
                fBestDistSq = fDistSq;
                sBest = sIsland;
            }
        }

        for(j = 0; j < GetAttributesNum(rIsland); j++)
        {
            rAnchor = GetAttributeN(rIsland, j);

            if(!CheckAttribute(rAnchor, "position.x") ||
               !CheckAttribute(rAnchor, "position.z"))
                continue;

            fDx = fX - stf(rAnchor.position.x);
            fDz = fZ - stf(rAnchor.position.z);
            fDistSq = fDx * fDx + fDz * fDz;

            if(fDistSq < fBestDistSq)
            {
                fBestDistSq = fDistSq;
                sAnchor = GetAttributeName(rAnchor);
                sBest = sIsland + "/" + sAnchor;
            }
        }
    }

    return sBest;
}

string DirectSail_QuestDiagQueueRoute(aref rQueued)
{
    string sRoute;

    sRoute = "none";

    if(CheckAttribute(rQueued, "XZGoto") &&
       CheckAttribute(rQueued, "x1") &&
       CheckAttribute(rQueued, "z1") &&
       CheckAttribute(rQueued, "x2") &&
       CheckAttribute(rQueued, "z2"))
    {
        return "XZ " + rQueued.x1 + "," + rQueued.z1 +
               " -> " + rQueued.x2 + "," + rQueued.z2;
    }

    if(CheckAttribute(rQueued, "beginlocator"))
        sRoute = rQueued.beginlocator;

    if(CheckAttribute(rQueued, "endLocator"))
        sRoute = sRoute + " -> " + rQueued.endLocator;

    return sRoute;
}

void DirectSail_QuestDiagLogQueueEntry(aref rQueued, string sPrefix)
{
    string sSlot;
    string sChr;
    string sKind;
    string sTimeout;
    string sRetry;

    sSlot = GetAttributeName(rQueued);
    sChr = "unknown";
    sKind = "unknown";
    sTimeout = "default";
    sRetry = "none";

    if(CheckAttribute(rQueued, "characterID")) sChr = rQueued.characterID;
    if(CheckAttribute(rQueued, "type")) sKind = rQueued.type;
    if(CheckAttribute(rQueued, "TimeOut")) sTimeout = rQueued.TimeOut;

    if(CheckAttribute(rQueued, "DirectSailRetryDay") &&
       CheckAttribute(rQueued, "DirectSailRetryHour"))
    {
        sRetry = rQueued.DirectSailRetryDay + ":" +
                 rQueued.DirectSailRetryHour;
    }

    trace(sPrefix +
          ": slot=" + sSlot +
          " chr=" + sChr +
          " label=" + DirectSail_QuestDiagCharacterLabel(sChr) +
          " kind=" + sKind +
          " route=" + DirectSail_QuestDiagQueueRoute(rQueued) +
          " timeoutDays=" + sTimeout +
          " retryDayHour=" + sRetry);
}

void DirectSail_QuestDiagLogNewQueueEntries()
{
    aref rQueuedRoot;
    aref rQueued;

    int i;

    if(!CheckAttribute(&worldMap, "addQuestEncounters")) return;

    makearef(rQueuedRoot, worldMap.addQuestEncounters);

    for(i = 0; i < GetAttributesNum(rQueuedRoot); i++)
    {
        rQueued = GetAttributeN(rQueuedRoot, i);

        if(!CheckAttribute(rQueued, "characterID")) continue;
        if(!CheckAttribute(rQueued, "type")) continue;
        if(CheckAttribute(rQueued, "DirectSailDiagSeen")) continue;

        DirectSail_QuestDiagLogQueueEntry(rQueued, "DS QUEST QUEUE NEW");
        rQueued.DirectSailDiagSeen = true;
    }
}

int DirectSail_QuestDiagLogQueueSnapshot()
{
    aref rQueuedRoot;
    aref rQueued;

    int i;
    int iCount;

    iCount = 0;

    if(!CheckAttribute(&worldMap, "addQuestEncounters"))
    {
        trace("DS QUEST DIAG QUEUE: count=0");
        return 0;
    }

    makearef(rQueuedRoot, worldMap.addQuestEncounters);

    for(i = 0; i < GetAttributesNum(rQueuedRoot); i++)
    {
        rQueued = GetAttributeN(rQueuedRoot, i);

        if(!CheckAttribute(rQueued, "characterID")) continue;
        if(!CheckAttribute(rQueued, "type")) continue;

        DirectSail_QuestDiagLogQueueEntry(rQueued, "DS QUEST DIAG QUEUED");
        iCount++;
    }

    trace("DS QUEST DIAG QUEUE: count=" + iCount);
    return iCount;
}

int DirectSail_QuestDiagLogLiveSnapshot(float fPlayerX, float fPlayerZ, bool bHavePlayerPos)
{
    aref rEncounters;
    aref rEnc;

    int i;
    int iCount;

    float fDx;
    float fDz;
    float fDistSq;

    string sID;
    string sChr;
    string sType;
    string sEvent;
    string sKind;
    string sLife;
    string sPos;
    string sGoto;
    string sNearest;
    string sRelative;
    string sState;

    iCount = 0;

    if(!CheckAttribute(&worldMap, "encounters"))
    {
        trace("DS QUEST DIAG LIVE: count=0");
        return 0;
    }

    makearef(rEncounters, worldMap.encounters);

    for(i = 0; i < GetAttributesNum(rEncounters); i++)
    {
        rEnc = GetAttributeN(rEncounters, i);

        sChr = "";
        if(CheckAttribute(rEnc, "quest.chrID"))
            sChr = rEnc.quest.chrID;
        else
        {
            if(CheckAttribute(rEnc, "quest") &&
               CheckAttribute(rEnc, "encdata.CharacterID"))
                sChr = rEnc.encdata.CharacterID;
        }

        if(sChr == "") continue;

        sID = GetAttributeName(rEnc);
        sType = "unknown";
        sEvent = "none";
        sKind = "unknown";
        sLife = "unknown";
        sPos = "unknown";
        sGoto = "none";
        sNearest = "unknown";
        sRelative = "unknown";
        sState = "live";

        if(CheckAttribute(rEnc, "type")) sType = rEnc.type;
        if(CheckAttribute(rEnc, "quest.event")) sEvent = rEnc.quest.event;
        if(CheckAttribute(rEnc, "dsQuestQueueKind")) sKind = rEnc.dsQuestQueueKind;
        if(CheckAttribute(rEnc, "livetime")) sLife = rEnc.livetime;

        if(CheckAttribute(rEnc, "x") && CheckAttribute(rEnc, "z"))
        {
            sPos = rEnc.x + "," + rEnc.z;
            sNearest = DirectSail_QuestDiagNearestAnchor(stf(rEnc.x), stf(rEnc.z));

            if(bHavePlayerPos)
            {
                fDx = stf(rEnc.x) - fPlayerX;
                fDz = stf(rEnc.z) - fPlayerZ;
                fDistSq = fDx * fDx + fDz * fDz;

                sRelative = DirectSail_WatchmanDirection(fDx, fDz) +
                            " distSq=" + fDistSq;
            }
        }

        if(CheckAttribute(rEnc, "gotoX") && CheckAttribute(rEnc, "gotoZ"))
            sGoto = rEnc.gotoX + "," + rEnc.gotoZ;

        if(CheckAttribute(rEnc, "dsQuestPromoted"))
            sState = sState + ",promoted";
        if(CheckAttribute(rEnc, "dsQuestHidden"))
            sState = sState + ",hidden";
        if(CheckAttribute(rEnc, "needDelete"))
            sState = sState + ",needDelete";
        if(CheckAttribute(rEnc, "dsQuestExpiryDeferred"))
            sState = sState + ",expiryDeferred";

        trace("DS QUEST DIAG LIVE ENTRY" +
              ": id=" + sID +
              " chr=" + sChr +
              " label=" + DirectSail_QuestDiagCharacterLabel(sChr) +
              " type=" + sType +
              " kind=" + sKind +
              " event=" + sEvent +
              " pos=" + sPos +
              " nearest=" + sNearest +
              " relative=" + sRelative +
              " goto=" + sGoto +
              " lifeSec=" + sLife +
              " state=" + sState);

        iCount++;
    }

    trace("DS QUEST DIAG LIVE: count=" + iCount);
    return iCount;
}

void DirectSail_QuestDiagnosticsUpdate()
{
    int iDay;
    int iHour;
    int iScale;
    int iQueued;
    int iLive;

    float fPlayerX;
    float fPlayerZ;

    bool bHavePlayerPos;

    string sZone;

    // Do this every frame so a queue entry which is created and immediately
    // promoted during the same game hour is still captured in compile.log.
    DirectSail_QuestDiagLogNewQueueEntries();

    iDay = GetDataDay();
    iHour = MakeInt(GetHour());

    if(CheckAttribute(pchar, "DirectSail.QuestDiagnostics.LastDay") &&
       CheckAttribute(pchar, "DirectSail.QuestDiagnostics.LastHour"))
    {
        if(sti(pchar.DirectSail.QuestDiagnostics.LastDay) == iDay &&
           sti(pchar.DirectSail.QuestDiagnostics.LastHour) == iHour)
        {
            return;
        }
    }

    pchar.DirectSail.QuestDiagnostics.LastDay = iDay;
    pchar.DirectSail.QuestDiagnostics.LastHour = iHour;

    bHavePlayerPos = false;
    fPlayerX = 0.0;
    fPlayerZ = 0.0;
    sZone = "unknown";

    if(CheckAttribute(&worldMap, "island")) sZone = worldMap.island;

    if(CheckAttribute(&worldMap, "island") &&
       CheckAttribute(&worldMap, "zeroX") &&
       CheckAttribute(&worldMap, "zeroZ") &&
       CheckAttribute(pchar, "Ship.Pos.x") &&
       CheckAttribute(pchar, "Ship.Pos.z"))
    {
        iScale = DirectSail_GetIslandScale(worldMap.island);
        if(iScale > 0)
        {
            fPlayerX = stf(worldMap.zeroX) +
                       stf(pchar.Ship.Pos.x) / makefloat(iScale);
            fPlayerZ = stf(worldMap.zeroZ) +
                       stf(pchar.Ship.Pos.z) / makefloat(iScale);
            bHavePlayerPos = true;
        }
    }

    trace("DS QUEST DIAG ===== SNAPSHOT day=" + iDay +
          " hour=" + iHour +
          " zone=" + sZone +
          " playerWorld=" + fPlayerX + "," + fPlayerZ +
          " havePlayerPos=" + bHavePlayerPos + " =====");

    iQueued = DirectSail_QuestDiagLogQueueSnapshot();
    iLive = DirectSail_QuestDiagLogLiveSnapshot(fPlayerX, fPlayerZ, bHavePlayerPos);

    trace("DS QUEST DIAG ===== END queued=" + iQueued +
          " live=" + iLive + " =====");
}
