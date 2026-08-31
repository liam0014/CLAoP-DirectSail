DirectSail Strategic-Map Compatibility
======================================

STATUS
------
Broad MapEnter compatibility build.

The original prototype validated two representative behaviours:

1. Hugo Lumbersaw / The Reformed Pirate
   PDM_Lesopilka_Treasures_BackToVillemstad
   - simple MapEnter quest-state advancement

2. Repeatable passenger delivery
   ConvoyMapPassenger -> AllPassengerDeck
   - MapEnter interrupts strategic travel and reloads to Ship_deck

Both paths have been tested successfully, including returning from the synthetic
Ship_deck scene without creating the invalid MapToSea autosave.

BROAD MAPENTER COMPATIBILITY
----------------------------
A source audit found that most CLAoP MapEnter conditions use MapEnter as a
strategic-travel / leave-sea sequencing event and do not require a live native
world-map entity.

DirectSail therefore now emits a one-shot synthetic MapEnter pulse on every
genuine sea-zone transition, provided the game has not set the hard quest map
lock bQuestDisableMapEnter.

During the pulse, MapEnter conditions are allowed by default.  The following
audited conditions remain explicitly denied because their callbacks genuinely
require the native world map or are part of the Peter Blood world-map tutorial:

    time_over_Beliz_attack_Map_01
    time_over_Beliz_attack_Map_02
    PQ8_PanamaTimerOver_01
    PQ8_PanamaTimerOver_02
    SeaPearl_FirstTime_Late_01
    PGGQuest1_Time2Late_01
    BloodLine_GlobalTutor_WorldmapTutorial
    BloodLine_GlobalTutor_OnExitToMapAgain

These exclusions should continue to require native world-map use or separate
DirectSail-specific compatibility work.

This does NOT solve world-map dependencies which do not use MapEnter, such as
systems based on actual world-map proximity, coordinates or movement.  Known
examples include Lost Ships City / Justice Island approach logic and Royal
Jackpot positional checks.

HOW IT WORKS
------------
During a normal DirectSail sea-zone transition the old tactical sea is already
unloaded when SeaLogin() begins.  SeaLogin() knows the new island/sea-region and
raises EVENT_SEA_LOGIN before constructing the destination tactical sea.

At that boundary DirectSail reconstructs the destination strategic-map position,
marks DirectSail.MapEnterPulse, and lets the normal quest-check event run.

quests_check.c.patch exposes the current quest name while that pulse is active.
MapEnter then:

- returns true for ordinary audited-compatible quest conditions;
- returns false for the explicit denylist above;
- returns false for any later MapEnter condition once a deck/location
  interruption has already been requested in the same quest-check pass.

If a callback calls DoReloadFromWorldMapToLocation(), quests.c.patch preserves
the reload request, aborts construction of the destination tactical sea, and
finishes the location reload immediately after SeaLogin() returns.

When a synthetic Ship_deck interruption later returns to sea, interface.c.patch
skips only the unsafe MapToSea autosave associated with that synthetic return,
then executes the normal after-save continuation so tactical sailing resumes.

RECOMMENDED REGRESSION TESTS
----------------------------
1. Ordinary DirectSail transition
   Cross a normal sea-zone boundary with no expected quest event.

   Expected:
       DS MAPENTER PULSE: from=... to=...
       DS MAPENTER CONTINUE: destination=...

   There should be no gameplay interruption.

2. Hugo / The Reformed Pirate
   Use a save where Hugo is still a passenger after the Dominica treasure stage.
   Cross a genuine DirectSail sea-zone transition.

   Expected:
       DS MAPENTER FIRE: quest=PDM_Lesopilka_Treasures_BackToVillemstad

   Hugo's Willemstad return stage should advance without entering the native map.

3. Overdue passenger
   Use a save with ConvoyMapPassenger pending and cross a sea-zone boundary.

   Expected:
       DS MAPENTER FIRE: quest=ConvoyMapPassenger
       DS MAPENTER INTERRUPT REQUEST: location=Ship_deck ...
       DS MAPENTER RELOAD DEFERRED: location=Ship_deck
       DS MAPENTER INTERRUPT HANDOFF: location=Ship_deck
       DS MAPENTER INTERRUPT COMPLETE: location=Ship_deck
       DS MAPENTER AUTOSAVE SKIP: type=MapToSea

   After the dialogue, tactical sea should resume normally and no invalid save
   should be created.

4. Save/reload control
   After returning to tactical sea from an interruption, create a normal save,
   reload it, and confirm DirectSail continues normally.

DIAGNOSTICS
-----------
Useful trace prefixes:

    DS MAPENTER PULSE
    DS MAPENTER PULSE BLOCKED
    DS MAPENTER FIRE
    DS MAPENTER DENYLIST
    DS MAPENTER SKIP AFTER INTERRUPT
    DS MAPENTER INTERRUPT REQUEST
    DS MAPENTER RELOAD DEFERRED
    DS MAPENTER INTERRUPT HANDOFF
    DS MAPENTER INTERRUPT COMPLETE
    DS MAPENTER AUTOSAVE SKIP
    DS MAPENTER CONTINUE
    DS MAPENTER ERROR

If an unexpected quest advances incorrectly, preserve the compile.log.  The
DS MAPENTER FIRE line identifies the newly-enabled quest condition which can be
reviewed and, if necessary, added to the denylist.
