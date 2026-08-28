# Caribbean Legend: Age of Pirates — Direct Sail

**Current target: Caribbean Legend: Age of Pirates 1.2 Beta**

Direct Sail restores and expands the game's dormant real-time island-to-island sailing system.

Instead of travelling through the global map, you can sail directly between islands in the normal sea environment. Direct Sail preserves your position and heading as you cross between sea areas and integrates world-map encounters, storms and other systems into the journey.

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
- Adds the player's current position to the paper world maps.
- Ships gradually repair while travelling, at 1/28 of the normal daily repair rate per in-game hour.

### Persistent encounters

Direct Sail uses the same persistent encounter population as the global map.

This means that while travelling you can encounter:

- merchant fleets and convoys;
- patrols and pursuing fleets;
- pirate and naval fleets;
- battles between opposing AI fleets;
- floating cargo;
- stranded or shipwrecked vessels.

New encounters continue to be generated while Direct Sail is active, and encounters generated during Direct Sail can later appear on the normal global map.

### Watchman

The 1.2 beta adds a new **Watchman** system for actively searching for nearby encounters without opening the global map.

Open the normal sea **Enter** menu and select **Watchman**.

Nearby contacts are listed with:

- encounter type or nationality;
- compass direction;
- approximate range: **Nearby**, **Moderate** or **Distant**.

Select a contact to begin tracking it. The Watchman will report its updated direction and range once per in-game hour as you sail.

Tracking continues when Direct Sail moves between island sea areas.

Open Watchman again and select **Stop Tracking** to abandon the pursuit.

Battles between two AI fleets are presented as a single contact showing both participating nations. Flotsam and stranded-vessel encounters can also be detected and tracked.

Watchman results are sorted with the nearest contacts first.

Watchman localisation is included for English, Russian, Simplified Chinese, French, German, Polish, Portuguese and Spanish. Non-English Watchman text was translated from English with AI assistance.

### Storms

- Existing world-map storms continue moving and ageing while Direct Sail is active.
- New persistent storms can be generated while travelling.
- Storms generated through Direct Sail can subsequently appear on the global map.
- Approaching a storm produces warning weather before entering it.
- Entering a storm uses the game's normal storm weather, damage and tornado systems.
- Storm conditions are preserved during Direct Sail sea-area transitions.

## Known limitations

Quest ships and special quest fleets that are intended to be located through the global map — such as coloured-sail encounters and other scripted fleets — are not supported through Direct Sail.

The normal global map must still be used when a quest specifically requires one of these encounters to be located.

Support for these quest-specific encounters is unlikely because they depend heavily on the game's normal world-map and quest scripting systems.

Once a persistent encounter has already spawned into the tactical sea, leaving that sea area without engaging it may cause the encounter to be lost. Watchman tracking primarily applies while the encounter remains in the persistent world-map population.

## Beta testing

The core Direct Sail system and ordinary fleet tracking have been tested extensively.

Warring battles, flotsam and stranded-vessel encounters are less common and may still reveal edge cases during wider beta testing.

If you encounter a problem, a description of what happened together with the game's `compile.log` is particularly useful. The beta retains targeted diagnostic logging for these rarer encounter types.

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
