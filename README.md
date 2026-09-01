# Caribbean Legend: Age of Pirates — Direct Sail

**Current target: Caribbean Legend: Age of Pirates 1.2 Beta — Hotfix #1 (Build 25045685)**

Direct Sail restores and expands the game's dormant real-time island-to-island sailing system.

Instead of travelling through the global map, you can sail directly between islands in the normal sea environment. Direct Sail preserves your position and heading as you cross between sea areas and integrates world-map encounters, roaming quest ships, storms and other strategic-map systems into the journey.

> **Beta status**
>
> The current GitHub version is developed and tested against [Caribbean Legend: Age of Pirates 1.2 Beta — Hotfix #1](https://steamcommunity.com/games/3549020/announcements/detail/674004457340535208).
>
> Compatibility has been checked against the updated Hotfix #1 game scripts. The obsolete Aruba `worldmap_init.c.patch` has been removed because the underlying coordinate correction is now included in the base game.
>
> The Steam Workshop version remains on the stable 1.1.2-compatible build until version 1.2 is released more widely.

## Installation

Download the latest repository files and copy the contents of the `DirectSail` folder over the existing Steam Workshop installation:

`C:\Program Files (x86)\Steam\steamapps\workshop\content\3549020\3781789104`

Allow the updated files to overwrite the Workshop version.

No new game is required.

## Features

### Direct sailing

* Sail directly between islands without entering the global map.
* Position and heading are preserved when moving between sea areas.
* Correctly handles transitions between islands using different map scales.
* Handles the corrected Aruba data supplied by Hotfix #1 and retains Direct Sail's additional Curacao transition handling.
* Adds the player's current position to the paper world maps.
* Ships gradually repair while travelling, at 1/27 of the normal daily repair rate per in-game hour.

### Persistent encounters

Direct Sail uses the same persistent encounter population as the global map.

While travelling you can encounter:

* merchant fleets and convoys;
* patrols and pursuing fleets;
* pirate and naval fleets;
* battles between opposing AI fleets;
* floating cargo;
* stranded or shipwrecked vessels;
* roaming quest ships and quest fleets created through the game's normal world-map encounter system.

New encounters continue to be generated while Direct Sail is active, and encounters generated during Direct Sail can later appear on the normal global map.

Persistent encounters continue ageing while travelling and can survive sea-area transitions.

### Roaming quest encounters

The 1.2 beta adds support for roaming quest encounters that would normally be created on the global map.

Supported encounter types include the game's normal:

* quest traders;
* quest warriors and pursuers;
* fast/cool-warrior pursuit encounters;
* explicit coordinate-based quest traders;
* multi-ship quest groups built around a quest commander.

Direct Sail preserves the underlying quest character and group rather than replacing them with a generic random fleet. Entering the encounter therefore uses the game's normal quest AI, relationships, dialogue and quest logic.

Quest encounters can also move between the two navigation systems:

* a quest encounter discovered in Direct Sail can be handed back to the normal global map;
* a quest encounter already created on the global map can subsequently be used by Direct Sail;
* remaining encounter lifetime is preserved when switching navigation modes;
* quest timeout and arrival events continue through the game's normal `Map_TraderSucces` / `Map_WarriorEnd` lifecycle;
* quest timeouts are deferred while the player is actively inside that quest encounter, preventing the strategic encounter from expiring during tactical combat.

Some quest scripts use same-origin/same-destination encounters as quest-state markers rather than ships that should be exposed to the player. Known cases such as Sharp's search sequence are handled without revealing the hidden clue state through Direct Sail.

### Strategic quest progression

Many game quests use a `MapEnter` condition as a general signal that strategic travel has occurred, even when the quest does not otherwise depend on the global map.

Direct Sail now emits a one-shot synthetic `MapEnter` signal whenever the player completes a genuine sea-area transition. This allows most audited `MapEnter` quest stages to progress while travelling through Direct Sail.

The system:

* respects the game's `bQuestDisableMapEnter` quest lock;
* emits the signal only during genuine sea-area transitions;
* allows ordinary quest-state changes to occur without interrupting sailing;
* supports quest callbacks that interrupt travel with a deck or location scene;
* safely resumes tactical sailing after such an interruption;
* avoids creating the invalid `MapToSea` autosave that would otherwise occur when returning from a synthetic world-map scene.

Some quest stages genuinely require the native world-map entity and remain deliberately excluded. These are listed under [Quests that still require the world map](#quests-that-still-require-the-world-map).

### Watchman

The 1.2 beta adds a **Watchman** system for actively searching for nearby encounters without opening the global map.

Open the normal sea **Enter** menu and select **Watchman**.

Nearby contacts within approximately 250 strategic-map units are listed with:

* encounter type, nationality or quest-ship name;
* compass direction;
* approximate range: **Nearby**, **Moderate** or **Distant**.

Select a contact to begin tracking it. The Watchman reports its updated direction and range once per in-game hour as you sail.

Tracking continues when Direct Sail moves between island sea areas.

Open Watchman again and select **Stop Tracking** to abandon the pursuit.

Tracking is automatically cleared when a tracked quest encounter is entered and when another save is loaded, preventing stale encounter IDs from being carried between saves.

Watchman localisation is included for English, Russian, Simplified Chinese, French, German, Polish, Portuguese and Spanish. Non-English Watchman text was translated from English with AI assistance.

### Encounter replenishment

Direct Sail maintains a local population of persistent encounters rather than relying only on encounters that happened to exist when Direct Sail began.

Encounter generation is evaluated several times per in-game hour so that long Direct Sail journeys produce a population closer to that seen during normal global-map travel.

The system respects the existing local encounter target and does not artificially force a contact to exist.

### Storms

* Existing world-map storms continue moving and ageing while Direct Sail is active.
* New persistent storms can be generated while travelling.
* Storms generated through Direct Sail can subsequently appear on the global map.
* Approaching a storm produces warning weather before entering it.
* Entering a storm uses the game's normal storm weather, damage and tornado systems.
* Storm conditions are preserved during Direct Sail sea-area transitions.

## Known limitations

### Quests that still require the world map

Most audited `MapEnter` quest conditions now work during Direct Sail transitions. The following specific stages remain dependent on the native world map.

These restrictions apply only to the stage described below. They do **not** mean that the entire questline must be played through the world map.

| Quest or situation                            | World-map-only stage                                                                                    | Internal condition                                                                    |
| --------------------------------------------- | ------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------- |
| English national questline — attack on Belize | Processing the expired attack deadline and Henry Morgan's resulting deck intervention                   | `time_over_Beliz_attack_Map_01` and `time_over_Beliz_attack_Map_02`                   |
| Pirate storyline — Panama expedition          | Processing the expired deadline for reaching Porto Bello and Henry Morgan's resulting deck intervention | `PQ8_PanamaTimerOver_01` and `PQ8_PanamaTimerOver_02`                                 |
| Sharp's pearl-diver search                    | Processing the expired deadline for the first pearl-diver search                                        | `SeaPearl_FirstTime_Late_01`                                                          |
| First PGG/random-captain quest                | Processing the overdue result for the first procedurally generated captain quest                        | `PGGQuest1_Time2Late_01`                                                              |
| Peter Blood prologue                          | The world-map tutorial and the associated exit and re-entry sequence                                    | `BloodLine_GlobalTutor_WorldmapTutorial` and `BloodLine_GlobalTutor_OnExitToMapAgain` |

If one of these stages becomes due while using Direct Sail, enter the normal world map and allow the game to process the event there. Direct Sail deliberately leaves the condition pending instead of consuming it incorrectly.

The source audit also identified some quest logic that depends on actual world-map coordinates, proximity or movement rather than a `MapEnter` condition. Synthetic `MapEnter` support cannot replace those systems.

Known examples include:

| Quest or system                  | Requirement                                                                                  |
| -------------------------------- | -------------------------------------------------------------------------------------------- |
| Lost Ships City / Justice Island | Use the normal world map for scripted approach or proximity stages involving Justice Island. |
| Royal Jackpot                    | Use the normal world map for stages which check the player's strategic-map position.         |

Other ordinary stages of these quests may still work with Direct Sail. The world map is required when the quest specifically expects one of the native positional or approach triggers.

### Quest fleet movement

Roaming quest encounters are currently **stationary while represented by Direct Sail**.

For example, if a quest creates a ship travelling from Charlestown to Port Royal, Direct Sail can expose and intercept that quest ship, preserve its timeout and hand it back to the global map, but Direct Sail does not yet simulate its movement along the strategic route.

If the player switches to the normal global map, the encounter returns to the game's native world-map system and moves normally from that point.

Quest fleet movement in Direct Sail is considered a separate future feature.

### Quest coverage

The roaming quest encounter and strategic `MapEnter` implementations have been checked against the game's 1.2 Beta source and are designed to support the active quest-encounter and compatible `MapEnter` methods found there.

However, not every individual quest and every possible quest stage has been runtime-tested.

Fixed quest ships spawned directly into tactical sea or attached to specific islands and shores use a different system. They are not dependent on the roaming world-map quest encounter implementation described above.

World-map dependencies that do not use `MapEnter`, such as direct strategic-coordinate or proximity checks, require separate compatibility work.

### Persistent tactical encounters

Once an ordinary persistent encounter has already spawned into tactical sea, leaving that sea area without engaging it can still cause the encounter to be lost in some circumstances.

Watchman tracking primarily applies while the encounter remains in the persistent strategic encounter population.

## Beta testing

The core Direct Sail system, ordinary persistent encounters, Watchman tracking and standard island transitions have been tested extensively.

Roaming quest encounter support has been tested successfully with several representative quest encounters, including trader and pursuing-fleet behaviours, and has also been reviewed against the wider 1.2 Beta quest source.

Broad strategic `MapEnter` compatibility has been source-audited and tested with representative behaviours including:

* ordinary quest-state advancement during a Direct Sail transition;
* Hugo Lumbersaw / The Reformed Pirate progression;
* overdue passenger delivery;
* interruption of strategic travel with a ship-deck scene;
* safe return to tactical sea without creating an invalid `MapToSea` autosave.

Less common encounter combinations — particularly unusual multi-ship quest groups, `"none"`-origin fleets, quest timeout edge cases and quest scripts with direct world-map dependencies — may still reveal issues during wider testing.

If you encounter a problem, please include:

* a short description of what happened;
* the active quest and current quest stage, if relevant;
* which navigation mode you were using;
* whether you had recently switched between Direct Sail and the global map;
* the game's `compile.log`;
* the game's `error.log`, if one was generated.

The game clears `compile.log` when it closes, so copy the log before exiting the game.

The beta retains targeted diagnostic logging for encounter generation, quest handoff, lifetime handling, Watchman state, strategic `MapEnter` processing and sea-area transitions.

## Repository structure

`DirectSail/Program/`
Game scripts and source patches.

`DirectSail/RESOURCE/`
Interface files, localisation and runtime resources.

`DirectSail/SOURCE_TEXTURE/`
Source artwork used for Direct Sail interface assets.

## Credits and attribution

World-map encounter support includes code adapted from the Direct Sail encounter implementation in:

**Gentlemen of Fortune: Historical Eras II**
https://chezjfrey.itch.io/gentlemen-of-fortune-historical-eras-module-2

In particular, `Program/interface/DirSailEnc.c` is based on the Historical Eras II Direct Sail encounter implementation and has been modified extensively for compatibility with Caribbean Legend: Age of Pirates and the expanded Direct Sail system.

Historical Eras II states that its assets are distributed under the Creative Commons Attribution-NonCommercial 4.0 International licence.

Original material remains copyright of its respective authors and contributors. No ownership is claimed over portions adapted from Historical Eras II.

## Compatibility

The current GitHub build is developed and tested against **Caribbean Legend: Age of Pirates 1.2 Beta — Hotfix #1 (Build 25045685)**.

The remaining Direct Sail source-patch anchors have been checked against the Hotfix #1 game files. The obsolete `Program/worldmap/worldmap_init.c.patch` file has been removed because the Aruba coordinate correction it previously supplied is now part of the base game.

The Steam Workshop release currently remains on the stable **1.1.2-compatible** version.

Other mods or future game updates that alter the same patched source sections may conflict with Direct Sail or prevent its patches from applying.

No new game is required.
