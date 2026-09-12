// ============================================================================
// Direct Sail - persistent quest encounter diagnostics
//
// Release logging records meaningful quest-encounter state changes without
// writing a full queue/live snapshot every game hour.
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

void DirectSail_QuestDiagnosticsUpdate()
{
    // Run every frame so a queue entry which is created and immediately
    // promoted is still captured in compile.log. Only newly-seen entries are
    // logged; routine hourly queue/live snapshots are deliberately omitted.
    DirectSail_QuestDiagLogNewQueueEntries();
}
