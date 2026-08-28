// ============================================================================
// Direct Sail - Watchman
// Beta release-candidate implementation.
//
// Watchman exposes the shared persistent world-map encounter population while
// sailing in Direct Sail. Contacts can be selected and tracked across sea-zone
// transitions, with hourly bearing/range reports.
// ============================================================================

#define DS_WATCHMAN_MAX_CONTACTS             8
#define DS_WATCHMAN_RADIUS_SQ                62500.0
#define DS_WATCHMAN_NEAR_SQ                   8100.0
#define DS_WATCHMAN_MODERATE_SQ              28900.0

#define DS_WATCHMAN_TEXTURE                      5

// User-provided DirectSail Watchman atlas: 16 x 2, 64x64 cells.
// Selected/highlighted row:
#define DS_WATCHMAN_ICON_SPAIN_SEL               0
#define DS_WATCHMAN_ICON_FRANCE_SEL              1
#define DS_WATCHMAN_ICON_ENGLAND_SEL             2
#define DS_WATCHMAN_ICON_HOLLAND_SEL             3
#define DS_WATCHMAN_ICON_PIRATE_SEL              4
#define DS_WATCHMAN_ICON_LOOKOUT_SEL             5

// Normal row:
#define DS_WATCHMAN_ICON_SPAIN                  16
#define DS_WATCHMAN_ICON_FRANCE                 17
#define DS_WATCHMAN_ICON_ENGLAND                18
#define DS_WATCHMAN_ICON_HOLLAND                19
#define DS_WATCHMAN_ICON_PIRATE                 20
#define DS_WATCHMAN_ICON_LOOKOUT                21

// Native list_icons.tga pairs.
#define DS_WATCHMAN_ICON_BATTLE_SEL             33
#define DS_WATCHMAN_ICON_BATTLE                 49
#define DS_WATCHMAN_ICON_SPECIAL_SEL            38
#define DS_WATCHMAN_ICON_SPECIAL                54

// Equivalent to DirSailEnc.c's 2000 tactical-unit Warring pairing radius.
#define DS_WATCHMAN_WARRING_TACTICAL_RADIUS_SQ  4000000.0

// Fixed-size nearest-contact buffer. This avoids relying on the arbitrary
// worldMap.encounters attribute order.
object DSWatchmanSorted[8];

string DirectSail_WatchmanText(string sKey)
{
    int iFile;
    string sText;

    iFile = LanguageOpenFile("DirectSailWatchman.txt");
    if(iFile < 0) return sKey;

    sText = LanguageConvertString(iFile, sKey);
    LanguageCloseFile(iFile);

    return sText;
}

string DirectSail_WatchmanDirection(float fDx, float fDz)
{
    float fAbsX = fDx;
    float fAbsZ = fDz;

    if(fAbsX < 0.0) fAbsX = -fAbsX;
    if(fAbsZ < 0.0) fAbsZ = -fAbsZ;

    if(fAbsZ > fAbsX * 2.0)
    {
        if(fDz >= 0.0) return DirectSail_WatchmanText("DirectionNorth");
        return DirectSail_WatchmanText("DirectionSouth");
    }

    if(fAbsX > fAbsZ * 2.0)
    {
        if(fDx >= 0.0) return DirectSail_WatchmanText("DirectionEast");
        return DirectSail_WatchmanText("DirectionWest");
    }

    if(fDx >= 0.0 && fDz >= 0.0) return DirectSail_WatchmanText("DirectionNorthEast");
    if(fDx >= 0.0 && fDz < 0.0) return DirectSail_WatchmanText("DirectionSouthEast");
    if(fDx < 0.0 && fDz >= 0.0) return DirectSail_WatchmanText("DirectionNorthWest");
    return DirectSail_WatchmanText("DirectionSouthWest");
}

string DirectSail_WatchmanRange(float fDistSq)
{
    if(fDistSq <= DS_WATCHMAN_NEAR_SQ)
        return DirectSail_WatchmanText("RangeNearby");

    if(fDistSq <= DS_WATCHMAN_MODERATE_SQ)
        return DirectSail_WatchmanText("RangeModerate");

    return DirectSail_WatchmanText("RangeDistant");
}

int DirectSail_WatchmanNation(aref rEnc)
{
    if(CheckAttribute(rEnc, "encdata.Nation"))
        return sti(rEnc.encdata.Nation);

    if(CheckAttribute(rEnc, "Nation"))
        return sti(rEnc.Nation);

    return -1;
}

string DirectSail_WatchmanNationTextByID(int iNation)
{
    switch(iNation)
    {
        case SPAIN:   return DirectSail_WatchmanText("NationSpanish"); break;
        case FRANCE:  return DirectSail_WatchmanText("NationFrench"); break;
        case ENGLAND: return DirectSail_WatchmanText("NationEnglish"); break;
        case HOLLAND: return DirectSail_WatchmanText("NationDutch"); break;
        case PIRATE:  return DirectSail_WatchmanText("NationPirate"); break;
    }

    return DirectSail_WatchmanText("NationUnknown");
}

string DirectSail_WatchmanBattleNationText(int iNation)
{
    switch(iNation)
    {
        case SPAIN:   return DirectSail_WatchmanText("BattleNationSpain"); break;
        case FRANCE:  return DirectSail_WatchmanText("BattleNationFrance"); break;
        case ENGLAND: return DirectSail_WatchmanText("BattleNationEngland"); break;
        case HOLLAND: return DirectSail_WatchmanText("BattleNationHolland"); break;
        case PIRATE:  return DirectSail_WatchmanText("BattleNationPirates"); break;
    }

    return DirectSail_WatchmanText("NationUnknown");
}

int DirectSail_WatchmanFlagPicNormal(aref rEnc)
{
    int iNation = DirectSail_WatchmanNation(rEnc);

    switch(iNation)
    {
        case SPAIN:   return DS_WATCHMAN_ICON_SPAIN; break;
        case FRANCE:  return DS_WATCHMAN_ICON_FRANCE; break;
        case ENGLAND: return DS_WATCHMAN_ICON_ENGLAND; break;
        case HOLLAND: return DS_WATCHMAN_ICON_HOLLAND; break;
        case PIRATE:  return DS_WATCHMAN_ICON_PIRATE; break;
    }

    return DS_WATCHMAN_ICON_PIRATE;
}

int DirectSail_WatchmanFlagPicSelected(aref rEnc)
{
    int iNation = DirectSail_WatchmanNation(rEnc);

    switch(iNation)
    {
        case SPAIN:   return DS_WATCHMAN_ICON_SPAIN_SEL; break;
        case FRANCE:  return DS_WATCHMAN_ICON_FRANCE_SEL; break;
        case ENGLAND: return DS_WATCHMAN_ICON_ENGLAND_SEL; break;
        case HOLLAND: return DS_WATCHMAN_ICON_HOLLAND_SEL; break;
        case PIRATE:  return DS_WATCHMAN_ICON_PIRATE_SEL; break;
    }

    return DS_WATCHMAN_ICON_PIRATE_SEL;
}

bool DirectSail_WatchmanIsWarring(aref rEnc)
{
    if(CheckAttribute(rEnc, "type") && rEnc.type == "Warring")
        return true;

    if(CheckAttribute(rEnc, "dsGeneratedKind") && rEnc.dsGeneratedKind == "Warring")
        return true;

    return false;
}

int DirectSail_WatchmanSpecialRealType(aref rEnc)
{
    if(!CheckAttribute(rEnc, "encdata.Type")) return -1;
    if(rEnc.encdata.Type != "special") return -1;
    if(!CheckAttribute(rEnc, "encdata.RealEncounterType")) return -1;

    return sti(rEnc.encdata.RealEncounterType);
}

bool DirectSail_WatchmanIsSpecialContact(aref rEnc)
{
    int iType = DirectSail_WatchmanSpecialRealType(rEnc);

    if(iType == ENCOUNTER_TYPE_BARREL) return true;
    if(iType == ENCOUNTER_TYPE_BOAT) return true;

    return false;
}

bool DirectSail_WatchmanIsContact(aref rEnc)
{
    int iSpecial;

    if(!CheckAttribute(rEnc, "type")) return false;
    if(rEnc.type == "Storm") return false;
    if(CheckAttribute(rEnc, "needDelete")) return false;
    if(CheckAttribute(rEnc, "dirSailEnc")) return false;
    if(CheckAttribute(rEnc, "quest")) return false;
    if(!CheckAttribute(rEnc, "encdata")) return false;
    if(!CheckAttribute(rEnc, "x") || !CheckAttribute(rEnc, "z")) return false;

    iSpecial = DirectSail_WatchmanSpecialRealType(rEnc);
    if(iSpecial >= 0)
    {
        if(iSpecial == ENCOUNTER_TYPE_BARREL) return true;
        if(iSpecial == ENCOUNTER_TYPE_BOAT) return true;
        return false;
    }

    // Warring + Attacked represent one battle opportunity.
    if(rEnc.type == "Attacked") return false;

    if(rEnc.type == "Merchant") return true;
    if(rEnc.type == "Warring") return true;
    if(rEnc.type == "Follow") return true;

    return false;
}

string DirectSail_WatchmanFindWarringPartnerID(aref rEnc)
{
    aref WME;
    aref rCandidate;

    int i;
    int iNum;
    int iScale;

    float fDx;
    float fDz;
    float fDistSq;
    float fBestDistSq;
    float fPairMaxSq;

    string sID;
    string sCandidateID;
    string sBestID;

    if(!DirectSail_WatchmanIsWarring(rEnc)) return "";

    if(!CheckAttribute(&worldMap, "island")) return "";

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0) return "";

    fPairMaxSq = DS_WATCHMAN_WARRING_TACTICAL_RADIUS_SQ /
                 makefloat(iScale * iScale);

    fBestDistSq = fPairMaxSq + 1.0;
    sBestID = "";
    sID = GetAttributeName(rEnc);

    makearef(WME, worldMap.encounters);
    iNum = GetAttributesNum(WME);

    for(i = 0; i < iNum; i++)
    {
        rCandidate = GetAttributeN(WME, i);
        sCandidateID = GetAttributeName(rCandidate);

        if(sCandidateID == sID) continue;
        if(!CheckAttribute(rCandidate, "type")) continue;
        if(rCandidate.type != "Attacked") continue;
        if(CheckAttribute(rCandidate, "needDelete")) continue;
        if(!CheckAttribute(rCandidate, "encdata")) continue;
        if(!CheckAttribute(rCandidate, "x") || !CheckAttribute(rCandidate, "z")) continue;

        fDx = stf(rCandidate.x) - stf(rEnc.x);
        fDz = stf(rCandidate.z) - stf(rEnc.z);
        fDistSq = fDx * fDx + fDz * fDz;

        if(fDistSq > fPairMaxSq) continue;

        if(fDistSq < fBestDistSq)
        {
            fBestDistSq = fDistSq;
            sBestID = sCandidateID;
        }
    }

    return sBestID;
}

string DirectSail_WatchmanTypeText(aref rEnc)
{
    int iType;

    if(DirectSail_WatchmanIsWarring(rEnc))
        return DirectSail_WatchmanText("TypeBattle");

    iType = DirectSail_WatchmanSpecialRealType(rEnc);

    if(iType == ENCOUNTER_TYPE_BARREL)
        return DirectSail_WatchmanText("TypeFlotsam");

    if(iType == ENCOUNTER_TYPE_BOAT)
        return DirectSail_WatchmanText("TypeStrandedVessel");

    if(CheckAttribute(rEnc, "encdata.RealEncounterType"))
    {
        iType = sti(rEnc.encdata.RealEncounterType);

        if(iType <= ENCOUNTER_TYPE_MERCHANT_LARGE)
            return DirectSail_WatchmanText("TypeMerchants");

        if(iType >= ENCOUNTER_TYPE_MERCHANT_GUARD_SMALL &&
           iType <= ENCOUNTER_TYPE_MERCHANT_GUARD_LARGE)
            return DirectSail_WatchmanText("TypeMerchantConvoy");

        if(iType >= ENCOUNTER_TYPE_ESCORT_SMALL &&
           iType <= ENCOUNTER_TYPE_ESCORT_LARGE)
            return DirectSail_WatchmanText("TypeTradeCaravan");

        if(iType >= ENCOUNTER_TYPE_PATROL_SMALL &&
           iType <= ENCOUNTER_TYPE_PATROL_LARGE)
            return DirectSail_WatchmanText("TypePatrol");

        if(iType >= ENCOUNTER_TYPE_PIRATE_SMALL &&
           iType <= ENCOUNTER_TYPE_PIRATE_LARGE)
            return DirectSail_WatchmanText("TypePirates");

        if(iType >= ENCOUNTER_TYPE_SQUADRON &&
           iType <= ENCOUNTER_TYPE_ARMADA)
            return DirectSail_WatchmanText("TypeNavalSquadron");

        if(iType == ENCOUNTER_TYPE_PUNITIVE_SQUADRON)
            return DirectSail_WatchmanText("TypePunitiveExpedition");
    }

    // Direct Sail metadata remains a useful fallback for generated shells.
    if(CheckAttribute(rEnc, "dsGeneratedKind"))
    {
        if(rEnc.dsGeneratedKind == "Merchant")
            return DirectSail_WatchmanText("TypeMerchants");

        if(rEnc.dsGeneratedKind == "Follow")
        {
            if(CheckAttribute(rEnc, "type") && rEnc.type == "Follow")
                return DirectSail_WatchmanText("TypePatrol");

            return DirectSail_WatchmanText("TypeFleet");
        }
    }

    if(CheckAttribute(rEnc, "type") && rEnc.type == "Merchant")
        return DirectSail_WatchmanText("TypeMerchants");

    if(CheckAttribute(rEnc, "type") && rEnc.type == "Follow")
        return DirectSail_WatchmanText("TypePatrol");

    return DirectSail_WatchmanText("TypeFleet");
}

string DirectSail_WatchmanContactLabel(aref rEnc)
{
    string sPartnerID;
    string sPartnerPath;
    aref rPartner;

    int iNation1;
    int iNation2;
    int iType;

    string sType;

    iType = DirectSail_WatchmanSpecialRealType(rEnc);

    if(iType == ENCOUNTER_TYPE_BARREL)
        return DirectSail_WatchmanText("TypeFlotsam");

    if(iType == ENCOUNTER_TYPE_BOAT)
        return DirectSail_WatchmanText("TypeStrandedVessel");

    if(DirectSail_WatchmanIsWarring(rEnc))
    {
        iNation1 = DirectSail_WatchmanNation(rEnc);
        sPartnerID = DirectSail_WatchmanFindWarringPartnerID(rEnc);

        if(sPartnerID != "")
        {
            sPartnerPath = "encounters." + sPartnerID;
            makearef(rPartner, worldMap.(sPartnerPath));
            iNation2 = DirectSail_WatchmanNation(rPartner);

            return DirectSail_WatchmanBattleNationText(iNation1) + " " +
                   DirectSail_WatchmanText("Versus") + " " +
                   DirectSail_WatchmanBattleNationText(iNation2) + " - " +
                   DirectSail_WatchmanText("TypeBattle");
        }

        return DirectSail_WatchmanBattleNationText(iNation1) + " - " +
               DirectSail_WatchmanText("TypeBattle");
    }

    sType = DirectSail_WatchmanTypeText(rEnc);

    // "Pirate pirates" is redundant; for pirate-type encounters the encounter
    // type already conveys the useful information.
    if(CheckAttribute(rEnc, "encdata.RealEncounterType"))
    {
        iType = sti(rEnc.encdata.RealEncounterType);

        if(iType >= ENCOUNTER_TYPE_PIRATE_SMALL &&
           iType <= ENCOUNTER_TYPE_PIRATE_LARGE)
            return sType;
    }

    return DirectSail_WatchmanNationTextByID(
               DirectSail_WatchmanNation(rEnc)
           ) + " " + sType;
}

int DirectSail_WatchmanContactTexture(aref rEnc)
{
    if(DirectSail_WatchmanIsWarring(rEnc))
        return BI_ICONS_TEXTURE_COMMAND;

    if(DirectSail_WatchmanIsSpecialContact(rEnc))
        return BI_ICONS_TEXTURE_COMMAND;

    return DS_WATCHMAN_TEXTURE;
}

int DirectSail_WatchmanContactPicNormal(aref rEnc)
{
    if(DirectSail_WatchmanIsWarring(rEnc))
        return DS_WATCHMAN_ICON_BATTLE;

    if(DirectSail_WatchmanIsSpecialContact(rEnc))
        return DS_WATCHMAN_ICON_SPECIAL;

    return DirectSail_WatchmanFlagPicNormal(rEnc);
}

int DirectSail_WatchmanContactPicSelected(aref rEnc)
{
    if(DirectSail_WatchmanIsWarring(rEnc))
        return DS_WATCHMAN_ICON_BATTLE_SEL;

    if(DirectSail_WatchmanIsSpecialContact(rEnc))
        return DS_WATCHMAN_ICON_SPECIAL_SEL;

    return DirectSail_WatchmanFlagPicSelected(rEnc);
}

bool DirectSail_WatchmanBetaDiagnosticContact(aref rEnc)
{
    if(DirectSail_WatchmanIsWarring(rEnc)) return true;
    if(DirectSail_WatchmanIsSpecialContact(rEnc)) return true;

    return false;
}

void DirectSail_WatchmanTraceBetaContact(aref rEnc, float fDistSq)
{
    string sID;
    string sPartnerID;
    string sPartnerPath;

    int iRealType;
    int iNation1;
    int iNation2;

    aref rPartner;

    sID = GetAttributeName(rEnc);

    if(DirectSail_WatchmanIsWarring(rEnc))
    {
        sPartnerID = DirectSail_WatchmanFindWarringPartnerID(rEnc);
        iNation1 = DirectSail_WatchmanNation(rEnc);
        iNation2 = -1;

        if(sPartnerID != "")
        {
            sPartnerPath = "encounters." + sPartnerID;
            makearef(rPartner, worldMap.(sPartnerPath));
            iNation2 = DirectSail_WatchmanNation(rPartner);
        }

        trace("DS WATCHMAN BATTLE: id=" + sID +
              " partner=" + sPartnerID +
              " nation1=" + iNation1 +
              " nation2=" + iNation2 +
              " distSq=" + fDistSq);
        return;
    }

    if(DirectSail_WatchmanIsSpecialContact(rEnc))
    {
        iRealType = DirectSail_WatchmanSpecialRealType(rEnc);

        trace("DS WATCHMAN SPECIAL: id=" + sID +
              " realType=" + iRealType +
              " label=" + DirectSail_WatchmanContactLabel(rEnc) +
              " distSq=" + fDistSq);
    }
}

void DirectSail_WatchmanBuildDirectionIcons()
{
    DeleteAttribute(&BattleInterface, "UserIcons");

    BattleInterface.UserIcons.ui1.enable = true;
    BattleInterface.UserIcons.ui1.selpic = 96;
    BattleInterface.UserIcons.ui1.pic = 112;
    BattleInterface.UserIcons.ui1.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui1.name = "dir_n";
    BattleInterface.UserIcons.ui1.note = LanguageConvertString(iCommandsFile, "sea_SailN");

    BattleInterface.UserIcons.ui2.enable = true;
    BattleInterface.UserIcons.ui2.selpic = 97;
    BattleInterface.UserIcons.ui2.pic = 113;
    BattleInterface.UserIcons.ui2.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui2.name = "dir_ne";
    BattleInterface.UserIcons.ui2.note = LanguageConvertString(iCommandsFile, "sea_SailNE");

    BattleInterface.UserIcons.ui3.enable = true;
    BattleInterface.UserIcons.ui3.selpic = 98;
    BattleInterface.UserIcons.ui3.pic = 114;
    BattleInterface.UserIcons.ui3.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui3.name = "dir_e";
    BattleInterface.UserIcons.ui3.note = LanguageConvertString(iCommandsFile, "sea_SailE");

    BattleInterface.UserIcons.ui4.enable = true;
    BattleInterface.UserIcons.ui4.selpic = 99;
    BattleInterface.UserIcons.ui4.pic = 115;
    BattleInterface.UserIcons.ui4.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui4.name = "dir_se";
    BattleInterface.UserIcons.ui4.note = LanguageConvertString(iCommandsFile, "sea_SailSE");

    BattleInterface.UserIcons.ui5.enable = true;
    BattleInterface.UserIcons.ui5.selpic = 100;
    BattleInterface.UserIcons.ui5.pic = 116;
    BattleInterface.UserIcons.ui5.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui5.name = "dir_s";
    BattleInterface.UserIcons.ui5.note = LanguageConvertString(iCommandsFile, "sea_SailS");

    BattleInterface.UserIcons.ui6.enable = true;
    BattleInterface.UserIcons.ui6.selpic = 101;
    BattleInterface.UserIcons.ui6.pic = 117;
    BattleInterface.UserIcons.ui6.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui6.name = "dir_sw";
    BattleInterface.UserIcons.ui6.note = LanguageConvertString(iCommandsFile, "sea_SailSW");

    BattleInterface.UserIcons.ui7.enable = true;
    BattleInterface.UserIcons.ui7.selpic = 102;
    BattleInterface.UserIcons.ui7.pic = 118;
    BattleInterface.UserIcons.ui7.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui7.name = "dir_w";
    BattleInterface.UserIcons.ui7.note = LanguageConvertString(iCommandsFile, "sea_SailW");

    BattleInterface.UserIcons.ui8.enable = true;
    BattleInterface.UserIcons.ui8.selpic = 103;
    BattleInterface.UserIcons.ui8.pic = 119;
    BattleInterface.UserIcons.ui8.tex = BI_ICONS_TEXTURE_COMMAND;
    BattleInterface.UserIcons.ui8.name = "dir_nw";
    BattleInterface.UserIcons.ui8.note = LanguageConvertString(iCommandsFile, "sea_SailNW");
}

void DirectSail_WatchmanInsertSorted(string sID, float fDistSq, int iLimit)
{
    int i;
    int j;

    for(i = 0; i < iLimit; i++)
    {
        if(DSWatchmanSorted[i].id == "" ||
           fDistSq < stf(DSWatchmanSorted[i].dist))
        {
            for(j = iLimit - 1; j > i; j--)
            {
                DSWatchmanSorted[j].id = DSWatchmanSorted[j - 1].id;
                DSWatchmanSorted[j].dist = DSWatchmanSorted[j - 1].dist;
            }

            DSWatchmanSorted[i].id = sID;
            DSWatchmanSorted[i].dist = fDistSq;
            return;
        }
    }
}

int DirectSail_WatchmanBuildContacts()
{
    aref WME;
    aref rEnc;

    int i;
    int iNum;
    int iScale;
    int iContactLimit;
    int iContacts;
    int iMenuItems;
    int iPic;
    int iSelPic;
    int iTex;

    float fPlayerX;
    float fPlayerZ;
    float fDx;
    float fDz;
    float fDistSq;

    bool bTracking;

    string sID;
    string sPath;
    string sUI;
    string sNote;

    DeleteAttribute(&BattleInterface, "UserIcons");

    bTracking = CheckAttribute(pchar, "DirectSail.Watchman.Target");

    iContactLimit = DS_WATCHMAN_MAX_CONTACTS;
    if(bTracking) iContactLimit = DS_WATCHMAN_MAX_CONTACTS - 1;

    for(i = 0; i < DS_WATCHMAN_MAX_CONTACTS; i++)
    {
        DSWatchmanSorted[i].id = "";
        DSWatchmanSorted[i].dist = 2500000000.0;
    }

    if(!CheckAttribute(&worldMap, "island") ||
       !CheckAttribute(&worldMap, "zeroX") ||
       !CheckAttribute(&worldMap, "zeroZ") ||
       !CheckAttribute(pchar, "Ship.Pos.x") ||
       !CheckAttribute(pchar, "Ship.Pos.z"))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("UnablePosition")
        );
        return 0;
    }

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0)
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("UnablePosition")
        );
        return 0;
    }

    fPlayerX = stf(worldMap.zeroX) +
               stf(pchar.Ship.Pos.x) / makefloat(iScale);

    fPlayerZ = stf(worldMap.zeroZ) +
               stf(pchar.Ship.Pos.z) / makefloat(iScale);

    if(!CheckAttribute(&worldMap, "encounters"))
    {
        if(!bTracking)
        {
            Log_SetStringToLog(
                DirectSail_WatchmanText("Watchman") + ": " +
                DirectSail_WatchmanText("NothingSpotted")
            );
        }
        return 0;
    }

    makearef(WME, worldMap.encounters);
    iNum = GetAttributesNum(WME);

    // First pass: collect only the nearest eligible contacts.
    for(i = 0; i < iNum; i++)
    {
        rEnc = GetAttributeN(WME, i);

        if(!DirectSail_WatchmanIsContact(rEnc)) continue;

        fDx = stf(rEnc.x) - fPlayerX;
        fDz = stf(rEnc.z) - fPlayerZ;
        fDistSq = fDx * fDx + fDz * fDz;

        if(fDistSq > DS_WATCHMAN_RADIUS_SQ) continue;

        sID = GetAttributeName(rEnc);
        DirectSail_WatchmanInsertSorted(sID, fDistSq, iContactLimit);
    }

    // Second pass: build UserIcons nearest-first.
    iContacts = 0;

    for(i = 0; i < iContactLimit; i++)
    {
        sID = DSWatchmanSorted[i].id;
        if(sID == "") break;

        sPath = "encounters." + sID;
        if(!CheckAttribute(&worldMap, sPath)) continue;

        makearef(rEnc, worldMap.(sPath));

        fDistSq = stf(DSWatchmanSorted[i].dist);
        fDx = stf(rEnc.x) - fPlayerX;
        fDz = stf(rEnc.z) - fPlayerZ;

        sUI = "ui" + (iContacts + 1);

        iPic = DirectSail_WatchmanContactPicNormal(rEnc);
        iSelPic = DirectSail_WatchmanContactPicSelected(rEnc);
        iTex = DirectSail_WatchmanContactTexture(rEnc);

        sNote = DirectSail_WatchmanContactLabel(rEnc) + " - " +
                DirectSail_WatchmanDirection(fDx, fDz) + " - " +
                DirectSail_WatchmanRange(fDistSq);

        BattleInterface.UserIcons.(sUI).enable = true;
        BattleInterface.UserIcons.(sUI).selpic = iSelPic;
        BattleInterface.UserIcons.(sUI).pic = iPic;
        BattleInterface.UserIcons.(sUI).tex = iTex;
        BattleInterface.UserIcons.(sUI).name = sID;
        BattleInterface.UserIcons.(sUI).note = sNote;

        if(DirectSail_WatchmanBetaDiagnosticContact(rEnc))
            DirectSail_WatchmanTraceBetaContact(rEnc, fDistSq);

        iContacts++;
    }

    iMenuItems = iContacts;

    if(bTracking)
    {
        sUI = "ui" + (iMenuItems + 1);

        BattleInterface.UserIcons.(sUI).enable = true;
        BattleInterface.UserIcons.(sUI).selpic = DS_WATCHMAN_ICON_LOOKOUT_SEL;
        BattleInterface.UserIcons.(sUI).pic = DS_WATCHMAN_ICON_LOOKOUT;
        BattleInterface.UserIcons.(sUI).tex = DS_WATCHMAN_TEXTURE;
        BattleInterface.UserIcons.(sUI).name = "DS_WATCHMAN_STOP";
        BattleInterface.UserIcons.(sUI).note =
            DirectSail_WatchmanText("StopTracking");

        iMenuItems++;
    }

    if(iMenuItems == 0)
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("NothingSpotted")
        );

        DirectSail_WatchmanBuildDirectionIcons();
    }

    return iMenuItems;
}

void DirectSail_WatchmanRestoreTargetTimestamp(aref rEnc)
{
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalSec"))
        rEnc.sec = pchar.DirectSail.Watchman.OriginalSec;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalMin"))
        rEnc.min = pchar.DirectSail.Watchman.OriginalMin;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalHour"))
        rEnc.hour = pchar.DirectSail.Watchman.OriginalHour;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalDay"))
        rEnc.day = pchar.DirectSail.Watchman.OriginalDay;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalMonth"))
        rEnc.month = pchar.DirectSail.Watchman.OriginalMonth;
    if(CheckAttribute(pchar, "DirectSail.Watchman.OriginalYear"))
        rEnc.year = pchar.DirectSail.Watchman.OriginalYear;
}

void DirectSail_WatchmanRestorePartnerTimestamp(aref rEnc)
{
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalSec"))
        rEnc.sec = pchar.DirectSail.Watchman.PartnerOriginalSec;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalMin"))
        rEnc.min = pchar.DirectSail.Watchman.PartnerOriginalMin;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalHour"))
        rEnc.hour = pchar.DirectSail.Watchman.PartnerOriginalHour;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalDay"))
        rEnc.day = pchar.DirectSail.Watchman.PartnerOriginalDay;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalMonth"))
        rEnc.month = pchar.DirectSail.Watchman.PartnerOriginalMonth;
    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerOriginalYear"))
        rEnc.year = pchar.DirectSail.Watchman.PartnerOriginalYear;
}

void DirectSail_WatchmanReleaseProtection()
{
    string sID;
    string sPath;
    string sPartnerID;
    string sPartnerPath;

    aref rEnc;
    aref rPartner;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    sPath = "encounters." + sID;

    if(CheckAttribute(pchar, "DirectSail.Watchman.Protected") &&
       CheckAttribute(&worldMap, sPath))
    {
        makearef(rEnc, worldMap.(sPath));

        if(!CheckAttribute(rEnc, "needDelete"))
            DirectSail_WatchmanRestoreTargetTimestamp(rEnc);
    }

    if(CheckAttribute(pchar, "DirectSail.Watchman.PartnerProtected") &&
       CheckAttribute(pchar, "DirectSail.Watchman.PartnerID"))
    {
        sPartnerID = pchar.DirectSail.Watchman.PartnerID;
        sPartnerPath = "encounters." + sPartnerID;

        if(CheckAttribute(&worldMap, sPartnerPath))
        {
            makearef(rPartner, worldMap.(sPartnerPath));

            if(!CheckAttribute(rPartner, "needDelete"))
                DirectSail_WatchmanRestorePartnerTimestamp(rPartner);
        }
    }
}

void DirectSail_WatchmanStoreTargetTimestamp(aref rEnc)
{
    if(CheckAttribute(rEnc, "sec"))
        pchar.DirectSail.Watchman.OriginalSec = rEnc.sec;
    if(CheckAttribute(rEnc, "min"))
        pchar.DirectSail.Watchman.OriginalMin = rEnc.min;
    if(CheckAttribute(rEnc, "hour"))
        pchar.DirectSail.Watchman.OriginalHour = rEnc.hour;
    if(CheckAttribute(rEnc, "day"))
        pchar.DirectSail.Watchman.OriginalDay = rEnc.day;
    if(CheckAttribute(rEnc, "month"))
        pchar.DirectSail.Watchman.OriginalMonth = rEnc.month;
    if(CheckAttribute(rEnc, "year"))
        pchar.DirectSail.Watchman.OriginalYear = rEnc.year;

    pchar.DirectSail.Watchman.Protected = true;
}

void DirectSail_WatchmanStorePartnerTimestamp(string sPartnerID, aref rEnc)
{
    pchar.DirectSail.Watchman.PartnerID = sPartnerID;

    if(CheckAttribute(rEnc, "sec"))
        pchar.DirectSail.Watchman.PartnerOriginalSec = rEnc.sec;
    if(CheckAttribute(rEnc, "min"))
        pchar.DirectSail.Watchman.PartnerOriginalMin = rEnc.min;
    if(CheckAttribute(rEnc, "hour"))
        pchar.DirectSail.Watchman.PartnerOriginalHour = rEnc.hour;
    if(CheckAttribute(rEnc, "day"))
        pchar.DirectSail.Watchman.PartnerOriginalDay = rEnc.day;
    if(CheckAttribute(rEnc, "month"))
        pchar.DirectSail.Watchman.PartnerOriginalMonth = rEnc.month;
    if(CheckAttribute(rEnc, "year"))
        pchar.DirectSail.Watchman.PartnerOriginalYear = rEnc.year;

    pchar.DirectSail.Watchman.PartnerProtected = true;
}

void DirectSail_WatchmanRefreshEncounterTimestamp(aref rEnc)
{
    rEnc.sec = MakeInt(GetSecond());
    rEnc.min = MakeInt(GetMinute());
    rEnc.hour = MakeInt(GetHour());
    rEnc.day = GetDataDay();
    rEnc.month = GetDataMonth();
    rEnc.year = GetDataYear();
}

void DirectSail_WatchmanRefreshProtection()
{
    string sID;
    string sPath;
    string sPartnerID;
    string sPartnerPath;

    aref rEnc;
    aref rPartner;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    sPath = "encounters." + sID;

    if(!CheckAttribute(&worldMap, sPath)) return;

    makearef(rEnc, worldMap.(sPath));

    if(CheckAttribute(rEnc, "dsGenerated") &&
       !CheckAttribute(rEnc, "needDelete"))
    {
        if(!CheckAttribute(pchar, "DirectSail.Watchman.Protected"))
            DirectSail_WatchmanStoreTargetTimestamp(rEnc);

        DirectSail_WatchmanRefreshEncounterTimestamp(rEnc);
    }

    // A generated Warring encounter needs its paired Attacked half preserved
    // as well; otherwise the target could survive cleanup while its opponent
    // expires before interception.
    if(DirectSail_WatchmanIsWarring(rEnc))
    {
        sPartnerID = DirectSail_WatchmanFindWarringPartnerID(rEnc);

        if(sPartnerID != "")
        {
            sPartnerPath = "encounters." + sPartnerID;

            if(CheckAttribute(&worldMap, sPartnerPath))
            {
                makearef(rPartner, worldMap.(sPartnerPath));

                if(CheckAttribute(rPartner, "dsGenerated") &&
                   !CheckAttribute(rPartner, "needDelete"))
                {
                    if(!CheckAttribute(pchar, "DirectSail.Watchman.PartnerProtected"))
                        DirectSail_WatchmanStorePartnerTimestamp(sPartnerID, rPartner);

                    DirectSail_WatchmanRefreshEncounterTimestamp(rPartner);
                }
            }
        }
    }
}

void DirectSail_WatchmanStopTracking(bool bNotify)
{
    if(!CheckAttribute(pchar, "DirectSail.Watchman")) return;

    DirectSail_WatchmanReleaseProtection();

    if(bNotify)
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("TrackingStopped")
        );
    }

    trace("DS WATCHMAN STOP");
    DeleteAttribute(pchar, "DirectSail.Watchman");
}

void DirectSail_WatchmanCompleteTracking(string sReason)
{
    if(!CheckAttribute(pchar, "DirectSail.Watchman")) return;

    // Successful interception consumes the target (and both halves of a
    // Warring pair), so protected timestamps are intentionally not restored.
    trace("DS WATCHMAN COMPLETE: reason=" + sReason);
    DeleteAttribute(pchar, "DirectSail.Watchman");
}

void DirectSail_WatchmanReportTarget(bool bInitial)
{
    string sID;
    string sPath;
    string sPrefix;
    string sLabel;
    string sDirection;
    string sRange;

    aref rEnc;

    int iScale;

    float fPlayerX;
    float fPlayerZ;
    float fDx;
    float fDz;
    float fDistSq;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    if(sID == "") return;

    sPath = "encounters." + sID;

    if(!CheckAttribute(&worldMap, sPath))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("ContactLost")
        );

        trace("DS WATCHMAN LOST: id=" + sID + " reason=missing");
        DirectSail_WatchmanStopTracking(false);
        return;
    }

    makearef(rEnc, worldMap.(sPath));

    if(CheckAttribute(rEnc, "needDelete"))
    {
        DirectSail_WatchmanCompleteTracking("needDelete");
        return;
    }

    if(!CheckAttribute(rEnc, "x") || !CheckAttribute(rEnc, "z"))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("ContactLost")
        );

        trace("DS WATCHMAN LOST: id=" + sID + " reason=no-position");
        DirectSail_WatchmanStopTracking(false);
        return;
    }

    if(!CheckAttribute(&worldMap, "island") ||
       !CheckAttribute(&worldMap, "zeroX") ||
       !CheckAttribute(&worldMap, "zeroZ") ||
       !CheckAttribute(pchar, "Ship.Pos.x") ||
       !CheckAttribute(pchar, "Ship.Pos.z"))
    {
        return;
    }

    iScale = DirectSail_GetIslandScale(worldMap.island);
    if(iScale <= 0) return;

    fPlayerX = stf(worldMap.zeroX) +
               stf(pchar.Ship.Pos.x) / makefloat(iScale);

    fPlayerZ = stf(worldMap.zeroZ) +
               stf(pchar.Ship.Pos.z) / makefloat(iScale);

    fDx = stf(rEnc.x) - fPlayerX;
    fDz = stf(rEnc.z) - fPlayerZ;
    fDistSq = fDx * fDx + fDz * fDz;

    sLabel = DirectSail_WatchmanContactLabel(rEnc);
    sDirection = DirectSail_WatchmanDirection(fDx, fDz);
    sRange = DirectSail_WatchmanRange(fDistSq);

    sPrefix = DirectSail_WatchmanText("Watchman") + ": ";
    if(bInitial)
    {
        sPrefix = DirectSail_WatchmanText("Watchman") + ": " +
                  DirectSail_WatchmanText("Tracking") + " ";
    }

    Log_SetStringToLog(
        sPrefix + sLabel + " - " + sDirection + " - " + sRange + "."
    );

    // Keep detailed hourly traces only for the beta's harder-to-test contact
    // classes; ordinary fleet tracking no longer floods compile.log.
    if(DirectSail_WatchmanBetaDiagnosticContact(rEnc))
    {
        trace("DS WATCHMAN BETA TRACK: id=" + sID +
              " label=" + sLabel +
              " direction=" + sDirection +
              " range=" + sRange +
              " distSq=" + fDistSq);
    }

    DirectSail_WatchmanRefreshProtection();
}

void DirectSail_WatchmanSelect(string sID)
{
    string sPath;
    aref rEnc;

    if(sID == "")
    {
        DirectSail_WatchmanBuildDirectionIcons();
        return;
    }

    if(sID == "DS_WATCHMAN_STOP")
    {
        DirectSail_WatchmanStopTracking(true);
        DirectSail_WatchmanBuildDirectionIcons();
        return;
    }

    sPath = "encounters." + sID;

    if(!CheckAttribute(&worldMap, sPath))
    {
        Log_SetStringToLog(
            DirectSail_WatchmanText("Watchman") + ": " +
            DirectSail_WatchmanText("ContactLost")
        );

        trace("DS WATCHMAN SELECT: missing id=" + sID);
        DirectSail_WatchmanBuildDirectionIcons();
        return;
    }

    if(CheckAttribute(pchar, "DirectSail.Watchman.Target"))
        DirectSail_WatchmanStopTracking(false);

    pchar.DirectSail.Watchman.Target = sID;
    pchar.DirectSail.Watchman.LastDay = GetDataDay();
    pchar.DirectSail.Watchman.LastHour = MakeInt(GetHour());

    makearef(rEnc, worldMap.(sPath));

    trace("DS WATCHMAN TRACK: id=" + sID +
          " label=" + DirectSail_WatchmanContactLabel(rEnc));

    DirectSail_WatchmanRefreshProtection();
    DirectSail_WatchmanReportTarget(true);
    DirectSail_WatchmanBuildDirectionIcons();
}

void DirectSail_WatchmanHourlyUpdate()
{
    string sID;
    string sPath;

    aref rEnc;

    int iDay;
    int iHour;

    if(!CheckAttribute(pchar, "DirectSail.Watchman.Target")) return;

    sID = pchar.DirectSail.Watchman.Target;
    sPath = "encounters." + sID;

    // needDelete is set when the persistent encounter has successfully become
    // the tactical encounter in front of the player. Clear silently.
    if(CheckAttribute(&worldMap, sPath))
    {
        makearef(rEnc, worldMap.(sPath));

        if(CheckAttribute(rEnc, "needDelete"))
        {
            DirectSail_WatchmanCompleteTracking("encountered");
            return;
        }
    }

    iDay = GetDataDay();
    iHour = MakeInt(GetHour());

    if(!CheckAttribute(pchar, "DirectSail.Watchman.LastDay") ||
       !CheckAttribute(pchar, "DirectSail.Watchman.LastHour"))
    {
        pchar.DirectSail.Watchman.LastDay = iDay;
        pchar.DirectSail.Watchman.LastHour = iHour;
        DirectSail_WatchmanRefreshProtection();
        return;
    }

    if(sti(pchar.DirectSail.Watchman.LastDay) == iDay &&
       sti(pchar.DirectSail.Watchman.LastHour) == iHour)
    {
        return;
    }

    pchar.DirectSail.Watchman.LastDay = iDay;
    pchar.DirectSail.Watchman.LastHour = iHour;

    DirectSail_WatchmanReportTarget(false);
}
