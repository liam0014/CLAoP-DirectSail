// ============================================================================
// Direct Sail - Arrow / MushketCap lifecycle diagnostics
//
// Adds an hourly status line for the roaming Arrow quest ship even while it is
// off-map in its port phase.  This complements the generic queue/live snapshot.
// ============================================================================

void DirectSail_ArrowStatusDiagnosticsUpdate()
{
    int iDay;
    int iHour;
    int iChr;

    ref rArrow;

    string sState;
    string sCity;
    string sCityShore;
    string sTargetCity;
    string sTargetShore;
    string sShipName;
    string sMapName;
    string sQuestTempShore;
    string sTimer;
    string sTimerTarget;
    string sTimerFunction;
    string sCapCheckTime;
    string sCapCheckSince;

    iDay = GetDataDay();
    iHour = MakeInt(GetHour());

    if(CheckAttribute(pchar, "DirectSail.ArrowDiagnostics.LastDay") &&
       CheckAttribute(pchar, "DirectSail.ArrowDiagnostics.LastHour"))
    {
        if(sti(pchar.DirectSail.ArrowDiagnostics.LastDay) == iDay &&
           sti(pchar.DirectSail.ArrowDiagnostics.LastHour) == iHour)
        {
            return;
        }
    }

    pchar.DirectSail.ArrowDiagnostics.LastDay = iDay;
    pchar.DirectSail.ArrowDiagnostics.LastHour = iHour;

    iChr = GetCharacterIndex("MushketCap");
    if(iChr < 0)
    {
        trace("DS QUEST STATUS ARROW: chr=MushketCap character=missing day=" +
              iDay + " hour=" + iHour);
        return;
    }

    rArrow = &characters[iChr];

    sState = "unknown";
    sCity = "none";
    sCityShore = "none";
    sTargetCity = "none";
    sTargetShore = "none";
    sShipName = "unknown";
    sMapName = "unknown";
    sQuestTempShore = "none";
    sTimer = "absent";
    sTimerTarget = "none";
    sTimerFunction = "none";
    sCapCheckTime = "none";
    sCapCheckSince = "none";

    if(CheckAttribute(rArrow, "quest")) sState = rArrow.quest;
    if(CheckAttribute(rArrow, "City")) sCity = rArrow.City;
    if(CheckAttribute(rArrow, "cityShore")) sCityShore = rArrow.cityShore;
    if(CheckAttribute(rArrow, "quest.targetCity")) sTargetCity = rArrow.quest.targetCity;
    if(CheckAttribute(rArrow, "quest.targetShore")) sTargetShore = rArrow.quest.targetShore;
    if(CheckAttribute(rArrow, "Ship.Name")) sShipName = rArrow.Ship.Name;
    if(CheckAttribute(rArrow, "mapEnc.Name")) sMapName = rArrow.mapEnc.Name;
    if(CheckAttribute(pchar, "questTemp.Mushket.Shore")) sQuestTempShore = pchar.questTemp.Mushket.Shore;

    if(CheckAttribute(pchar, "quest.MushketTimer"))
    {
        sTimer = "present";

        if(CheckAttribute(pchar, "quest.MushketTimer.over"))
            sTimer = sTimer + ",over=" + pchar.quest.MushketTimer.over;

        if(CheckAttribute(pchar, "quest.MushketTimer.win_condition.l1.date.year") &&
           CheckAttribute(pchar, "quest.MushketTimer.win_condition.l1.date.month") &&
           CheckAttribute(pchar, "quest.MushketTimer.win_condition.l1.date.day"))
        {
            sTimerTarget = pchar.quest.MushketTimer.win_condition.l1.date.year + "-" +
                           pchar.quest.MushketTimer.win_condition.l1.date.month + "-" +
                           pchar.quest.MushketTimer.win_condition.l1.date.day;
        }

        if(CheckAttribute(pchar, "quest.MushketTimer.win_condition.l1.date.hour"))
            sTimerTarget = sTimerTarget + "@" + pchar.quest.MushketTimer.win_condition.l1.date.hour;

        if(CheckAttribute(pchar, "quest.MushketTimer.function"))
            sTimerFunction = pchar.quest.MushketTimer.function;
    }

    if(CheckAttribute(&NullCharacter, "capitainBase.MushketCap.checkTime"))
        sCapCheckTime = NullCharacter.capitainBase.MushketCap.checkTime;

    if(CheckAttribute(&NullCharacter, "capitainBase.MushketCap.checkTime.control_year") &&
       CheckAttribute(&NullCharacter, "capitainBase.MushketCap.checkTime.control_month") &&
       CheckAttribute(&NullCharacter, "capitainBase.MushketCap.checkTime.control_day"))
    {
        sCapCheckSince = NullCharacter.capitainBase.MushketCap.checkTime.control_year + "-" +
                         NullCharacter.capitainBase.MushketCap.checkTime.control_month + "-" +
                         NullCharacter.capitainBase.MushketCap.checkTime.control_day;
    }

    trace("DS QUEST STATUS ARROW: chr=MushketCap" +
          " state=" + sState +
          " ship=" + sShipName +
          " mapLabel=" + sMapName +
          " city=" + sCity +
          " cityShore=" + sCityShore +
          " targetCity=" + sTargetCity +
          " targetShore=" + sTargetShore +
          " questTempShore=" + sQuestTempShore +
          " MushketTimer=" + sTimer +
          " timerTarget=" + sTimerTarget +
          " timerFunction=" + sTimerFunction +
          " capCheckTimeDays=" + sCapCheckTime +
          " capCheckSince=" + sCapCheckSince +
          " current=" + GetDataYear() + "-" + GetDataMonth() + "-" + GetDataDay() +
          "@" + MakeInt(GetHour()));
}
