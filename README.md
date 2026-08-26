# CLAoP-DirectSail

Target: Caribbean Legend: Age of Pirates 1.2


Included
--------
- Re-enables the dormant Direct Sail island-to-island transition system.
- Allows world-map ship encounters to be encountered while travelling through Direct Sail.
- Continues generating new fleet encounters while Direct Sail is active.
- Uses a shared persistent encounter population, so encounters generated during Direct Sail can also appear on the normal global map.
- Maintains encounter population around the player's current route.
- Supports Warring encounters, allowing opposing AI fleets generated during Direct Sail to appear as live battles.
- Supports special encounters such as floating cargo and shipwrecked survivors.
- Uses the game's normal special-encounter pickup and reward handling.
- Integrates storms into Direct Sail.
- Existing world-map storms continue moving and ageing while Direct Sail is active.
- New storms can be generated during Direct Sail using the game's normal storm-generation rate.
- Storms generated during Direct Sail are persistent and can also appear on the normal global map.
- Approaching a storm produces warning weather before the player enters the storm itself.
- Entering a storm uses the game's normal storm weather, ship damage and tornado handling.
- Preserves storm warning or storm conditions when transitioning directly between sea areas.
- Corrects position preservation when transitioning between sea areas that use different map scales.
- Restores the normal map script whenever the ordinary global-map encounter interface is launched.
- Clears Direct Sail encounter-use markers when the normal world-map cleanup lifecycle runs.
- While in Direct Sail, ship repairs occur once per in-game hour at 1/28 of the normal daily repair rate, providing gradual repairs without making Direct Sail faster than the game's standard daily repair system.


Known limitations
-----------------
- Quest ships and special quest fleets, such as coloured-sail ships or gold fleets visible on the global map, are not supported through Direct Sail.
- The normal global map must still be used when a quest requires the player to locate one of these encounters.
- Support for quest-specific fleets through Direct Sail is unlikely to be added because these encounters rely heavily on the normal world-map and quest scripting systems.


Files
-----
Patch files:
  Program\islands\islands.c.patch
  Program\islands\islands_loader.c.patch
  Program\Weather\WhrWeather.c.patch
  Program\interface\interface.c.patch
  Program\worldmap\worldmap_globals.c.patch

New files:
  Program\interface\DirSailEnc.c
  RESOURCE\INI\INTERFACES\DirSailEnc.ini


Credits and attribution
-----------------------
World-map encounter support includes code adapted from the Direct Sail encounter implementation in:

Gentlemen of Fortune: Historical Eras II
https://chezjfrey.itch.io/gentlemen-of-fortune-historical-eras-module-2

In particular, Program\interface\DirSailEnc.c is based on the Historical Eras II Direct Sail encounter implementation and has been modified for compatibility with Caribbean Legend: Age of Pirates.

Historical Eras II states that its assets are distributed under the Creative Commons Attribution-NonCommercial 4.0 International licence.

Original material remains copyright of its respective authors and contributors. No ownership is claimed over the portions adapted from Historical Eras II.


Compatibility notes
-------------------
The patches were constructed against the Caribbean Legend: Age of Pirates 1.1.2 source files.

Other mods or future game updates that alter the same source sections may conflict with these patches or prevent them from applying.

No new game is expected to be required.
