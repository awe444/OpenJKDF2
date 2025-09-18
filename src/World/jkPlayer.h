#ifndef _JK_PLAYER_H
#define _JK_PLAYER_H

#include "types.h"
#include "globals.h"

#include "Primitives/rdPolyLine.h"
#include "Engine/rdThing.h"
#include "Gameplay/sithPlayer.h"

#define jkPlayer_LoadAutosave_ADDR (0x00404600)
#define jkPlayer_LoadSave_ADDR (0x00404660)
#define jkPlayer_Startup_ADDR (0x00404680)
#define jkPlayer_Shutdown_ADDR (0x004046A0)
#define jkPlayer_Open_ADDR (0x00404740)
#define jkPlayer_Close_ADDR (0x00404750)
#define jkPlayer_InitSaber_ADDR (0x00404760)
#define jkPlayer_InitThings_ADDR (0x00404830)
#define jkPlayer_nullsub_1_ADDR (0x00404900)
#define jkPlayer_CreateConf_ADDR (0x00404910)
#define jkPlayer_WriteConf_ADDR (0x00404D20)
#define jkPlayer_ReadConf_ADDR (0x00404EA0)
#define jkPlayer_SetPovModel_ADDR (0x00405190)
#define jkPlayer_DrawPov_ADDR (0x004051E0)
#define jkPlayer_renderSaberWeaponMesh_ADDR (0x00405520)
#define jkPlayer_renderSaberTwinkle_ADDR (0x00405720)
#define jkPlayer_SetWaggle_ADDR (0x00405930)
#define jkPlayer_VerifyWcharName_ADDR (0x00405980)
#define jkPlayer_VerifyCharName_ADDR (0x00405B30)
#define jkPlayer_SetMpcInfo_ADDR (0x00405B70)
#define jkPlayer_SetPlayerName_ADDR (0x00405C10)
#define jkPlayer_GetMpcInfo_ADDR (0x00405C30)
#define jkPlayer_SetChoice_ADDR (0x00405CC0)
#define jkPlayer_GetChoice_ADDR (0x00405CE0)
#define jkPlayer_CalcAlignment_ADDR (0x00405CF0)
#define jkPlayer_MpcInitBins_ADDR (0x00405E30)
#define jkPlayer_MPCParse_ADDR (0x00405FB0)
#define jkPlayer_MPCWrite_ADDR (0x00406270)
#define jkPlayer_MPCBinWrite_ADDR (0x004063D0)
#define jkPlayer_MPCBinRead_ADDR (0x00406440)
#define jkPlayer_InitForceBins_ADDR (0x004064E0)
#define jkPlayer_GetAlignment_ADDR (0x00406570)
#define jkPlayer_SetAccessiblePowers_ADDR (0x00406860)
#define jkPlayer_ResetPowers_ADDR (0x00406990)
#define jkPlayer_WriteConfSwap_ADDR (0x004069B0)
#define jkPlayer_WriteCutsceneConf_ADDR (0x00406C00)
#define jkPlayer_ReadCutsceneConf_ADDR (0x00406C70)
#define jkPlayer_FixStars_ADDR (0x00406D50)
#define jkPlayer_CalcStarsAlign_ADDR (0x00406FE0)
#define jkPlayer_SetProtectionDeadlysight_ADDR (0x00407040)
#define jkPlayer_DisallowOtherSide_ADDR (0x00407210)
#define jkPlayer_WriteOptionsConf_ADDR (0x00407320)
#define jkPlayer_ReadOptionsConf_ADDR (0x004073C0)
#define jkPlayer_GetJediRank_ADDR (0x004074D0)
#define jkPlayer_SetRank_ADDR (0x004074E0)

#ifdef JKM_DSS
extern jkPlayerInfo jkPlayer_aMotsInfos[64];
extern jkBubbleInfo jkPlayer_aBubbleInfo[64];
extern int jkPlayer_personality;
extern flex_t jkPlayer_aMultiParams[0x100];
#endif

typedef struct sithSurface sithSurface;

enum JKFLAG
{
    JKFLAG_SABERON = 1,
    JKFLAG_SABERDAMAGE = 2,
    JKFLAG_SABEREXTEND = 4,
    JKFLAG_SABERRETRACT = 8,
    JKFLAG_DUALSABERS = 0x10,
    JKFLAG_PERSUASION = 0x20,
    JKFLAG_40 = 0x40,
    JKFLAG_SABERFORCEON = 0x80,
    JKFLAG_100 = 0x100,
};

void jkPlayer_StartupVars();
void jkPlayer_ResetVars();

int jkPlayer_LoadAutosave();
int jkPlayer_LoadSave(char *path);
void jkPlayer_Startup();
void jkPlayer_Shutdown();
void jkPlayer_Open();
void jkPlayer_Close();
void jkPlayer_InitSaber();
void jkPlayer_InitThings();
void jkPlayer_nullsub_1(jkPlayerInfo* unk);
void jkPlayer_CreateConf(wchar_t *name);
void jkPlayer_WriteConf(wchar_t *name);
int jkPlayer_ReadConf(wchar_t *name);
void jkPlayer_SetPovModel(jkPlayerInfo *info, rdModel3 *model);
MATH_FUNC void jkPlayer_DrawPov();
MATH_FUNC void jkPlayer_renderSaberWeaponMesh(sithThing *a1);
MATH_FUNC void jkPlayer_renderSaberTwinkle(sithThing *player);
void jkPlayer_SetWaggle(sithThing *player, rdVector3 *waggleVec, flex_t waggleMag);
int jkPlayer_VerifyWcharName(wchar_t *name);
int jkPlayer_VerifyCharName(char *name);
void jkPlayer_SetMpcInfo(wchar_t *name, char *model, char *soundclass, char *sidemat, char *tipmat);
void jkPlayer_SetPlayerName(wchar_t *name);
int jkPlayer_GetMpcInfo(wchar_t *name, char *model, char *soundclass, char *sidemat, char *tipmat);
void jkPlayer_SetChoice(int amt);
int jkPlayer_GetChoice();
flex_t jkPlayer_CalcAlignment(int isMp);
void jkPlayer_MpcInitBins(sithPlayerInfo* unk);
int jkPlayer_MPCParse(jkPlayerMpcInfo *info, sithPlayerInfo* unk, wchar_t *fname, wchar_t *name, int hasBins);
int jkPlayer_MPCWrite(sithPlayerInfo* unk, wchar_t *mpcName, wchar_t *playerName);
int jkPlayer_MPCBinWrite();
int jkPlayer_MPCBinRead();
void jkPlayer_InitForceBins();
int jkPlayer_GetAlignment();
void jkPlayer_SetAccessiblePowers(int rank);
void jkPlayer_ResetPowers();
int jkPlayer_WriteConfSwap(jkPlayerInfo* unk, int a2, char *a3);
int jkPlayer_WriteCutsceneConf();
int jkPlayer_ReadCutsceneConf();
void jkPlayer_FixStars();
flex_t jkPlayer_CalcStarsAlign();
int jkPlayer_SetProtectionDeadlysight();
void jkPlayer_DisallowOtherSide(int rank);
int jkPlayer_WriteOptionsConf();
int jkPlayer_ReadOptionsConf();
int jkPlayer_GetJediRank();
void jkPlayer_SetRank(int rank);

uint32_t jkPlayer_ChecksumExtra(uint32_t hash); // MOTS added
jkPlayerInfo* jkPlayer_FUN_00404fe0(sithThing *pPlayerThing); // MOTS added
int jkPlayer_SetAmmoMaximums(int classIdx); // MOTS added
void jkPlayer_idkEndLevel(void); // MOTS added
int jkPlayer_SyncForcePowers(int rank,int bIsMulti); // MOTS added

extern int jkPlayer_aMotsFpBins[74];

#ifdef QOL_IMPROVEMENTS
extern int jkPlayer_fov;
extern int jkPlayer_fovIsVertical;
extern int jkPlayer_enableTextureFilter;
extern int jkPlayer_enableOrigAspect;
extern int jkPlayer_enableBloom;
extern int jkPlayer_fpslimit;
extern int jkPlayer_enableVsync;
extern flex_t jkPlayer_ssaaMultiple;
extern int jkPlayer_enableSSAO;
extern flex_t jkPlayer_gamma;
extern flex_t jkPlayer_hudScale;
extern flex_t jkPlayer_crosshairLineWidth;
extern flex_t jkPlayer_crosshairScale;
extern flex_t jkPlayer_canonicalCogTickrate;
extern flex_t jkPlayer_canonicalPhysTickrate;
extern flex_t jkPlayer_sectorThrustScale;  // Scale factor for sector thrust (water current) effects, default 1.0

extern int jkPlayer_bEnableJkgm;
extern int jkPlayer_bEnableTexturePrecache;

extern int jkPlayer_bKeepCorpses;
extern int jkPlayer_bFastMissionText;
extern int jkPlayer_bUseOldPlayerPhysics;

extern int jkPlayer_setCrosshairOnLightsaber;
extern int jkPlayer_setCrosshairOnFist;
extern int jkPlayer_bDisableWeaponWaggle;

extern int jkPlayer_bHasLoadedSettingsOnce;

extern int jkPlayer_bEnableEmissiveTextures;
extern int jkPlayer_bEnableClassicLighting;

#define FOV_MIN (40)
#define FOV_MAX (170)

#define FPS_LIMIT_MIN (0)
#define FPS_LIMIT_MAX (360)
#endif

#ifdef FIXED_TIMESTEP_PHYS
extern int jkPlayer_bJankyPhysics;
#endif


#define HUD_SCALED(x) ((int)((flex_t)(x) * jkPlayer_hudScale)) // FLEXTODO

//static void (*jkPlayer_InitThings)() = (void*)jkPlayer_InitThings_ADDR;
//static int (*jkPlayer_ReadConf)(wchar_t *a1) = (void*)jkPlayer_ReadConf_ADDR;
//static int (*jkPlayer_VerifyWcharName)(wchar_t *a1) = (void*)jkPlayer_VerifyWcharName_ADDR;
//static void (*jkPlayer_SetAccessiblePowers)(int rank) = (void*)jkPlayer_SetAccessiblePowers_ADDR;
//static int (*jkPlayer_SetProtectionDeadlysight)() = (void*)jkPlayer_SetProtectionDeadlysight_ADDR;
//static int (*jkPlayer_GetAlignment)() = (void*)jkPlayer_GetAlignment_ADDR;
//static int (*jkPlayer_GetJediRank)() = (void*)jkPlayer_GetJediRank_ADDR;
//static int (*jkPlayer_DisallowOtherSide)() = (void*)jkPlayer_DisallowOtherSide_ADDR;
//static void (*jkPlayer_SetChoice)(signed int a1) = (void*)jkPlayer_SetChoice_ADDR;
//static double (*jkPlayer_CalcAlignment)(flex_t a1) = (void*)jkPlayer_CalcAlignment_ADDR;
//static void (__cdecl *jkPlayer_renderSaberTwinkle)(sithThing *a1) = (void*)jkPlayer_renderSaberTwinkle_ADDR;

#endif // _JK_PLAYER_H
