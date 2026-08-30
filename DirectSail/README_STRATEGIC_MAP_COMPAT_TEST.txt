DirectSail Strategic-Map Compatibility - Prototype 1
====================================================

STATUS
------
TEST BUILD ONLY.  This branch is intentionally not merged into main.

Prototype 1 validates two different CLAoP MapEnter behaviours:

1. Hugo Lumbersaw / The Reformed Pirate
   PDM_Lesopilka_Treasures_BackToVillemstad
   - simple MapEnter quest-state advancement

2. Repeatable passenger delivery
   ConvoyMapPassenger -> AllPassengerDeck
   - MapEnter interrupts strategic travel and reloads to Ship_deck

All other MapEnter quests remain pending in this prototype.

CHANGED MOD PATCH FILES
-----------------------
These are complete replacement .patch files.  When testing an existing
DirectSail installation, overwrite/copy these paths from this branch:

    DirectSail\Program\sea_ai\sea.c.patch
    DirectSail\Program\quests\quests.c.patch
    DirectSail\Program\quests\quests_check.c.patch

The islands_loader patch is NOT changed by Prototype 1.

HOW THE PROTOTYPE WORKS
-----------------------
During a normal DirectSail sea-zone transition the old tactical sea is already
unloaded when SeaLogin() begins.  SeaLogin() sets pchar.location to the new
island/sea-region and then raises EVENT_SEA_LOGIN before it constructs the new
sea environment.

At that boundary Prototype 1 temporarily marks a Direct Sail MapEnter pulse,
reconstructs the destination world-map-equivalent coordinates, and lets the
normal QuestsCheck() event run.

quests_check.c.patch permits MapEnter to become true only for the two test
quests above.  Other active MapEnter conditions are logged but not consumed.

If the quest requests a world-map-to-location interruption, quests.c.patch
preserves that reload request, tells SeaLogin() not to construct the new sea,
and completes the location reload immediately afterwards.

HUGO TEST
---------
Use a save where Hugo is still a passenger after the Dominica treasure stage,
before manually entering the vanilla world map to fix the quest.

DirectSail until a genuine sea-zone transition occurs.

Expected compile.log lines include:

    DS MAPENTER PULSE: from=... to=...
    DS MAPENTER FIRE: quest=PDM_Lesopilka_Treasures_BackToVillemstad
    DS MAPENTER CONTINUE: destination=...

Then enter Willemstad town normally.  Hugo's final conversation should now be
armed without having visited the strategic world map.

PASSENGER TEST - NORMAL METHOD
------------------------------
Find and accept the repeatable tavern passenger job whose quest-log title is:

    Deliver the passenger named <name> to <city>

Do NOT deliver the passenger.  Let the original delivery deadline expire.
The game then arms ConvoyMapPassenger and waits for MapEnter so the passenger
can call you to the ship deck.

Remain in DirectSail and cross a genuine sea-zone boundary.

Expected compile.log lines include:

    DS MAPENTER PULSE: from=... to=...
    DS MAPENTER FIRE: quest=ConvoyMapPassenger
    DS MAPENTER INTERRUPT REQUEST: location=Ship_deck group=goto locator=goto5
    DS MAPENTER RELOAD DEFERRED: location=Ship_deck
    DS MAPENTER INTERRUPT HANDOFF: location=Ship_deck
    DS MAPENTER INTERRUPT COMPLETE: location=Ship_deck

Expected gameplay:
- you are taken to your ship deck;
- the overdue passenger speaks to you;
- choosing to continue the delivery grants another seven days;
- the reward is reduced;
- after leaving the deck, normal sea travel should resume.

PASSENGER TEST - QUICK ONE-DAY HELPER
-------------------------------------
The branch also contains a TEST-ONLY helper at:

    TestOnly\PassengerDeadline1Day\Program\dialogs\convoy_passenger.c.patch

To use it, copy that file temporarily to:

    DirectSail\Program\dialogs\convoy_passenger.c.patch

Then accept a NEW passenger-delivery job.  Its initial deadline will be one
in-game day.  The compile log will contain:

    DS TEST PASSENGER: initial deadline forced to 1 day

After the day expires, remain in DirectSail and cross a sea-zone boundary.
Remove the test-only patch when the passenger test is finished.

CONTROL TEST
------------
Cross several normal sea-zone boundaries when neither Hugo nor an overdue
passenger MapEnter condition is pending.

There should be no gameplay interruption and DirectSail transitions should
continue exactly as before.

DIAGNOSTICS
-----------
Prototype 1 uses these trace prefixes:

    DS MAPENTER PULSE
    DS MAPENTER FIRE
    DS MAPENTER PENDING
    DS MAPENTER PROTOTYPE SKIP AFTER INTERRUPT
    DS MAPENTER INTERRUPT REQUEST
    DS MAPENTER RELOAD DEFERRED
    DS MAPENTER INTERRUPT HANDOFF
    DS MAPENTER INTERRUPT COMPLETE
    DS MAPENTER CONTINUE
    DS MAPENTER ERROR

If a test fails, save the compile.log from that session before restarting the
game and provide it for the next build.
