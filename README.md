# Caribbean Legend: Age of Pirates — Direct Sail

**Current target: Caribbean Legend: Age of Pirates 1.2**

Direct Sail restores and expands the game's dormant real-time island-to-island sailing system.

Instead of travelling through the global map, you can sail directly between islands in the normal sea environment. Direct Sail preserves your position and heading as you cross between sea areas and integrates world-map encounters, roaming quest ships, storms and other strategic-map systems into the journey.

> **Version 1.2 status**
>
> The current GitHub and Steam Workshop versions target the official release of **Caribbean Legend: Age of Pirates 1.2**.
>
> Direct Sail's source patches and world-map integration have been reviewed against the final 1.2 game scripts and runtime-tested on the official 1.2 release. 
>
> Direct Sail was developed and tested with the **Sea Dogs** DLC installed. The newer **Buccaneers** DLC has **not** been used during development or compatibility testing; see [Buccaneers DLC compatibility](#buccaneers-dlc-compatibility).
>
> If you are still using **Caribbean Legend: Age of Pirates 1.1.2**, use the dedicated [1.1.2 branch](https://github.com/liam0014/CLAoP-DirectSail/tree/1.1.2) instead of the current 1.2 build.

## Installation

Download the latest repository files and copy the contents of the `DirectSail` folder over the existing Steam Workshop installation:

`C:\Program Files (x86)\Steam\steamapps\workshop\content\3549020\3781789104`

Allow the updated files to overwrite the Workshop version.

If you are still using **game version 1.1.2**, download the [1.1.2 branch](https://github.com/liam0014/CLAoP-DirectSail/tree/1.1.2) instead.

No new game is required.

## Features

### Direct sailing

* Sail directly between islands without entering the global map.
* Position and heading are preserved when moving between sea areas.
* Correctly handles transitions between islands using different map scales.
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

Version 1.2 adds support for roaming quest encounters that would normally be created on the global map.

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

Synthetic `MapEnter` handling is deliberately **fail-closed**. Only conditions identified and source-audited in the official 1.2 game files are allowed to consume the Direct Sail signal. Any unknown `MapEnter` condition — including conditions added by DLC or a future game update — remains pending until the player enters the normal world map.

The system:

* respects the game's `bQuestDisableMapEnter` quest lock;
* emits the signal only during genuine sea-area transitions;
* allows only source-audited official 1.2 `MapEnter` conditions to consume the synthetic signal;
* leaves unknown or unaudited `MapEnter` conditions pending for the native world map;
* allows ordinary quest-state changes to occur without interrupting sailing;
* supports quest callbacks that interrupt travel with a deck or location scene;
* safely resumes tactical sailing after such an interruption;
* avoids creating the invalid `MapToSea` autosave that would otherwise occur when returning from a synthetic world-map scene.

Some audited quest stages genuinely require the native world-map entity and therefore remain deliberately excluded. These are listed under [Quests that still require the world map](#quests-that-still-require-the-world-map).

### Watchman

Version 1.2 adds a **Watchman** system for actively searching for nearby encounters without opening the global map.

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

Direct Sail also respects the game's `worldmapencountersoff` flag. As in the native world-map generator, merchants remain eligible while hostile/pursuing and special encounter generation is suppressed.

### Storms

* Existing world-map storms continue moving and ageing while Direct Sail is active.
* New persistent storms can be generated while travelling.
* New natural storm generation is suppressed while the game's `worldmapencountersoff` flag is active; existing storms continue moving and ageing.
* Storms generated through Direct Sail can subsequently appear on the global map.
* Approaching a storm produces warning weather before entering it.
* Entering a storm uses the game's normal storm weather, damage and tornado systems.
* Storm conditions are preserved during Direct Sail sea-area transitions.

## Known limitations

### Audited `MapEnter` compatibility

Direct Sail uses a **fail-closed** synthetic `MapEnter` system. A Direct Sail sea-area transition can advance a `MapEnter` condition only when that condition was identified and source-audited against the official **Caribbean Legend: Age of Pirates 1.2** game files.

The table below lists every quest or strategic event currently permitted to consume Direct Sail's synthetic `MapEnter` signal. The names in the first column use the game's English quest-book titles where one exists. A few engine/system events do not have quest-book entries and are labelled accordingly.

#### Included quests and strategic events

| In-game quest / event | Direct Sail-compatible stage | Internal condition(s) |
| --- | --- | --- |
| Chronicle of City Conquests | If the player leaves a captured town before the temporary capture sequence has fully cleaned up, reset that state and free the forts. | `FreeCaptureForts` |
| Pirate’s Offer | After the first PGG/random-captain sea fight, entering strategic travel resolves the post-battle/runaway outcome and cleans up the encounter. | `PGGQuest1_Runaway` |
| Reason to Hurry | After the victim/hunter branch reaches the next sea stage, strategic travel prepares the pirate ship required for the confrontation. | `ReasonToFast_SetPirateShip` |
| Captain’s Assignment (Ransom) | During the pirate-squadron battle, leaving the battle invokes the quest’s withdrawal/failure resolution. | `CapComission_DieHard` |
| Captain’s Assignment (Operation “Galleon”) | Post-gang, captain-death and shore-battle cleanup: strategic travel removes temporary characters/groups or advances the operation after leaving the shore. | `CaptainComission_ExitFromShoreAfterGang`<br>`CaptainComission_MapEnterAfterGang`<br>`CaptainComission_MapEnter`<br>`CaptainComission_MapEnterAfterShoreBattle` |
| Shipwreck survivors | After the survivors/captain outcome is resolved, the next strategic-travel transition removes the temporary shipwreck characters and finishes cleanup. | `ShipWreck_MapEnter` |
| Crew salary / payday (system event) | If the salary screen could not be shown immediately, show it on the next strategic-travel transition. | `LaunchSalaryScreen` |
| French national questline — FL5 (legacy English journal header absent) | After the silver-convoy engagement or the later Maracaibo fleet sequence, clean up temporary fleets/companions and advance toward the next Tortuga stage. | `FL5as26`<br>`FL5s45` |
| French national questline — FL7 (legacy English journal header absent) | After defeating the Spanish squadron at Cumaná, move from the naval battle into the scripted post-battle storm/shore sequence. | `FL7s25` |
| French national questline — FL9 (legacy English journal header absent) | After the French/Spanish battle at Santa Catalina, restore the temporary nation relation, clear the fleet and advance the return-to-Tortuga stage. | `FL9s59` |
| Nation Legend / City of Lost Ships system handoff | Show the deferred Nation Legend interface when quest processing becomes safe; the same audited condition name is also reused for the Lost Ships City storm handoff. | `Nation_Legend_Map`<br>`Nation_Legend_Map_TEST` |
| Deliver the passenger named @<sName> to @<sCity> | When the passenger-delivery deadline expires at sea, the next strategic-travel transition starts the overdue-passenger deck confrontation. | `ConvoyMapPassenger` |
| Captain of the privateer frigate Morris Williams | During the attempt to take the Spanish pinnace, leaving the battle resolves the stage as the pinnace not being captured. | `MorrisWillams_DieHard` |
| To Edward Mansfeld for gold | During the Spanish-galleon interception, leaving the encounter resolves the stage as the galleon not being captured/found. | `ToMansfield_DieHard` |
| Attack on Santiago | During the battle with Pardal’s squadron off Santiago, leaving sea runs the audited battle-resolution branch and clears the temporary squadron. | `AttackSantiago_DieHard` |
| Attack of the Spanish Squadron on Port Royal | During the defence of Port Royal, leaving the battle applies the quest-line failure/capture outcome and cleans up the attacking fleet. | `SpanishAttack_DieHard` |
| Defend Fort Orange from the impending English attack | During the English attack on Fort Orange, leaving the battle resolves the failed-defence/captured-settlement state. | `DefenceOrange_DieHard` |
| Escort three fluyts to the buccaneer settlement of La Vega and back | During the Spanish attack on the escort, leaving the battle resolves the failed escort and removes the temporary convoy/attack groups. | `ThreeFleutes_DieHard` |
| Deliver the dispatch to Governor Bertrand d’Ogeron of Tortuga | During the pirate-brig encounter near Puerto Rico or the Spanish-warship encounter near Santa Catalina, leaving resolves the corresponding missed/escaped encounter branch. | `DelivLettTortuga_BrigDieHard`<br>`DelivLettTortuga_WarshipDieHard` |
| Assignment to Aaron Mendes Chumakeiro | During the attempt to save de Graaf, leaving sea resolves the rescue outcome; later, after speaking with de Graaf, strategic travel returns him to the Tortuga tavern. | `SeekBible_DieHard`<br>`SeekBible_DeGrafToTavern` |
| Repel the Spanish squadron’s attack on Curacao | During the defence of Willemstad, leaving the battle breaks the quest line, applies the Spanish-capture outcome and cleans the attacking fleet. | `SpaAttackCuracao_DieHard` |
| Repel the Spanish attack on San Martin | During the defence of San Martin, leaving the battle breaks the quest line, applies the Spanish-capture outcome and cleans the attacking fleet. | `SpaAttackSentMartin_DieHard` |
| Work for the Holy Inquisition | During its scripted sea encounter, leaving resolves the escape/failure branch; after the fight with Andrew in the Willemstad bank, strategic travel restores the normal bank/usurer state. | `Inquisition_DieHard`<br>`Inquisition_afterFightAndrew` |
| Capture Roca the Brazilian | During the target’s scripted sea encounter, leaving the battle runs the audited escape/failure cleanup. | `TakeRockBras_DieHard` |
| Intercepting the Dutch Governor-General’s Messenger | After the scripted Tortuga sea action, the next strategic-travel transition restores normal Tortuga island access and cleans the temporary encounter state. | `Sp6TakeMess_OpenTortuga` |
| Assist Manoel Rivero Pardal | During the scripted naval battle, leaving sea runs the quest’s audited withdrawal/failure cleanup. | `Sp7SavePardal_DieHard` |
| Defending Cumana from the attack of the combined Franco-English pirate squadron | During the defence battle, leaving sea runs the audited withdrawal/failure cleanup. | `Sp8SaveCumana_DieHard` |
| Portobelo Governor’s Escort Mission | During the pirate ambush near Caiman on the return leg with the four galleons, leaving sea resolves the failed escort and removes the convoy. | `Sp9SaveCumana_DieHard` |
| Defending Maracaibo from a possible invasion | During the scripted defence battle, leaving sea applies the quest-line failure/capture outcome and cleans the attacking fleet. | `Sp10Maracaibo_DieHard` |
| Escort the ship of the line 'Soleil Royal' to Guadeloupe | During the scripted escort battle, leaving sea runs the audited withdrawal/failure cleanup. | `Fr4SoleiRoyal_DieHard` |
| Repelling the Spanish attack on Port-au-Prince | During the defence battle, leaving sea applies the quest-line failure/capture outcome and cleans the attacking fleet. | `Fr9GuardPP_DieHard` |
| The Story of Beautiful Isabella | During the pirate-brig pursuit near Porto Bello, leaving the encounter records that the brig escaped and advances the corresponding Atilla dialogue state. | `Romantic_BrigDieHard` |
| Searching for Mummy Powder | During the Spanish man-of-war encounter near Caiman, leaving sea records that the target warships were not destroyed and cleans the battle. | `Ascold_DieHardManowar` |
| Cursed Idol | During the sea battle with James Callow, leaving sea closes the quest through its failure branch and cleans the encounter. | `PDM_NEPobeda_nad_Callow` |
| Destroy the xebec "Blue Bird" | During the Blue Bird fight and the later merchant-fluyt interception, leaving strategic sea ends the current interception and performs the scripted cleanup. | `BlueBird_DieHard2`<br>`BlueBird_DieHard1`<br>`BlueBirdFleut_over3` |
| Secrets of the Pearl Craft | Leaving a Sharp roaming encounter, a pearl-diver fleet attack or the Sharp revenge encounter resolves/cleans the current sea stage as scripted. | `Sharp_mapOver`<br>`SeaPearl_DieHard2`<br>`SeaPearl_SharpRevenge_OnExit` |
| Find and kill Edward Low | During the Edward Low / Sea Wolf sea-search stage, entering strategic travel ends the current interception and advances or resets the search as scripted. | `PiratesLine_q3_over3` |
| Morgan’s Tip on the Pearl Hunters’ Tartanas | During the pearl-fleet raid, leaving sea resolves the raid through its audited result/failure branch. | `PQ4_SeaPearl_DieHard2` |
| Enchanted City | If the quest initializes while the player is already in Caracas, the next strategic-travel transition re-arms the Caracas location trigger so the quest can start cleanly on re-entry. | `MC_startAgain` |
| Slave Trader | Across the quest’s scripted sea/shore pursuit battles, leaving sea runs the corresponding escape/failure cleanup; the Havana battle has its own audited MapEnter resolution. | `Slavetrader_DieHard`<br>`Slavetrader_DieHardHavana` |
| Prison Affairs (Note) | During the trader/courier ship engagements, leaving sea records the scripted failure/escape outcome and cleans the temporary fleet. | `jailCanMoveDeliver_DieHard`<br>`jailCanMoveDeliver_DieHardC` |
| Officer departure / pay dispute (system event) | If an officer’s departure cannot be resolved immediately, process it on the next strategic-travel transition; this can also lead into the normal crew-mutiny logic when applicable. | `mOfficer_fc2` |
| Condottiere | During the quest’s scripted target-fleet encounters, leaving sea invokes the appropriate escape/failure branch and cleans the temporary fleet. | `Headhunter_DieHard` |
| The Reformed Pirate | Leaving the Dominica treasure expedition abandons/fails that stage; after the grotto sequence, strategic travel restores the temporary Dominica changes and switches the objective back toward Willemstad. | `PDM_Lesopilka_Treasures_Leave`<br>`PDM_Lesopilka_Treasures_BackToVillemstad` |
| Find Steve Linney | After reading the clue and being sent to sea, the first strategic-travel transition checks elapsed time: spawn the quest battleship if still timely, otherwise record the late result. | `PQ7_setBattleShip` |
| Willemstad city-capture cleanup (system event) | After the relevant capture/governor sequence, the next strategic-travel transition restores the normal Willemstad residence/gate state. | `VillemstadResGatesLock` |
| Nameless Debtor | Peter Blood prologue: after accepting the usurer task, the next strategic-travel transition performs the prologue-specific quest update. | `CapBloodLine_UsurerQuest_MapEnter` |
| Contraband Sale | After a coast-guard pursuit at sea, the next strategic-travel transition stops the pursuit and applies the normal aftermath/rumour cleanup. | `Rand_ContrabandAtSeaEnded` |
| Bermuda shipyard — temporary dungeon access (side interaction) | After paying for temporary dungeon access, the next strategic-travel transition closes the shipyard dungeon door and resets the temporary access state. | `CloseBermudesDungeonDoor` |
| The Noble Lady's Dressmaker | Leaving Willemstad before completing the opening local investigation fails/closes the mission and resets its temporary quest state. | `EPL_PZD_ProvalMissii` |
| The beginning of something new | After escaping the La Vega pursuers, the next strategic-travel transition completes the prologue handoff, clears the temporary attackers and sets the Santo Domingo follow-up. | `EPL_PrologueEnd` |
| Escort of the fluyt "Orion" | During the pirate-squadron attack off Hispaniola, entering strategic travel counts as abandoning the fight/escort and closes the quest through its scripted branch. | `Andre_Abel_Quest_EnterMap` |
| Spanish Treasure Fleet | When the player leaves or completes contact with the Treasure Fleet, update the attacked/unattacked result and persistent fleet lifecycle; the later Havana check uses the same audited resolution logic. | `LeaveGoldleet`<br>`CheckHavanaGoldFleet` |
| Generated citizen captain quests — Wanted Captains / rescue / revenge variants | For generated `SCQ_*` captain quests, the next strategic-travel transition after the target captain is resolved updates the correct journal entry and cleans the map encounter. This covers the game’s Wanted Captains, missing-husband/friend, rescue and revenge variants. | dynamic `SCQ_*` quest name; audited callback `SCQ_seekCapIsDeath` |

Only the conditions listed above are allowed to consume a synthetic Direct Sail `MapEnter`. **Everything else is excluded by default** and remains pending until the player enters the normal world map. This includes conditions added by future game updates, other mods or DLC.

**Buccaneers DLC:** I do not own the **Buccaneers** DLC, so its quest source has not been audited for Direct Sail. As a result, **all Buccaneers `MapEnter` conditions are excluded by default** and will only process when the real world map is entered. This does **not** mean that every Buccaneers quest is incompatible with Direct Sail; only that Direct Sail will not synthetically advance an unaudited `MapEnter` stage.

#### Known excluded `MapEnter` stages

The following official 1.2 conditions were specifically identified during the source audit as requiring the native world map. These are known exclusions rather than merely unknown conditions.

These restrictions apply only to the stage described below. They do **not** mean that the entire questline must be played through the world map.

| In-game quest / event | World-map-only stage | Internal condition(s) |
| --- | --- | --- |
| Henry Morgan’s Offer | When the deadline for the Belize attack expires, process Henry Morgan’s native world-map/deck intervention. | `time_over_Beliz_attack_Map_01`<br>`time_over_Beliz_attack_Map_02` |
| Expedition to Panama | When the deadline for reaching Porto Bello expires, process Henry Morgan’s native world-map/deck intervention. | `PQ8_PanamaTimerOver_01`<br>`PQ8_PanamaTimerOver_02` |
| Secrets of the Pearl Craft | When the first pearl-diver search deadline expires, process the native world-map/deck sequence involving Sharp. | `SeaPearl_FirstTime_Late_01` |
| Pirate’s Offer | When the first generated PGG venture expires, process its overdue result through the native world-map path. | `PGGQuest1_Time2Late_01` |
| Peter Blood prologue — world-map tutorial to Tortuga | Run the guided world-map tutorial and its associated exit/re-entry sequence. This stage intentionally requires the real world map. | `BloodLine_GlobalTutor_WorldmapTutorial`<br>`BloodLine_GlobalTutor_OnExitToMapAgain` |
| Crew mutiny / morale system | Process a pending low-morale mutiny that was deliberately deferred until the player enters the real world map. | `Munity_on_Ship_Map` |
| Cursed Idol | Process the skeletons-at-sea sequence that expects the native world map before moving into its ship/deck scene. | `PDM_Ne_Spryacheshsy` |
| World Map tutorial (system event) | Launch and consume the game’s one-time general world-map tutorial. | `Tut_WorldMap` |

If one of these stages becomes due while using Direct Sail, enter the normal world map and allow the game to process the event there. Direct Sail deliberately leaves the condition pending instead of consuming it incorrectly.

The source audit also identified some quest logic that depends on actual world-map coordinates, proximity or movement rather than a `MapEnter` condition. Synthetic `MapEnter` support cannot replace those systems.

Known examples include:

| Quest or system | Requirement |
| --- | --- |
| Lost Ships City / Justice Island | Use the normal world map for scripted approach or proximity stages involving Justice Island. |
| Royal Jackpot | Use the normal world map for stages which check the player's strategic-map position. |

Other ordinary stages of these quests may still work with Direct Sail. The world map is required when the quest specifically expects one of the native positional or approach triggers.


### Buccaneers DLC compatibility

Direct Sail 1.2 was developed and tested with the **Sea Dogs** DLC installed. It has **not** been developed against or runtime-tested with the **Buccaneers** DLC.

The Buccaneers expansion adds a large story campaign, an evolving buccaneer outpost, a new minor faction and major scripted naval content. Because Direct Sail integrates directly with strategic-map encounters and quest progression, compatibility with those new systems cannot currently be guaranteed.

The most likely risk areas are:

* **New quest world-map dependencies.** Buccaneers may contain new `MapEnter`, strategic-coordinate, proximity, scripted-approach or world-map movement checks that were not present in the 1.2 source audit. Unknown `MapEnter` conditions are deliberately left pending for the native world map rather than being consumed by Direct Sail, which should make new `MapEnter` stages fail safely. Direct strategic-coordinate, proximity, scripted-approach and movement checks still require DLC-specific review.
* **New roaming quest encounter methods.** Direct Sail currently supports the normal trader, warrior and cool-warrior quest encounter paths found in the audited 1.2 source. If Buccaneers introduces new world-map encounter creation methods, custom encounter types or special fleet lifecycle events, those encounters may not be promoted, tracked or handed back correctly.
* **Buccaneer nationality and Watchman presentation.** Direct Sail's Watchman and encounter-description code currently knows the standard Spanish, French, English, Dutch and Pirate nation/flag cases. If Buccaneer ships use a distinct nation/flag identifier rather than an existing French or Pirate identity, their Watchman label or icon may be missing, incorrect or fall back to a generic/unknown presentation.
* **Interface texture collisions.** Direct Sail adds its own Watchman command texture to the sea battle interface. If Buccaneers also adds or reserves the same additional command-texture slot, Watchman graphics could display incorrectly or require a different slot.
* **Special strategic regions or map scales.** This is considered a lower-risk area. Direct Sail defaults unknown regions to the normal world-map scale, so an ordinary new location should work, but any Buccaneers-only sea region using custom coordinates or a non-standard map scale would need explicit support.

The new weapons, unique ships, land locations, settlement upgrades and ordinary character/dialogue content are not expected to conflict with Direct Sail by themselves.

If you play the Buccaneers campaign with Direct Sail, it is advisable to use the **normal world map for important scripted campaign journeys** until the DLC has been audited and tested. If a quest stops progressing, a named quest fleet fails to appear, or a Buccaneer contact displays incorrectly in Watchman, please report the relevant quest stage and include `compile.log`.

### Quest fleet movement

Roaming quest encounters are currently **stationary while represented by Direct Sail**.

For example, if a quest creates a ship travelling from Charlestown to Port Royal, Direct Sail can expose and intercept that quest ship, preserve its timeout and hand it back to the global map, but Direct Sail does not yet simulate its movement along the strategic route.

If the player switches to the normal global map, the encounter returns to the game's native world-map system and moves normally from that point.

Quest fleet movement in Direct Sail is considered a separate future feature.

### Quest coverage

The roaming quest encounter and strategic `MapEnter` implementations have been checked against the game's official 1.2 source and are designed to support the active quest-encounter and compatible `MapEnter` methods found there.

However, not every individual quest and every possible quest stage has been runtime-tested.

Fixed quest ships spawned directly into tactical sea or attached to specific islands and shores use a different system. They are not dependent on the roaming world-map quest encounter implementation described above.

World-map dependencies that do not use `MapEnter`, such as direct strategic-coordinate or proximity checks, require separate compatibility work.

### Persistent tactical encounters

Once an ordinary persistent encounter has already spawned into tactical sea, leaving that sea area without engaging it can still cause the encounter to be lost in some circumstances.

Watchman tracking primarily applies while the encounter remains in the persistent strategic encounter population.

## Testing

The core Direct Sail system, ordinary persistent encounters, Watchman tracking and standard island transitions have been tested extensively.

Roaming quest encounter support has been tested successfully with several representative quest encounters, including trader and pursuing-fleet behaviours, and has also been reviewed against the wider official 1.2 quest source.

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

The current build retains targeted diagnostic logging for encounter generation, quest handoff, lifetime handling, Watchman state, strategic `MapEnter` processing and sea-area transitions.

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

The current GitHub and Steam Workshop builds target **Caribbean Legend: Age of Pirates 1.2**.

Direct Sail's source-patch anchors and relevant world-map and quest integrations have been checked against the official 1.2 game files, and the core system has been runtime-tested on the official 1.2 release. 

The mod was developed and tested with the **Sea Dogs** DLC installed.

The **Buccaneers** DLC is currently **untested and not formally supported**. See [Buccaneers DLC compatibility](#buccaneers-dlc-compatibility) for the areas most likely to require additional compatibility work.

Players remaining on **game version 1.1.2** should use the dedicated [1.1.2 branch](https://github.com/liam0014/CLAoP-DirectSail/tree/1.1.2).

Other mods, DLC content or future game updates that alter the same patched source sections may conflict with Direct Sail or prevent its patches from applying.

No new game is required.
