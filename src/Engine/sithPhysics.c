#include "sithPhysics.h"

#include "General/stdMath.h"
#include "Primitives/rdMath.h"
#include "Engine/sithCollision.h"
#include "World/sithSurface.h"
#include "World/sithThing.h"
#include "World/sithSector.h"
#include "World/jkPlayer.h"
#include "jk.h"

void sithPhysics_FindFloor(sithThing *pThing, int a3)
{
    int v4; // ecx
    sithCollisionSearchEntry *v5; // eax
    flex_d_t v8; // st7
    flex_d_t v9; // st7
    sithCollisionSearchEntry *i; // esi
    sithThing *v11; // edi
    rdFace *v12; // eax
    int searchFlags; // [esp+10h] [ebp-20h]
    flex_t range; // [esp+14h] [ebp-1Ch]
    rdVector3 direction; // [esp+18h] [ebp-18h] BYREF
    rdVector3 a1; // [esp+24h] [ebp-Ch] BYREF
    flex_t thinga; // [esp+34h] [ebp+4h]

    // Added: noclip
    if ((g_debugmodeFlags & DEBUGFLAG_NOCLIP) && pThing == sithPlayer_pLocalPlayerThing)
    {
        pThing->physicsParams.physflags &= ~SITH_PF_USEGRAVITY;
        pThing->physicsParams.physflags |= SITH_PF_FLY;

        sithThing_DetachThing(pThing);
        return;
    }

    range = 0.0;
    searchFlags = 0;
    if (!pThing->sector)
        return;

    if (pThing->sector->flags & SITH_SECTOR_UNDERWATER && pThing->type == SITH_THING_PLAYER)
    {
        sithCollision_SearchRadiusForThings(pThing->sector, pThing, &pThing->position, &rdroid_zVector3, 0.05, 0.0, RAYCAST_1);
        v5 = sithCollision_NextSearchResult();
        if ( v5 )
        {
            while ( (v5->hitType & SITHCOLLISION_ADJOINCROSS) == 0 || (v5->surface->adjoin->sector->flags & SITH_SECTOR_UNDERWATER) != 0 )
            {
                v5 = sithCollision_NextSearchResult();
                if ( !v5 )
                    goto LABEL_8;
            }
            pThing->field_48 = v5->distance;
            pThing->physicsParams.physflags |= SITH_PF_WATERSURFACE;
            sithCollision_SearchClose();
        }
        else
        {
LABEL_8:
            sithCollision_SearchClose();
            pThing->physicsParams.physflags &= ~SITH_PF_WATERSURFACE;
        }
    }
    else
    {
        if ( (pThing->physicsParams.physflags & SITH_PF_WALLSTICK) == 0 )
        {
            direction.x = -0.0;
            direction.y = direction.x;
            direction.z = -1.0;
            searchFlags = RAYCAST_10;
        }
        else
        {
            rdVector_Neg3(&direction, &pThing->lookOrientation.uvec);
        }

        if ( a3 || pThing->attach_flags )
        {
            v9 = pThing->physicsParams.height;
            if ( v9 == 0.0 )
            {
                if ( pThing->rdthing.type == RD_THINGTYPE_MODEL )
                    v9 = pThing->rdthing.model3->insertOffset.z;
                thinga = pThing->moveSize - -0.005;
                if ( v9 <= thinga )
                    v9 = thinga;
            }
            if ( (pThing->physicsParams.physflags & (SITH_PF_FLOORSTICK|SITH_PF_WALLSTICK)) != 0 )
                v8 = v9 + v9;
            else
                v8 = v9 * 1.1;
        }
        else
        {
            v8 = pThing->moveSize - -0.005;
        }

        if ( v8 > 0.0 )
        {
            sithCollision_SearchRadiusForThings(pThing->sector, 0, &pThing->position, &direction, v8, 0.0, searchFlags | RAYCAST_2000 | RAYCAST_800 | RAYCAST_2);
            while ( 1 )
            {
                for ( i = sithCollision_NextSearchResult(); i; i = sithCollision_NextSearchResult() )
                {
                    if ( (i->hitType & SITHCOLLISION_WORLD) != 0 )
                    {
                        //printf("Attach to new surface? %x\n", i->surface->field_0);
                        sithThing_AttachToSurface(pThing, i->surface, a3);
                        sithCollision_SearchClose();
                        return;
                    }
                    if ( (i->hitType & SITHCOLLISION_THING) != 0 )
                    {
                        v11 = i->receiver;
                        if ( v11 != pThing )
                        {
                            v12 = i->face;
                            if ( !v12 || !i->sender )
                            {
                                sithCollision_SearchClose();
                                return;
                            }
                            
                            // Track thing that can move
                            if ( (searchFlags & RAYCAST_10) == 0
                              || (rdMatrix_TransformVector34(&a1, &v12->normal, &v11->lookOrientation), rdVector_Dot3(&a1, &rdroid_zVector3) >= 0.6) )
                            {
                                sithThing_LandThing(pThing, v11, i->face, i->sender->vertices, a3);
                                sithCollision_SearchClose();
                                return;
                            }
                        }
                    }
                }
                sithCollision_SearchClose();
                if ( range != 0.0 )
                    break;

                if ( pThing->type != SITH_THING_ACTOR && pThing->type != SITH_THING_PLAYER )
                    break;
                if ( pThing->moveSize == 0.0 )
                    break;
                range = pThing->moveSize;
                sithCollision_SearchRadiusForThings(pThing->sector, 0, &pThing->position, &direction, v8, range, searchFlags | RAYCAST_2000 | RAYCAST_800 | RAYCAST_2);
            }
        }
        if ( pThing->attach_flags )
            sithThing_DetachThing(pThing);
    }
}

// Inlined func

void sithPhysics_ThingTick(sithThing *pThing, flex_t deltaSecs)
{
    if (!pThing->sector)
        return;

    rdVector_Zero3(&pThing->physicsParams.velocityMaybe);
    rdVector_Zero3(&pThing->physicsParams.addedVelocity);

    if ((pThing->type == SITH_THING_ACTOR || pThing->type == SITH_THING_PLAYER) 
        && (pThing->actorParams.typeflags & SITH_AF_COMBO_FREEZE))
    {
        rdVector_Zero3(&pThing->physicsParams.acceleration);
    }

    if (pThing->attach_flags & (SITH_ATTACH_THINGSURFACE | SITH_ATTACH_WORLDSURFACE))
    {
        sithPhysics_ThingPhysAttached(pThing, deltaSecs);
    }
    else if (pThing->sector->flags & SITH_SECTOR_UNDERWATER)
    {
        sithPhysics_ThingPhysUnderwater(pThing, deltaSecs);
    }
#ifdef QOL_IMPROVEMENTS
    else if ( pThing->type == SITH_THING_PLAYER && (jkPlayer_bUseOldPlayerPhysics || sithNet_isMulti))
    {
#ifdef FIXED_TIMESTEP_PHYS
        if ((NEEDS_STEPPED_PHYS) && !jkPlayer_bUseOldPlayerPhysics) {
            // time stepping is handled elsewhere
            sithPhysics_ThingPhysGeneral(pThing, deltaSecs);
        }
        else
        {
            sithPhysics_ThingPhysPlayer(pThing, deltaSecs);
        }
#else
        sithPhysics_ThingPhysPlayer(pThing, deltaSecs);
#endif
    }
#else
    else if ( pThing->type == SITH_THING_PLAYER )
    {
        sithPhysics_ThingPhysPlayer(pThing, deltaSecs);
    }
#endif
    else
    {
        sithPhysics_ThingPhysGeneral(pThing, deltaSecs);
    }
}

void sithPhysics_ThingApplyForce(sithThing *pThing, rdVector3 *forceVec)
{
    // Added: noclip
    if (pThing == sithPlayer_pLocalPlayerThing && (g_debugmodeFlags & DEBUGFLAG_NOCLIP)) {
        return;
    }

    if ( pThing->moveType == SITH_MT_PHYSICS && pThing->physicsParams.mass > 0.0 )
    {
        flex_t invMass = 1.0 / pThing->physicsParams.mass;

        if ( forceVec->z * invMass > 0.5 ) // TODO verify
            sithThing_DetachThing(pThing);

        rdVector_MultAcc3(&pThing->physicsParams.vel, forceVec, invMass);
        pThing->physicsParams.physflags |= SITH_PF_8000;
    }
}

void sithPhysics_ThingSetLook(sithThing *pThing, const rdVector3 *look, flex_t a3)
{
    flex_d_t v4; // st7
    flex_d_t v20; // st7

    v4 = stdMath_ClipPrecision(1.0 - rdVector_Dot3(&pThing->lookOrientation.uvec, look));
    if ( v4 == 0.0 )
    {
        pThing->physicsParams.physflags |= SITH_PF_100;
    }
    else if ( a3 == 0.0 )
    {
        // TODO: rdMatrix? Or are they just manually doing basis vectors?
        pThing->lookOrientation.uvec.x = look->x;
        pThing->lookOrientation.uvec.y = look->y;
        pThing->lookOrientation.uvec.z = look->z;
        pThing->lookOrientation.rvec.x = (pThing->lookOrientation.lvec.y * pThing->lookOrientation.uvec.z) - (pThing->lookOrientation.lvec.z * pThing->lookOrientation.uvec.y);
        pThing->lookOrientation.rvec.y = (pThing->lookOrientation.lvec.z * pThing->lookOrientation.uvec.x) - (pThing->lookOrientation.lvec.x * pThing->lookOrientation.uvec.z);
        pThing->lookOrientation.rvec.z = (pThing->lookOrientation.lvec.x * pThing->lookOrientation.uvec.y) - (pThing->lookOrientation.lvec.y * pThing->lookOrientation.uvec.x);
        rdVector_Normalize3Acc(&pThing->lookOrientation.rvec);
        pThing->lookOrientation.lvec.x = (pThing->lookOrientation.rvec.z * pThing->lookOrientation.uvec.y) - (pThing->lookOrientation.rvec.y * pThing->lookOrientation.uvec.z);
        pThing->lookOrientation.lvec.y = (pThing->lookOrientation.rvec.x * pThing->lookOrientation.uvec.z) - (pThing->lookOrientation.rvec.z * pThing->lookOrientation.uvec.x);
        pThing->lookOrientation.lvec.z = (pThing->lookOrientation.rvec.y * pThing->lookOrientation.uvec.x) - (pThing->lookOrientation.rvec.x * pThing->lookOrientation.uvec.y);
        

        pThing->physicsParams.physflags |= SITH_PF_100;
    }
    else
    {
        // TODO: rdMatrix? Or are they just manually doing basis vectors?
        v20 = a3 * 10.0;
        pThing->lookOrientation.uvec.x = look->x * v20 + pThing->lookOrientation.uvec.x;
        pThing->lookOrientation.uvec.y = look->z * v20 + pThing->lookOrientation.uvec.y;
        pThing->lookOrientation.uvec.z = look->y * v20 + pThing->lookOrientation.uvec.z;
        rdVector_Normalize3Acc(&pThing->lookOrientation.uvec);
        pThing->lookOrientation.lvec.x = (pThing->lookOrientation.rvec.z * pThing->lookOrientation.uvec.y) - (pThing->lookOrientation.rvec.y * pThing->lookOrientation.uvec.z);
        pThing->lookOrientation.lvec.y = (pThing->lookOrientation.rvec.x * pThing->lookOrientation.uvec.z) - pThing->lookOrientation.rvec.z * pThing->lookOrientation.uvec.x;
        pThing->lookOrientation.lvec.z = (pThing->lookOrientation.rvec.y * pThing->lookOrientation.uvec.x) - (pThing->lookOrientation.rvec.x * pThing->lookOrientation.uvec.y);
        rdVector_Normalize3Acc(&pThing->lookOrientation.lvec);
        pThing->lookOrientation.rvec.x = (pThing->lookOrientation.lvec.y * pThing->lookOrientation.uvec.z) - (pThing->lookOrientation.lvec.z * pThing->lookOrientation.uvec.y);
        pThing->lookOrientation.rvec.y = (pThing->lookOrientation.lvec.z * pThing->lookOrientation.uvec.x) - (pThing->lookOrientation.lvec.x * pThing->lookOrientation.uvec.z);
        pThing->lookOrientation.rvec.z = (pThing->lookOrientation.lvec.x * pThing->lookOrientation.uvec.y) - (pThing->lookOrientation.lvec.y * pThing->lookOrientation.uvec.x);
    }
}

void sithPhysics_ApplyDrag(rdVector3 *vec, flex_t drag, flex_t mag, flex_t deltaSecs)
{
    if (mag == 0.0 || rdVector_Len3(vec) >= mag)
    {
        if (drag != 0.0)
        {
            flex_d_t scaled = deltaSecs * drag;
            if (scaled > 1.0)
                scaled = 1.0;

            rdVector_MultAcc3(vec, vec, -scaled);
            
            rdMath_ClampVector(vec, 0.00001);
        }
    }
    else
    {
        rdVector_Zero3(vec);
    }
}

int sithPhysics_LoadThingParams(stdConffileArg *arg, sithThing *pThing, int param)
{
    flex32_t tmp;
    int tmpInt;
    flex32_t velx, vely, velz;

    switch ( param )
    {
        case THINGPARAM_SURFDRAG:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 )
                return 0;
            pThing->physicsParams.surfaceDrag = tmp;
            return 1;
        case THINGPARAM_AIRDRAG:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 )
                return 0;
            pThing->physicsParams.airDrag = tmp;
            return 1;
        case THINGPARAM_STATICDRAG:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 )
                return 0;
            pThing->physicsParams.staticDrag = tmp;
            return 1;
        case THINGPARAM_MASS:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 )
                return 0;
            pThing->physicsParams.mass = tmp;
            return 1;
        case THINGPARAM_HEIGHT:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 )
                return 0;
            pThing->physicsParams.height = tmp;
            return 1;
        case THINGPARAM_PHYSFLAGS:
            if ( _sscanf(arg->value, "%x", &tmpInt) != 1 )
                return 0;
            pThing->physicsParams.physflags = tmpInt;
            return 1;
        case THINGPARAM_MAXROTVEL:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 || pThing->moveType != SITH_MT_PHYSICS )
                return 0;
            pThing->physicsParams.maxRotVel = tmp;
            return 1;
        case THINGPARAM_MAXVEL:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 || pThing->moveType != SITH_MT_PHYSICS )
                return 0;
            pThing->physicsParams.maxVel = tmp;
            return 1;
        case THINGPARAM_VEL:
            if (_sscanf(
                      arg->value,
                      "(%f/%f/%f)",
                      &velx, &vely, &velz) != 3)
                return 0;
            pThing->physicsParams.vel.x = velx;
            pThing->physicsParams.vel.y = vely;
            pThing->physicsParams.vel.z = velz;
            return 1;
        case THINGPARAM_ANGVEL:
            if (_sscanf(
                      arg->value,
                      "(%f/%f/%f)",
                      &velx, &vely, &velz) != 3)
                return 0;
            pThing->physicsParams.angVel.x = velx;
            pThing->physicsParams.angVel.y = vely;
            pThing->physicsParams.angVel.z = velz;
            return 1;
        case THINGPARAM_ORIENTSPEED:
            tmp = _atof(arg->value);
            if ( tmp < 0.0 || pThing->moveType != SITH_MT_PHYSICS )
                return 0;
            pThing->physicsParams.orientSpeed = tmp;
            return 1;
        case THINGPARAM_BUOYANCY:
            tmp = _atof(arg->value);
            pThing->physicsParams.buoyancy = tmp;
            return 1;
        default:
            return 0;
    }
}

void sithPhysics_ThingStop(sithThing *pThing)
{
    rdVector_Zero3(&pThing->physicsParams.vel);
    rdVector_Zero3(&pThing->physicsParams.angVel);
    rdVector_Zero3(&pThing->physicsParams.field_1F8);
    rdVector_Zero3(&pThing->physicsParams.acceleration);
    rdVector_Zero3(&pThing->physicsParams.velocityMaybe);
    rdVector_Zero3(&pThing->field_268);
}

flex_t sithPhysics_ThingGetInsertOffsetZ(sithThing *pThing)
{
    flex_d_t result; // st7
    flex_t v2; // [esp+4h] [ebp+4h]

    result = pThing->physicsParams.height;
    if ( result == 0.0 )
    {
        if ( pThing->rdthing.type == RD_THINGTYPE_MODEL )
            result = pThing->rdthing.model3->insertOffset.z;
        v2 = pThing->moveSize - -0.005;
        if ( result <= v2 )
            result = v2;
    }
    return result;
}

// MOTS altered
void sithPhysics_ThingPhysGeneral(sithThing *pThing, flex_t deltaSeconds)
{
    rdVector3 a1a;
    rdVector3 a3;
    rdMatrix34 a;
    int bOverrideIdk = 0;
    flex_t zOverride = 0.0;

    rdVector_Zero3(&pThing->physicsParams.addedVelocity);
    rdVector_Zero3(&a1a);

    if (pThing->physicsParams.physflags & SITH_PF_ANGTHRUST)
    {
        if (!rdVector_IsZero3(&pThing->physicsParams.angVel))
        {
            sithPhysics_ApplyDrag(&pThing->physicsParams.angVel, pThing->physicsParams.airDrag - -0.2, 0.0, deltaSeconds);
        }

        rdVector_MultAcc3(&pThing->physicsParams.angVel, &pThing->physicsParams.field_1F8, deltaSeconds);
        
        rdMath_ClampVectorRange(&pThing->physicsParams.angVel, -pThing->physicsParams.maxRotVel, pThing->physicsParams.maxRotVel);
        rdMath_ClampVector(&pThing->physicsParams.angVel, 0.00001);
    }

    if (rdVector_IsZero3(&pThing->physicsParams.angVel))
    {
        rdVector_Zero3(&a3);
    }
    else
    {
        rdVector_Scale3(&a3, &pThing->physicsParams.angVel, deltaSeconds);
    }

    // MOTS added: weapon tracking?
#ifdef JKM_PARAMS
    if (pThing->type == SITH_THING_WEAPON && pThing->weaponParams.pTargetThing && pThing->weaponParams.field_38 != 0.0) {
        rdVector3 tmp;
        rdMatrix34 local_60;
        rdVector3 local_6c, local_78;

        rdVector_Sub3(&tmp, &pThing->weaponParams.pTargetThing->position, &pThing->position);
        flex_t fVar3 = deltaSeconds * pThing->weaponParams.field_38;

        if (-0.03 <= tmp.z) {
            if (tmp.z > 0.03) {
                zOverride = 1.0;
            }
        }
        else {
            zOverride = -1.0;
        }
        rdVector_Normalize3Acc(&tmp);
        rdMatrix_BuildFromLook34(&local_60,&tmp);
        rdMatrix_ExtractAngles34(&local_60,&local_6c);
        rdMatrix_Copy34(&local_60, &pThing->lookOrientation);
        rdMatrix_ExtractAngles34(&local_60,&local_78);
        tmp.y = local_6c.y - local_78.y;
        tmp.x = -local_78.x;
        tmp.z = -local_78.z;
        if (tmp.y > 180.0) {
            tmp.y = tmp.y - 360.0;
        }
        else if (tmp.y < -180.0) {
            tmp.y = tmp.y - -360.0;
        }
        flex_t fVar6 = tmp.y;
        if (tmp.y < 0.0) {
            fVar6 = -tmp.y;
        }
        if (fVar6 > fVar3) {
            fVar6 = tmp.y;
            if (tmp.y < 0.0) {
                fVar6 = -tmp.y;
            }
            tmp.y = (fVar3 / fVar6) * tmp.y;
        }
        bOverrideIdk = 1;
        rdVector_Copy3(&a3, &tmp);
    }
#endif

    if (!rdVector_IsZero3(&a3))
    {
        rdMatrix_BuildRotate34(&a, &a3);
        sithCollision_sub_4E7670(pThing, &a);

        if (pThing->physicsParams.physflags & SITH_PF_FLY)
            rdMatrix_TransformVector34Acc(&pThing->physicsParams.vel, &a);

        if ( ((jkPlayer_currentTickIdx + (pThing->thingIdx & 0xFF)) & 7) == 0 )
            rdMatrix_Normalize34(&pThing->lookOrientation);
    }

    if ( pThing->physicsParams.airDrag != 0.0 )
        sithPhysics_ApplyDrag(&pThing->physicsParams.vel, pThing->physicsParams.airDrag, 0.0, deltaSeconds);

    if (pThing->physicsParams.physflags & SITH_PF_USESTHRUST)
    {
        if (!(pThing->physicsParams.physflags & SITH_PF_FLY))
        {
            rdVector_Scale3Acc(&pThing->physicsParams.acceleration, 0.3);
        }
        rdVector_Scale3(&a1a, &pThing->physicsParams.acceleration, deltaSeconds);
        rdMatrix_TransformVector34Acc(&a1a, &pThing->lookOrientation);
    }

    if (pThing->physicsParams.mass != 0.0 
        && (pThing->sector->flags & SITH_SECTOR_HASTHRUST) 
        && !(pThing->physicsParams.physflags & SITH_PF_NOTHRUST))
    {
        // Apply configurable sector thrust scale factor to lessen water current effects
        rdVector3 scaledThrust;
        rdVector_Scale3(&scaledThrust, &pThing->sector->thrust, jkPlayer_sectorThrustScale);
        rdVector_MultAcc3(&a1a, &scaledThrust, deltaSeconds);
    }

    if (pThing->physicsParams.mass != 0.0 
        && pThing->physicsParams.physflags & SITH_PF_USEGRAVITY
        && !(pThing->sector->flags & SITH_SECTOR_NOGRAVITY))
    {
        flex_t gravity = sithWorld_pCurrentWorld->worldGravity * deltaSeconds;
        if ( (pThing->physicsParams.physflags & SITH_PF_PARTIALGRAVITY) != 0 )
            gravity *= 0.5;
        a1a.z = a1a.z - gravity;
        pThing->physicsParams.addedVelocity.z = -gravity;
    }

    rdVector_Add3Acc(&pThing->physicsParams.vel, &a1a);
#ifdef JKM_PARAMS
    if (bOverrideIdk) {
        pThing->physicsParams.vel.z = zOverride;
    }
#endif
    rdMath_ClampVector(&pThing->physicsParams.vel, 0.00001);

    if (!rdVector_IsZero3(&pThing->physicsParams.vel))
    {
        rdVector_Scale3(&pThing->physicsParams.velocityMaybe, &pThing->physicsParams.vel, deltaSeconds);
    }
}

// MOTS altered
void sithPhysics_ThingPhysPlayer(sithThing *player, flex_t deltaSeconds)
{
    rdMatrix34 a;
    rdVector3 a3;
    rdVector3 a1a;
    //int bOverrideIdk = 0; // Remove compiler warns
    flex_t zOverride = 0.0;

    rdVector_Zero3(&player->physicsParams.addedVelocity);
    if (player->physicsParams.physflags & SITH_PF_ANGTHRUST)
    {
        if (!rdVector_IsZero3(&player->physicsParams.angVel))
        {
            sithPhysics_ApplyDrag(&player->physicsParams.angVel, player->physicsParams.airDrag - -0.2, 0.0, deltaSeconds);
        }

        rdVector_MultAcc3(&player->physicsParams.angVel, &player->physicsParams.field_1F8, deltaSeconds);

        rdMath_ClampVectorRange(&player->physicsParams.angVel, -player->physicsParams.maxRotVel, player->physicsParams.maxRotVel);
        rdMath_ClampVector(&player->physicsParams.angVel, 0.00001);
    }

    if (rdVector_IsZero3(&player->physicsParams.angVel))
    {
        rdVector_Zero3(&a3);
    }
    else
    {
        rdVector_Scale3(&a3, &player->physicsParams.angVel, deltaSeconds);
    }

// MOTS added: weapon tracking? why is this here lol
#ifdef JKM_PARAMS
    if (player->type == SITH_THING_WEAPON && player->weaponParams.pTargetThing && player->weaponParams.field_38 != 0.0) {
        rdVector3 tmp;
        rdMatrix34 local_60;
        rdVector3 local_6c, local_78;

        rdVector_Sub3(&tmp, &player->weaponParams.pTargetThing->position, &player->position);
        flex_t fVar3 = deltaSeconds * player->weaponParams.field_38;

        rdVector_Normalize3Acc(&tmp);
        rdMatrix_BuildFromLook34(&local_60,&tmp);
        rdMatrix_ExtractAngles34(&local_60,&local_6c);
        rdMatrix_Copy34(&local_60, &player->lookOrientation);
        rdMatrix_ExtractAngles34(&local_60,&local_78);
        tmp.y = local_6c.y - local_78.y;
        tmp.x = -local_78.x;
        tmp.z = -local_78.z;
        if (tmp.y > 180.0) {
            tmp.y = tmp.y - 360.0;
        }
        else if (tmp.y < -180.0) {
            tmp.y = tmp.y - -360.0;
        }
        flex_t fVar6 = tmp.y;
        if (tmp.y < 0.0) {
            fVar6 = -tmp.y;
        }
        if (fVar6 > fVar3) {
            fVar6 = tmp.y;
            if (tmp.y < 0.0) {
                fVar6 = -tmp.y;
            }
            tmp.y = (fVar3 / fVar6) * tmp.y;
        }
        //bOverrideIdk = 1; // Remove compiler warns
        rdVector_Copy3(&a3, &tmp);
    }
#endif

    if (!rdVector_IsZero3(&a3))
    {
        rdMatrix_BuildRotate34(&a, &a3);
        sithCollision_sub_4E7670(player, &a);

        if (player->physicsParams.physflags & SITH_PF_FLY)
            rdMatrix_TransformVector34Acc(&player->physicsParams.vel, &a);

        if ( ((jkPlayer_currentTickIdx + (player->thingIdx & 0xFF)) & 7) == 0 )
            rdMatrix_Normalize34(&player->lookOrientation);
    }

    if (!(player->physicsParams.physflags & SITH_PF_FLY))
    {
        rdVector_Scale3Acc(&player->physicsParams.acceleration, 0.3);
    }

    // I think all of this is specifically for multiplayer, so that player things
    // sync better between clients.
    flex_t rolloverCombine = deltaSeconds + player->physicsParams.physicsRolloverFrames;

    flex_t framesToApply = rolloverCombine * OLDSTEP_TARGET_FPS; // get number of 50FPS steps passed
    player->physicsParams.physicsRolloverFrames = rolloverCombine - (flex_d_t)(unsigned int)(int)framesToApply * OLDSTEP_DELTA_50FPS;

    for (int i = (int)framesToApply; i > 0; i--)
    {
        rdVector_Zero3(&a1a);
        if ( player->physicsParams.airDrag != 0.0 )
        {
            sithPhysics_ApplyDrag(&player->physicsParams.vel, player->physicsParams.airDrag, 0.0, OLDSTEP_DELTA_50FPS);
        }

        if (player->physicsParams.physflags & SITH_PF_USESTHRUST)
        {
            rdVector_Scale3(&a1a, &player->physicsParams.acceleration, OLDSTEP_DELTA_50FPS);
            rdMatrix_TransformVector34Acc(&a1a, &player->lookOrientation);
        }

        if ( player->physicsParams.mass != 0.0 )
        {
            if ((player->sector->flags & SITH_SECTOR_HASTHRUST)
                && !(player->physicsParams.physflags & SITH_PF_NOTHRUST))
            {
                // Apply configurable sector thrust scale factor to lessen water current effects
                rdVector3 scaledThrust;
                rdVector_Scale3(&scaledThrust, &player->sector->thrust, jkPlayer_sectorThrustScale);
                rdVector_MultAcc3(&a1a, &scaledThrust, OLDSTEP_DELTA_50FPS);
            }
        }

        if ( player->physicsParams.mass != 0.0 
             && (player->physicsParams.physflags & SITH_PF_USEGRAVITY) 
             && !(player->sector->flags & SITH_SECTOR_NOGRAVITY) )
        {
            flex_t gravity = sithWorld_pCurrentWorld->worldGravity * OLDSTEP_DELTA_50FPS;
            if ( (player->physicsParams.physflags & SITH_PF_PARTIALGRAVITY) != 0 )
                gravity = gravity * 0.5;
            a1a.z = a1a.z - gravity;
            player->physicsParams.addedVelocity.z = -gravity;
        }
        rdVector_Add3Acc(&player->physicsParams.vel, &a1a);
        rdVector_MultAcc3(&player->physicsParams.velocityMaybe, &player->physicsParams.vel, OLDSTEP_DELTA_50FPS);
    }
}

// MOTS altered
void sithPhysics_ThingPhysUnderwater(sithThing *pThing, flex_t deltaSeconds)
{
    flex_d_t v35; // st6
    flex_d_t v51; // st7
    rdVector3 a1a; // [esp+24h] [ebp-48h] BYREF
    rdVector3 a3; // [esp+30h] [ebp-3Ch] BYREF
    rdMatrix34 tmpMat; // [esp+3Ch] [ebp-30h] BYREF

    rdVector_Zero3(&a1a);
    rdVector_Zero3(&pThing->physicsParams.addedVelocity);
    if ( (pThing->physicsParams.physflags & SITH_PF_ANGTHRUST) != 0 )
    {
        if ( !rdVector_IsZero3(&pThing->physicsParams.angVel) )
        {
            sithPhysics_ApplyDrag(&pThing->physicsParams.angVel, pThing->physicsParams.airDrag - -0.2, 0.0, deltaSeconds);
        }
        rdVector_MultAcc3(&pThing->physicsParams.angVel, &pThing->physicsParams.field_1F8, deltaSeconds);
        rdVector_ClampValue3(&pThing->physicsParams.angVel, pThing->physicsParams.maxRotVel);
        rdVector_ClipPrecision3(&pThing->physicsParams.angVel);
    }
    if ( rdVector_IsZero3(&pThing->physicsParams.angVel) )
    {
        rdVector_Zero3(&a3);
    }
    else
    {
        rdVector_Scale3(&a3, &pThing->physicsParams.angVel, deltaSeconds);
    }

    if (!rdVector_IsZero3(&a3))
    {
        rdMatrix_BuildRotate34(&tmpMat, &a3);
        sithCollision_sub_4E7670(pThing, &tmpMat);
        if ( (((jkPlayer_currentTickIdx & 0xFF) + (pThing->thingIdx & 0xFF)) & 7) == 0 )
            rdMatrix_Normalize34(&pThing->lookOrientation);
    }
    if ( pThing->physicsParams.airDrag != 0.0 )
    {
        sithPhysics_ApplyDrag(&pThing->physicsParams.vel, pThing->physicsParams.airDrag * 4.0, 0.0, deltaSeconds);
    }
    if ( (pThing->physicsParams.physflags & SITH_PF_USESTHRUST) != 0 )
    {
        rdVector_Scale3Acc(&pThing->physicsParams.acceleration, 0.6);
        rdVector_Scale3(&a1a, &pThing->physicsParams.acceleration, deltaSeconds);
        rdMatrix_TransformVector34Acc(&a1a, &pThing->lookOrientation);
    }
    if ( pThing->physicsParams.mass != 0.0 && pThing->sector && (pThing->sector->flags & SITH_SECTOR_HASTHRUST) && !(pThing->physicsParams.physflags & SITH_PF_NOTHRUST) )
    {
        // Apply configurable sector thrust scale factor to lessen water current effects
        rdVector3 scaledThrust;
        rdVector_Scale3(&scaledThrust, &pThing->sector->thrust, jkPlayer_sectorThrustScale);
        rdVector_MultAcc3(&a1a, &scaledThrust, deltaSeconds);
    }

    if ( ((pThing->physicsParams.physflags & SITH_PF_WATERSURFACE) == 0 || (pThing->thingflags & SITH_TF_DEAD) != 0) && (pThing->physicsParams.physflags & SITH_PF_USEGRAVITY) != 0 )
    {
        v35 = sithWorld_pCurrentWorld->worldGravity * deltaSeconds * pThing->physicsParams.buoyancy;
        a1a.z -= v35;
        pThing->physicsParams.addedVelocity.z -= v35;
    }
    rdVector_Add3Acc(&pThing->physicsParams.vel, &a1a);

    rdVector_ClipPrecision3(&pThing->physicsParams.vel);
    if ( !rdVector_IsZero3(&pThing->physicsParams.vel) )
    {
        rdVector_Scale3(&pThing->physicsParams.velocityMaybe, &pThing->physicsParams.vel, deltaSeconds);
    }
    if ( (pThing->physicsParams.physflags & SITH_PF_WATERSURFACE) != 0 && pThing->physicsParams.acceleration.z >= 0.0 )
    {
        v51 = pThing->field_48 - 0.01;
        if ( pThing->physicsParams.velocityMaybe.z > 0.0 && pThing->physicsParams.velocityMaybe.z < (flex_d_t)deltaSeconds * 0.2 ) // verify first
            pThing->physicsParams.velocityMaybe.z = 0.0;
        if ( v51 > 0.0 )
        {
            if ( v51 >= deltaSeconds * 0.2 )
                v51 = deltaSeconds * 0.2;
            rdVector_MultAcc3(&pThing->physicsParams.velocityMaybe, &rdroid_zVector3, v51);
        }
    }
}

// MOTS altered
void sithPhysics_ThingPhysAttached(sithThing *pThing, flex_t deltaSeconds)
{   
    flex_t a2a; // [esp+0h] [ebp-94h]
    flex_t v144; // [esp+4h] [ebp-90h]
    flex_t possibly_undef_2; // [esp+1Ch] [ebp-78h]
    flex_t new_z; // [esp+20h] [ebp-74h]
    flex_t new_x; // [esp+24h] [ebp-70h]
    flex_t v158; // [esp+28h] [ebp-6Ch]
    flex_t possibly_undef_1; // [esp+2Ch] [ebp-68h]
    flex_t new_y; // [esp+30h] [ebp-64h]
    flex_t new_ya; // [esp+30h] [ebp-64h]
    rdVector3 vel_change; // [esp+34h] [ebp-60h] BYREF
    rdVector3 attachedNormal; // [esp+40h] [ebp-54h] BYREF
    rdVector3 out; // [esp+4Ch] [ebp-48h] BYREF
    rdVector3 a3; // [esp+58h] [ebp-3Ch] BYREF
    rdMatrix34 a; // [esp+64h] [ebp-30h] BYREF
    int bOverrideIdk = 0;
    flex_t zOverride = 0.0;

    possibly_undef_1 = 0.0;
    possibly_undef_2 = 0.0;

    rdVector_Zero3(&vel_change);
    v158 = 1.0;
    pThing->physicsParams.physflags &= ~SITH_PF_200000;
    if ( (pThing->attach_flags & SITH_ATTACH_WORLDSURFACE) != 0 )
    {
        attachedNormal = pThing->attachedSufaceInfo->face.normal;
        possibly_undef_1 = rdMath_DistancePointToPlane(&pThing->position, &attachedNormal, &pThing->field_38);
        if ( (pThing->attachedSurface->surfaceFlags & (SITH_SURFACE_ICY|SITH_SURFACE_VERYICY)) != 0 )
        {
            if ( (pThing->attachedSurface->surfaceFlags & SITH_SURFACE_VERYICY) != 0 )
                possibly_undef_2 = 0.1;
            else
                possibly_undef_2 = 0.3;
        }
        else
        {
            possibly_undef_2 = 1.0;
        }
    }
    else if ( (pThing->attach_flags & SITH_ATTACH_THINGSURFACE) != 0 )
    {
        rdMatrix_TransformVector34(&attachedNormal, &pThing->attachedSufaceInfo->face.normal, &pThing->attachedThing->lookOrientation);
        rdMatrix_TransformVector34(&a3, &pThing->field_38, &pThing->attachedThing->lookOrientation);
        possibly_undef_2 = 1.0;
        rdVector_Add3Acc(&a3, &pThing->attachedThing->position);
        possibly_undef_1 = rdMath_DistancePointToPlane(&pThing->position, &attachedNormal, &a3);
    }

    if (pThing->physicsParams.physflags & SITH_PF_800)
    {
        v158 = rdVector_Dot3(&attachedNormal, &rdroid_zVector3);
        if ( v158 < 1.0 )
            possibly_undef_1 = possibly_undef_1 / v158;
    }

    if (!(pThing->physicsParams.physflags & SITH_PF_100))
    {
        if ( (pThing->physicsParams.physflags & SITH_PF_SURFACEALIGN) != 0 )
        {
            sithPhysics_ThingSetLook(pThing, &attachedNormal, pThing->physicsParams.orientSpeed * deltaSeconds);
        }
        else if ( (pThing->physicsParams.physflags & SITH_PF_800) != 0 )
        {
            sithPhysics_ThingSetLook(pThing, &rdroid_zVector3, pThing->physicsParams.orientSpeed * deltaSeconds);
        }
        else
        {
            pThing->physicsParams.physflags |= SITH_PF_100;
        }
    }

    if (pThing->physicsParams.physflags & SITH_PF_ANGTHRUST)
    {
        if (!rdVector_IsZero3(&pThing->physicsParams.angVel))
        {
            sithPhysics_ApplyDrag(&pThing->physicsParams.angVel, pThing->physicsParams.surfaceDrag - -0.2, 0.0, deltaSeconds);
        }

        pThing->physicsParams.angVel.y = pThing->physicsParams.field_1F8.y * deltaSeconds + pThing->physicsParams.angVel.y;
        rdVector_ClampValue3(&pThing->physicsParams.angVel, pThing->physicsParams.maxRotVel);
        rdVector_ClipPrecision3(&pThing->physicsParams.angVel);
    }
    if ( pThing->physicsParams.angVel.y != 0.0 )
    {
        rdVector_Scale3(&a3, &pThing->physicsParams.angVel, deltaSeconds);

// MOTS added: weapon tracking?
#ifdef JKM_PARAMS
    if (pThing->type == SITH_THING_WEAPON && pThing->weaponParams.pTargetThing && pThing->weaponParams.field_38 != 0.0) {
        rdVector3 tmp;
        rdMatrix34 local_60;
        rdVector3 local_6c, local_78;

        rdVector_Sub3(&tmp, &pThing->weaponParams.pTargetThing->position, &pThing->position);
        flex_t fVar3 = deltaSeconds * pThing->weaponParams.field_38;

        if (-0.03 <= tmp.z) {
            if (tmp.z > 0.03) {
                zOverride = 1.0;
            }
        }
        else {
            zOverride = -1.0;
        }
        rdVector_Normalize3Acc(&tmp);
        rdMatrix_BuildFromLook34(&local_60,&tmp);
        rdMatrix_ExtractAngles34(&local_60,&local_6c);
        rdMatrix_Copy34(&local_60, &pThing->lookOrientation);
        rdMatrix_ExtractAngles34(&local_60,&local_78);
        tmp.y = local_6c.y - local_78.y;
        tmp.x = -local_78.x;
        tmp.z = -local_78.z;
        if (tmp.y > 180.0) {
            tmp.y = tmp.y - 360.0;
        }
        else if (tmp.y < -180.0) {
            tmp.y = tmp.y - -360.0;
        }
        flex_t fVar6 = tmp.y;
        if (tmp.y < 0.0) {
            fVar6 = -tmp.y;
        }
        if (fVar6 > fVar3) {
            fVar6 = tmp.y;
            if (tmp.y < 0.0) {
                fVar6 = -tmp.y;
            }
            tmp.y = (fVar3 / fVar6) * tmp.y;
        }
        bOverrideIdk = 1;
        rdVector_Copy3(&a3, &tmp);
    }
#endif

        rdMatrix_BuildRotate34(&a, &a3);
        sithCollision_sub_4E7670(pThing, &a);
        if ( possibly_undef_2 >= 1.0 )
        {
            rdMatrix_TransformVector34Acc(&pThing->physicsParams.vel, &a);
        }
        else
        {
            rdMatrix_TransformVector34(&out, &pThing->physicsParams.vel, &a);
            rdVector_Scale3Acc(&pThing->physicsParams.vel, 1.0 - possibly_undef_2);
            rdVector_MultAcc3(&pThing->physicsParams.vel, &out, possibly_undef_2);
        }
        if ( (((jkPlayer_currentTickIdx & 0xFF) + (pThing->thingIdx & 0xFF)) & 7) == 0 )
            rdMatrix_Normalize34(&pThing->lookOrientation);
    }
    if ( possibly_undef_2 < 0.25 )
    {
        possibly_undef_2 = 0.25;
    }
    else if ( possibly_undef_2 > 1.0 )
    {
        possibly_undef_2 = 1.0;
    }

    if (!rdVector_IsZero3(&pThing->physicsParams.vel) && pThing->physicsParams.surfaceDrag != 0.0)
    {
        if ( (pThing->physicsParams.physflags & SITH_PF_8000) == 0 )
        {
            if ( rdVector_IsZero3(&pThing->physicsParams.acceleration)
              && !(pThing->sector->flags & SITH_SECTOR_HASTHRUST)
              && possibly_undef_2 > 0.8 )
            {
                a2a = pThing->physicsParams.surfaceDrag * possibly_undef_2;
                v144 = pThing->physicsParams.staticDrag * possibly_undef_2;
            }
            else
            {
                a2a = pThing->physicsParams.surfaceDrag * possibly_undef_2;
                v144 = 0.0;
            }
            sithPhysics_ApplyDrag(&pThing->physicsParams.vel, a2a, v144, deltaSeconds);
        }
        else
        {
            pThing->physicsParams.physflags &= ~SITH_PF_8000;
        }
    }

    if ( (pThing->physicsParams.physflags & SITH_PF_USESTHRUST) != 0
      && !rdVector_IsZero3(&pThing->physicsParams.acceleration) )
    {
        flex_t v44 = possibly_undef_2 * deltaSeconds;
        if ( (pThing->physicsParams.physflags & SITH_PF_CROUCHING) != 0 )
            v44 = deltaSeconds * 0.8;
        rdVector_Scale3(&vel_change, &pThing->physicsParams.acceleration, v44);
        rdVector_ClipPrecision3(&vel_change);
        if ( !rdVector_IsZero3(&vel_change) )
            rdMatrix_TransformVector34Acc(&vel_change, &pThing->lookOrientation);
    }

    if (pThing->physicsParams.mass != 0.0 && (pThing->sector->flags & SITH_SECTOR_HASTHRUST) && !(pThing->physicsParams.physflags & SITH_PF_NOTHRUST))
    {
        // Check if scaled thrust is strong enough to lift the object
        if ( pThing->sector->thrust.z * jkPlayer_sectorThrustScale > sithWorld_pCurrentWorld->worldGravity * pThing->physicsParams.mass )
        {
            sithThing_DetachThing(pThing);
            rdVector_Zero3(&pThing->physicsParams.addedVelocity);
            rdVector_Zero3(&out);
            if ( (pThing->physicsParams.physflags & SITH_PF_ANGTHRUST) != 0 )
            {
                if ( !rdVector_IsZero3(&pThing->physicsParams.angVel) )
                {
                    sithPhysics_ApplyDrag(&pThing->physicsParams.angVel, pThing->physicsParams.airDrag - -0.2, 0.0, deltaSeconds);
                }
                rdVector_MultAcc3(&pThing->physicsParams.angVel, &pThing->physicsParams.field_1F8, deltaSeconds);

                rdVector_ClampValue3(&pThing->physicsParams.angVel, pThing->physicsParams.maxRotVel);
                rdVector_ClipPrecision3(&pThing->physicsParams.angVel);
            }
            if ( rdVector_IsZero3(&pThing->physicsParams.angVel) )
            {
                rdVector_Zero3(&a3);
            }
            else
            {
                rdVector_Scale3(&a3, &pThing->physicsParams.angVel, deltaSeconds);
            }
            if ( !rdVector_IsZero3(&a3) )
            {
                rdMatrix_BuildRotate34(&a, &a3);
                sithCollision_sub_4E7670(pThing, &a);
                if ( (pThing->physicsParams.physflags & SITH_PF_FLY) != 0 )
                    rdMatrix_TransformVector34Acc(&pThing->physicsParams.vel, &a);
                if ( ((jkPlayer_currentTickIdx + (pThing->thingIdx & 0xFF)) & 7) == 0 )
                    rdMatrix_Normalize34(&pThing->lookOrientation);
            }

            if ( pThing->physicsParams.airDrag != 0.0 )
                sithPhysics_ApplyDrag(&pThing->physicsParams.vel, pThing->physicsParams.airDrag, 0.0, deltaSeconds);

            if (pThing->physicsParams.physflags & SITH_PF_USESTHRUST)
            {
                if (!(pThing->physicsParams.physflags & SITH_PF_FLY))
                {
                    rdVector_Scale3Acc(&pThing->physicsParams.acceleration, 0.3);
                }
                rdVector_Scale3(&out, &pThing->physicsParams.acceleration, deltaSeconds);
            }

            if ( pThing->physicsParams.mass != 0.0
              && (pThing->sector->flags & SITH_SECTOR_HASTHRUST)
              && !(pThing->physicsParams.physflags & SITH_PF_NOTHRUST))
            {
                // Apply configurable sector thrust scale factor to lessen water current effects
                rdVector3 scaledThrust;
                rdVector_Scale3(&scaledThrust, &pThing->sector->thrust, jkPlayer_sectorThrustScale);
                rdVector_MultAcc3(&out, &scaledThrust, deltaSeconds);
            }

            if ( pThing->physicsParams.mass != 0.0 && (pThing->physicsParams.physflags & SITH_PF_USEGRAVITY) != 0 && (pThing->sector->flags & SITH_PF_USEGRAVITY) == 0 )
            {
                flex_t v91 = sithWorld_pCurrentWorld->worldGravity * deltaSeconds;
                if ( (pThing->physicsParams.physflags & SITH_PF_PARTIALGRAVITY) != 0 )
                    v91 = v91 * 0.5;
                out.z -= v91;
                pThing->physicsParams.addedVelocity.z = -v91;
            }
            rdVector_Add3Acc(&pThing->physicsParams.vel, &out);
            rdVector_ClipPrecision3(&pThing->physicsParams.vel);
            if ( !rdVector_IsZero3(&pThing->physicsParams.vel) )
            {
                rdVector_Scale3(&pThing->physicsParams.velocityMaybe, &pThing->physicsParams.vel, deltaSeconds);
            }
            return;
        }
        // Apply configurable sector thrust scale factor to lessen water current effects
        rdVector3 scaledThrust;
        rdVector_Scale3(&scaledThrust, &pThing->sector->thrust, jkPlayer_sectorThrustScale);
        rdVector_MultAcc3(&vel_change, &scaledThrust, deltaSeconds);
    }
    rdVector_Add3Acc(&pThing->physicsParams.vel, &vel_change);
    
    // Is the player climbing up/down a slope?
    if ( pThing->type == SITH_THING_PLAYER
      && (pThing->physicsParams.physflags & SITH_PF_USEGRAVITY) != 0
      && v158 <= 1.0
      && (possibly_undef_2 < 0.8 || !rdVector_IsZero3(&pThing->physicsParams.vel)) )
    {
        flex_t v108 = stdMath_Clamp(1.0 - possibly_undef_2, 0.2, 0.8);
        pThing->physicsParams.vel.z -= sithWorld_pCurrentWorld->worldGravity * deltaSeconds * v108;
    }

    if ( !rdVector_IsZero3(&pThing->physicsParams.vel) )
    {
        flex_t v109 = rdVector_Dot3(&attachedNormal, &pThing->physicsParams.vel);

        if ( stdMath_ClipPrecision(v109) != 0.0 )
        {
            // If deltaSeconds is < the canonical phys tickrate, we get these
            // crazy oscillations
#ifdef FIXED_TIMESTEP_PHYS
            // Fix physics being tied to framerate?
            if (NEEDS_STEPPED_PHYS && (deltaSeconds < CANONICAL_PHYS_TICKRATE))
                v109 *= (deltaSeconds / CANONICAL_PHYS_TICKRATE);
#endif
#ifdef EXPERIMENTAL_FIXED_POINT
            if (!(NEEDS_STEPPED_PHYS) && (deltaSeconds < CANONICAL_PHYS_TICKRATE))
                v109 *= (deltaSeconds / CANONICAL_PHYS_TICKRATE);
#endif
            rdVector_MultAcc3(&pThing->physicsParams.vel, &attachedNormal, -v109);
        }
    }

#ifdef JKM_PARAMS
    if (bOverrideIdk) {
        pThing->physicsParams.vel.z = zOverride;
    }
#endif

    rdVector_ClipPrecision3(&pThing->physicsParams.vel);
    if ( !rdVector_IsZero3(&pThing->physicsParams.vel) )
    {
        rdVector_Scale3(&pThing->physicsParams.velocityMaybe, &pThing->physicsParams.vel, deltaSeconds);
    }

    flex_t v131;
    if (pThing->physicsParams.physflags & SITH_PF_CROUCHING)
    {
        v131 = v158 * possibly_undef_1 - (pThing->moveSize - -0.01);
    }
    else
    {
        flex_t v132 = pThing->physicsParams.height;
        if ( v132 == 0.0 )
        {
            if ( pThing->rdthing.type == RD_THINGTYPE_MODEL )
                v132 = pThing->rdthing.model3->insertOffset.z;
            new_ya = pThing->moveSize - -0.005;
            if ( v132 <= new_ya )
                v132 = new_ya;
        }
        v131 = possibly_undef_1 - v132;
    }

    // Slide down slopes
    v131 = stdMath_ClipPrecision(v131);
    if ( v131 != 0.0 )
    {
        // Fix physics being tied to framerate?
        flex_t orig_v131 = stdMath_ClampValue(v131, deltaSeconds * 0.5);
        flex_t new_v131 = v131 * (deltaSeconds / CANONICAL_PHYS_TICKRATE);
        new_v131 = stdMath_ClampValue(new_v131, deltaSeconds * 0.5);

        // If deltaSeconds is < the canonical phys tickrate, we get these
        // crazy oscillations
#ifdef FIXED_TIMESTEP_PHYS
        if (NEEDS_STEPPED_PHYS && deltaSeconds < CANONICAL_PHYS_TICKRATE)
            v131 = new_v131;
        else
            v131 = orig_v131;
#else
        v131 = orig_v131;
#endif
#ifdef EXPERIMENTAL_FIXED_POINT
        if (deltaSeconds < CANONICAL_PHYS_TICKRATE)
            v131 = new_v131;
#endif

        // Added: Fix turret slowly drifting up?
        if ((pThing->type == SITH_THING_ACTOR || pThing->type == SITH_THING_PLAYER) 
            && (pThing->actorParams.typeflags & SITH_AF_COMBO_FREEZE))
        {
            v131 = orig_v131;
        }

        if ( (pThing->physicsParams.physflags & SITH_PF_800) != 0 )
        {
            rdVector_MultAcc3(&pThing->physicsParams.velocityMaybe, &rdroid_zVector3, -v131);
        }
        else
        {
            rdVector_MultAcc3(&pThing->physicsParams.velocityMaybe, &attachedNormal, -v131);
        }
    }
}