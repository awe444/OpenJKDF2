#ifndef _ENGINE_SITHPHYSICS_H
#define _ENGINE_SITHPHYSICS_H

#include "types.h"
#include "globals.h"

// Global thrust scale factor for water currents
extern flex_t sithPhysics_thrustScale;

#define sithPhysics_FindFloor_ADDR (0x004F5550)
#define sithPhysics_ThingPhysIdk_inlined_ADDR (0x004F5870)
#define sithPhysics_ThingTick_ADDR (0x004F5900)
#define sithPhysics_ThingApplyForce_ADDR (0x004F59B0)
#define sithPhysics_ThingSetLook_ADDR (0x004F5A80)
#define sithPhysics_ApplyDrag_ADDR (0x004F5D50)
#define sithPhysics_LoadThingParams_ADDR (0x004F5EC0)
#define sithPhysics_ThingStop_ADDR (0x004F61A0)
#define sithPhysics_ThingGetInsertOffsetZ_ADDR (0x004F6210)
#define sithPhysics_ThingPhysGeneral_ADDR (0x004F6270)
#define sithPhysics_ThingPhysPlayer_ADDR (0x004F6860)
#define sithPhysics_ThingPhysUnderwater_ADDR (0x004F6D80)
#define sithPhysics_ThingPhysAttached_ADDR (0x004F7430)

MATH_FUNC void sithPhysics_FindFloor(sithThing *pThing, int a3);
MATH_FUNC void sithPhysics_ThingTick(sithThing *pThing, flex_t force);
MATH_FUNC void sithPhysics_ThingApplyForce(sithThing *pThing, rdVector3 *forceVec);
MATH_FUNC void sithPhysics_ThingSetLook(sithThing *pThing, const rdVector3 *look, flex_t a3);
MATH_FUNC void sithPhysics_ApplyDrag(rdVector3 *vec, flex_t drag, flex_t mag, flex_t dragCoef);
MATH_FUNC int sithPhysics_LoadThingParams(stdConffileArg *arg, sithThing *pThing, int param);
MATH_FUNC void sithPhysics_ThingStop(sithThing *pThing);
MATH_FUNC flex_t sithPhysics_ThingGetInsertOffsetZ(sithThing *pThing);
MATH_FUNC void sithPhysics_ThingPhysGeneral(sithThing *pThing, flex_t deltaSeconds);
MATH_FUNC void sithPhysics_ThingPhysPlayer(sithThing *player, flex_t deltaSeconds);
MATH_FUNC void sithPhysics_ThingPhysUnderwater(sithThing *pThing, flex_t deltaSeconds);
MATH_FUNC void sithPhysics_ThingPhysAttached(sithThing *pThing, flex_t deltaSeconds);

void sithPhysics_StartupVars();

//static void (*_sithPhysics_ThingPhysAttached)(sithThing *pThing, flex_t deltaSeconds) = (void*)sithPhysics_ThingPhysAttached_ADDR;

#endif // _ENGINE_SITHPHYSICS_H