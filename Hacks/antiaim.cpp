//
//  antiaim.cpp
//  vHook
//

#include "antiaim.h"


Vector AntiAim::realAngle;
float side = 1.0f;
Vector AntiAim::fakeAngle;


static bool manualswitch = true;



Vector atTargets;
bool isManual = false;
bool Swtich = false;
static bool jitter2 = false;


template <class T>
constexpr const T& Clamp(const T& v, const T& lo, const T& hi)
{
    return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
}

template <class T>
constexpr const T& Min(const T& x, const T& y)
{
    return (x > y) ? y : x;
}

template <class T>
constexpr const T& Max(const T& x, const T& y)
{
    return (x < y) ? y : x;
}

void inline SinCos(float radians, float* sine, float* cosine)
{
    *sine = sin(radians);
    *cosine = cos(radians);
}


void VectorAngles2(const Vector &vecForward, Vector &vecAngles)
{
    Vector vecView;
    if (vecForward[1] == 0.f && vecForward[0] == 0.f)
    {
        vecView[0] = 0.f;
        vecView[1] = 0.f;
    }
    else
    {
        vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / M_PI;
        
        if (vecView[1] < 0.f)
            vecView[1] += 360.f;
        
        vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);
        
        vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / M_PI;
    }
    
    vecAngles[0] = -vecView[0];
    vecAngles[1] = vecView[1];
    vecAngles[2] = 0.f;
}


float GetMaxDelta(CCSGOAnimState *animState ) {
    
    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }

    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;

    return delta - 0.5f;
}
static float Distance(Vector a, Vector b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}
void AngleVectors3(const Vector &angles, Vector& forward, Vector& right, Vector& up)
{
    float sr, sp, sy, cr, cp, cy;
    
    SinCos(DEG2RAD(angles[1]), &sy, &cy);
    SinCos(DEG2RAD(angles[0]), &sp, &cp);
    SinCos(DEG2RAD(angles[2]), &sr, &cr);
    
    forward.x = (cp * cy);
    forward.y = (cp * sy);
    forward.z = (-sp);
    right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
    right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
    right.z = (-1 * sr * cp);
    up.x = (cr * sp * cy + -sr * -sy);
    up.y = (cr * sp * sy + -sr * cy);
    up.z = (cr * cp);
}
static bool GetBestHeadAngle(CUserCmd* cmd, Vector &angle, bool bSend)
{
    float b, r, l;

    Vector src3D, dst3D, forward, right, up;

    trace_t tr;
    Ray_t ray, ray2, ray3;
    CTraceFilter filter;

    C_BasePlayer *localplayer = (C_BasePlayer *)pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer)
        return false;

    QAngle viewAngles;
       pEngine->GetViewAngles(angle);

    angle.x = 0;


    AngleVectors3(angle, forward, right, up);

  

    //auto target = GetTargetEntity();
    filter.pSkip = localplayer;
    src3D = localplayer->GetEyePosition();
    dst3D = src3D + (forward * 384);

//    if (!target)
  //      return false;

    ray.Init(src3D, dst3D);
    pEngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
    b = (tr.endpos - tr.startpos).Length();

    ray2.Init(src3D + right * 35, dst3D + right * 35);
    pEngineTrace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
    r = (tr.endpos - tr.startpos).Length();

    ray3.Init(src3D - right * 35, dst3D - right * 35);
    pEngineTrace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
    l = (tr.endpos - tr.startpos).Length();


    if (b < r && b < l && l == r)
            return true; //if left and right are equal and better than back

        if (b > r && b > l)
            angle.y -= 180; //if back is the best angle
        else if (r > l && r > b)
            angle.y += 90; //if right is the best angle
        else if (r > l && r == b)
            angle.y += 135; //if right is equal to back
        else if (l > r && l > b)
            angle.y -= 90; //if left is the best angle
        else if (l > r && l == b)
            angle.y -= 135; //if left is equal to back
        else
            return false;

        return true;
    if( bSend ){
        AntiAim::fakeAngle.y = angle.y;
       } else {
           AntiAim::realAngle.y = angle.y;
       }
    }

static void DoAntiAimX(Vector& angle, bool bFlip, bool& clamp)
{
    static float pDance = 0.0f;

    switch (vars.misc.aaX)
    {
        case 1:
            angle.x = -89.0f;
            break;
        case 2:
            angle.x = 89.0f;
            break;
        case 3:
            pDance += 45.0f;
            if (pDance > 100)
                pDance = 0.0f;
            else if (pDance > 75.f)
                angle.x = -89.f;
            else if (pDance < 75.f)
                angle.x = 89.f;
            break;
        default:
            break;
    }
}

static void DoAntiAimY(C_BasePlayer *const localplayer, Vector& angle, bool bSend, CUserCmd* cmd)
{
    int aa_type = vars.misc.type;

    bool directionSwitch = false;
    
    static bool bFlip = false;
    float delta;
    
    float maxDelta = GetMaxDelta(localplayer->GetAnimState());
    
    switch (aa_type)
    {
        case 1:
            angle.y = AntiAim::fakeAngle.y - maxDelta;
            break;
        case 2:
                        angle.y = AntiAim::fakeAngle.y + maxDelta;
            break;
        case 3:
                        bFlip = !bFlip;
            angle.y -= bFlip ? maxDelta : -maxDelta;
            break;
        case 4:
                        angle.y += bFlip ? maxDelta : -1 * maxDelta;
            bFlip = !bFlip;
            break;
        case 5:
                {
                    static bool yFlip = false;

                   // angle.x = 89.0f;
                    if (yFlip)
                        angle.y += directionSwitch ? delta / 4 : -delta / 4;
                    else
                        angle.y += directionSwitch ? -delta / 4 + 180.0f : delta / 4 + 180.0f;

                    if (!bSend)
                    {
                        if (yFlip)
                            angle.y += directionSwitch ? -delta : delta;
                        else
                            angle.y += directionSwitch ? delta : -delta;
                    }
                    else
                        yFlip = !yFlip;
                }
                break;
        case 6:{
            //angle.x = 89.0f;
            Vector edging_head = angle;
            if (GetBestHeadAngle(cmd,angle, bSend))
                angle.y = edging_head.y;
        }
        default:
            break;
    }
    if( bSend ){
        AntiAim::fakeAngle.y = angle.y;
    } else {
        AntiAim::realAngle.y = angle.y;
    }
}

inline float NormalizeYaw(float yaw)
{
    if (yaw > 180)
        yaw -= (round(yaw / 360) * 360.f);
    else if (yaw < -180)
        yaw += (round(yaw / 360) * -360.f);
    
    return yaw;
}

static void DoFakeAngle2(C_BasePlayer *const localplayer, Vector& angle, bool bSend, CUserCmd* cmd)
{
    

        CCSGOAnimState* animState = localplayer->GetAnimState();
    auto feet_yaw_delta = cmd->viewangles.y - NormalizeYaw(localplayer->GetAnimState()->goalFeetYaw);
    float maxDelta = GetMaxDelta(animState);
    static bool yFlip = false;
    static float flLastSentYaw;
     //static bool directionSwitch = false;
    
    int aa_type = vars.misc.FaaY;

    switch (aa_type)
    {
       
            
            case 1:  {
                           static bool yFlip = false;

                           angle.x = 89.0f;
                           if (yFlip)
                               angle.y += manualswitch ? maxDelta / 2 + 45 : -maxDelta / 2 + 45;
                           else
                               angle.y += manualswitch ? -maxDelta / 2 + 45 : maxDelta / 2 + 45;

                           if (!bSend)
                           {
                               if (yFlip)
                                   angle.y += manualswitch ? -maxDelta * 2 : maxDelta * 2;
                               else
                                   angle.y += manualswitch ? maxDelta * 2 : -maxDelta * 2;
                           }
                           else
                               yFlip = !yFlip;
                       } break;
}
}


void DoAntiAim(CUserCmd* cmd, C_BaseCombatWeapon* weapon)
{
    if (!vars.misc.antiaim && !vars.misc.lbybreaker && !vars.misc.desyncenabled && !vars.misc.pitchenabled)
        return;

    Vector oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    
    AntiAim::realAngle = AntiAim::fakeAngle = CreateMove::lastTickViewAngles;

    Vector angle = cmd->viewangles;

    C_BasePlayer* localplayer = (C_BasePlayer*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) pEntList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
        return;
    
    CSWeaponType weaponType = (CSWeaponType)activeWeapon->GetCSWpnData()->m_WeaponType;

    if (weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeWeapon;

        if (csGrenade->GetThrowTime() > 0.f)
            return;
    }

    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 && *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
        return;

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;

    static bool bSend = true;
    bSend = !bSend;

    bool should_clamp = true;
    bool bFlip = false;

    bool needToFlick = false;
    static bool lbyBreak = false;
    static float lastCheck;
    float vel2D = localplayer->GetVelocity().Length2D();//localplayer->GetAnimState()->verticalVelocity + localplayer->GetAnimState()->horizontalVelocity;

     if( vars.misc.lbybreaker ){
           
           if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
               lbyBreak = false;
               lastCheck = pGlobals->curtime;
           } else {
               if( !lbyBreak && ( pGlobals->curtime - lastCheck ) > 0.22 ){
                   cmd->viewangles.y -= vars.misc.lbybreakeroffset;
                   lbyBreak = true;
                   lastCheck = pGlobals->curtime;
                   needToFlick = true;
               } else if( lbyBreak && ( pGlobals->curtime - lastCheck ) > 1.1 ){
                   cmd->viewangles.y -= vars.misc.lbybreakeroffset;
                   lbyBreak = true;
                   lastCheck = pGlobals->curtime;
                   needToFlick = true;
               }
           }
       }
    Vector OldAngles = cmd->viewangles;

    
    if (cmd->buttons & (IN_ATTACK | IN_ATTACK2 | IN_USE) || localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP
            || !localplayer->GetAlive())
            return;

        if (localplayer->GetImmune() || localplayer->GetFlags() & FL_FROZEN)
            return;

        if (!weapon)
            return;

        auto weapon_index = weapon->GetItemDefinitionIndex();
        if ((*weapon_index == WEAPON_GLOCK || *weapon_index == WEAPON_FAMAS) && weapon->GetNextPrimaryAttack() >= pGlobals->curtime)
            return;

        if (!vars.misc.antiaim)
            return;
        

        if (weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
        {
            C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) weapon;
            if(!csGrenade->GetPinPulled()){
                if (csGrenade->GetThrowTime() > 0.f)
                    return;
            }
        }

        static bool broke_lby = false;
        
    if (vars.misc.desyncenabled && !needToFlick)
    {
        DoAntiAimY(localplayer, angle, bSend, cmd);
        DoFakeAngle2(localplayer, angle, bSend,  cmd);
      DoAntiAimX(angle, bSend, should_clamp);
    

       
    

    if( should_clamp ){
        NormalizeAngles(angle);
        ClampAngles(angle);
    }

   cmd->viewangles = angle;

   CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
}
