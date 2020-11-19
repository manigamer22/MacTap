

#include "fakelag.hpp"
/*static int ticks = 0;
int ticksMax = 16;

void FakeLag(CUserCmd *cmd) {
    if (!vars.misc.fakelag)
        return;
    if(!pEngine->IsInGame())
        return;
    
    C_BasePlayer* localplayer = (C_BasePlayer*)pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    
    if (!localplayer || !localplayer->GetAlive())
        return;
    if (localplayer->GetFlags() & FL_ONGROUND && vars.misc.adaptive)
        return;
    
    
    if (cmd->buttons & IN_ATTACK) {
        CreateMove::sendPacket = true;
        return;
    }
    
    if (ticks >= ticksMax) {
        CreateMove::sendPacket = true;
        ticks = 0;
    } else {
        if (vars.misc.adaptive) {
            int packetsToChoke;
            if (localplayer->GetVelocity().Length() > 0.f) {
                packetsToChoke = (int)((64.f / pGlobals->interval_per_tick) / localplayer->GetVelocity().Length()) + 1;
                if (packetsToChoke >= 15)
                    packetsToChoke = 14;
                if (packetsToChoke < vars.misc.fakelagfactor)
                    packetsToChoke = vars.misc.fakelagfactor;
            } else
                packetsToChoke = 0;
            
            CreateMove::sendPacket = ticks < 16 - packetsToChoke;
        } else
            CreateMove::sendPacket = ticks < 16 - vars.misc.fakelagfactor;
    }
    
    ticks++;
    
}*/
#include <climits>
#ifndef absol
        #define absol(x) x < 0 ? x*-1 : x
#endif
int ticksMax = 50;
int ticks = 0;
 int shift = 0;
bool CheckPeaking(CUserCmd* cmd){
        float forMove = absol(cmd->forwardmove);
        float sideMove = absol(cmd->sidemove);
        if (sideMove > forMove) {
            C_BasePlayer* localplayer = (C_BasePlayer*)pEntList->GetClientEntity(pEngine->GetLocalPlayer());
                if (!localplayer || !localplayer->GetAlive()){
                        return false;
                }

                if ( localplayer->GetVelocity().Length2D() > 100.f )
                        return true;
        }
        return false;
}
void LagSpike(CUserCmd* cmd, int lagTick){
        if (!CheckPeaking){
                ticksMax = 16;
                return;
        }

        ticksMax = 25;
        ticks = 0;


}
void FakeLag(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*)pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    if (!vars.misc.fakelag)
        return;
        int velocity2d = localplayer->GetVelocity().Length2D();
                int max_choke;
    if (pInputSystem->IsButtonDown(KEY_F)){
       static auto should_recharge = true;

        if (should_recharge)
        {
                ++ticks;
                CreateMove::sendPacket = true;

                cmd->tick_count = INT_MAX;
                cmd->forwardmove = 0.0f;
                cmd->sidemove = 0.0f;
                cmd->upmove = 0.0f;
                cmd->buttons &= ~IN_ATTACK;
                cmd->buttons &= ~IN_ATTACK2;

                if (ticks >= 16)
                {
            pCvar->ConsoleDPrintf(("we charged bois"));
                        should_recharge = false;
                       // *(bool*)(*frame_ptr - 0x1C) = true;
                }

        }


        shift = 16;
        should_recharge = true;
        }else
        shift = 0;
            if (cmd->buttons & IN_ATTACK)
            {
                CreateMove::sendPacket = true;
           C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) pEntList->GetClientEntityFromHandle(localplayer->GetActiveWeapon() );

        if (activeWeapon->GetCSWpnData1()->GetWeaponType() != CSWeaponType::WEAPONTYPE_GRENADE)
        max_choke = 25;
            }

        if (vars.misc.adaptive){
        if (velocity2d >= 5.0f)
                {
                        auto dynamic_factor = std::ceil(64.0f / (velocity2d * pGlobals->interval_per_tick));

                        if (dynamic_factor > 16)
                                dynamic_factor = vars.misc.fakelagfactor;

                        max_choke = dynamic_factor;
                }
                else
                        max_choke = vars.misc.fakelagfactor;
        }else if ( !(cmd->buttons & IN_ATTACK)) max_choke = vars.misc.fakelagfactor;
     
         if (ticks >= max_choke){
            CreateMove::sendPacket = true;
            ticks = -1;
        }else{
            CreateMove::sendPacket = false;
        }
    ticks++;
}
