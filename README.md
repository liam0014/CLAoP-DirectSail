# Caribbean Legend: Age of Pirates — Direct Sail

**Current target: Caribbean Legend: Age of Pirates 1.2 Beta (24.08.2026)**

Direct Sail restores and expands the game's dormant real-time island-to-island sailing system.

Instead of travelling through the global map, you can sail directly between islands in the normal sea environment. Direct Sail preserves your position and heading as you cross between sea areas and integrates world-map encounters, roaming quest ships, storms and other strategic-map systems into the journey.

> **Beta status**
>
> The current GitHub version is developed and tested against Caribbean Legend: Age of Pirates 1.2 Beta.
>
> The Steam Workshop version remains on the stable 1.1.2-compatible build until version 1.2 is released more widely.

## Installation

Download the latest repository files and copy the contents of the `DirectSail` folder over the existing Steam Workshop installation:

`C:\Program Files (x86)\Steam\steamapps\workshop\content\3549020\3781789104`

Allow the updated files to overwrite the Workshop version.

No new game is required.

## Features

### Direct sailing

- Sail directly between islands without entering the global map.
- Position and heading are preserved when moving between sea areas.
- Correctly handles transitions between islands using different map scales.
- Includes additional transition fixes for locations such as Aruba and Curacao.
- Adds the player's current position to the paper world maps.
- Ships gradually repair while travelling, at 1/27 of the normal daily repair rate per in-game hour.

### Persistent encounters

Direct Sail uses the same persistent encounter population as the global map.

While travelling you can encounter:

- merchant fleets and convoys;
- patrols and pursuing fleets;
- pirate and naval fleets;
- battles between opposing AI fleets;
- floating cargo;
- stranded or shipwrecked vessels;
- roaming quest ships and quest fleets created through the game's normal world-map encounter system.

New encounters continue to be generated while Direct Sail is active, and encounters generated during Direct Sail can later appear on the normal global map.

Persistent encounters continue ageing while travelling and can survive sea-area transitions.

### Roaming quest encounters

The 1.2 beta adds support for roaming quest encounters that would normally be created on the global map.

Supported encounter types include the game's normal:

- quest traders;
- quest warriors and pursuers;
- fast/cool-warrior pursuit encounters;
- explicit coordinate-based quest traders;
- multi-ship quest groups built around a quest commander.

Direct Sail preserves the underlying quest character and group rather than replacing them with a generic random fleet. Entering the encounter therefore uses the game's normal quest AI, relationships, dialogue and quest logic.

Quest encounters can also move between the two navigation systems:

- a quest encounter discovered in Direct Sail can be handed back to the normal global map;
- a quest encounter already created on the global map can subsequently be used by Direct Sail;
- remaining encounter lifetime is preserved when switching navigation modes;
- quest timeout and arrival events continue through the game's normal `Map_TraderSucces` / `Map_WarriorEnd` lifecycle;
- quest timeouts are deferred while the player is actively inside that quest encounter, preventing the strategic encounter from expiring during tactical combat.

Some quest scripts also use same-origin/same-destination encounters as quest-state markers rather than ships that should be exposed to the player. Known cases such as Sharp's search sequence are handled without revealing the hidden clue state through Direct Sail.

### Watchman

The 1.2 beta adds a **Watchman** system for actively searching for nearby encounters without opening the global map.

Open the normal sea **Enter** menu and select **Watchman**.

Nearby contacts within approximately 250 strategic-map units are listed with:

- encounter type, nationality or quest-ship name;
- compass direction;
- approximate range: **Nearby**, **Moderate** or **Distant**.

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

- Existing world-map storms continue moving and ageing while Direct Sail is active.
- New persistent storms can be generated while travelling.
- Storms generated through Direct Sail can subsequently appear on the global map.
- Approaching a storm produces warning weather before entering it.
- Entering a storm uses the game's normal storm weather, damage and tornado systems.
- Storm conditions are preserved during Direct Sail sea-area transitions.

## Known limitations

### Quest fleet movement

Roaming quest encounters are currently **stationary while represented by Direct Sail**.

For example, if a quest creates a ship travelling from Charlestown to Port Royal, Direct Sail can expose and intercept that quest ship, preserve its timeout, and hand it back to the global map, but Direct Sail does not yet simulate its movement along the strategic route.

If the player switches to the normal global map, the encounter returns to the game's native world-map system and moves normally from that point.

Quest fleet movement in Direct Sail is considered a separate future feature.

### Quest coverage

The roaming quest encounter implementation has been checked against the game's 1.2 Beta source and is designed to support all active quest-encounter creation methods currently found there.

However, not every individual quest and every quest stage has been runtime-tested.

Fixed quest ships that are spawned directly in tactical sea or attached to specific island/shore locations use a different system and are not dependent on the roaming world-map quest encounter implementation described above.

### Persistent tactical encounters

Once an ordinary persistent encounter has already spawned into tactical sea, leaving that sea area without engaging it can still cause the encounter to be lost in some circumstances. Watchman tracking primarily applies while the encounter remains in the persistent strategic encounter population.

## Beta testing

The core Direct Sail system, ordinary persistent encounters, Watchman tracking and standard island transitions have been tested extensively.

Roaming quest encounter support has been tested successfully with several representative quest encounters, including trader and pursuing-fleet behaviours, and has also been reviewed against the wider 1.2 Beta quest source.

Less common encounter combinations — particularly unusual multi-ship quest groups, `"none"`-origin fleets and quest timeout edge cases — may still reveal issues during wider testing.

If you encounter a problem, please include:

- a short description of what happened;
- what navigation mode you were using;
- whether you had recently switched between Direct Sail and the global map;
- the game's `compile.log`, note that the compile.log is cleared when you close the game so you need to grab this before you exit;
- the game's `error.log` if one was generated.

The beta retains targeted diagnostic logging for encounter generation, quest handoff, lifetime handling, Watchman state and sea-area transitions.

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

The current GitHub build is developed and tested against **Caribbean Legend: Age of Pirates 1.2 Beta (24.08.2026)**.

The Steam Workshop release currently remains on the stable **1.1.2-compatible** version.

Other mods or future game updates that alter the same patched source sections may conflict with Direct Sail or prevent its patches from applying.

No new game is required.
