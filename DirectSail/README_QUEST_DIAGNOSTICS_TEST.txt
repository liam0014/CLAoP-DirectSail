Quest encounter diagnostics - test notes

Search compile.log for:

DS QUEST QUEUE NEW
    Written immediately when DirectSail first sees a queued quest encounter.

DS QUEST DIAG ===== SNAPSHOT
    Start of the hourly quest encounter snapshot.

DS QUEST DIAG QUEUED
    One line for every quest encounter still waiting in worldMap.addQuestEncounters.

DS QUEST DIAG LIVE ENTRY
    One line for every live persistent quest encounter in worldMap.encounters.
    Includes character ID, display label, encounter type, world-map X/Z,
    nearest known island/town/anchor, position relative to the player, route
    destination where available, remaining lifetime, and DirectSail state.

DS QUEST DIAG ===== END
    Summary count for queued and live quest encounters.

For the Arrow specifically, search compile.log for either:
    Arrow
or its quest character ID if the display name is not present.
