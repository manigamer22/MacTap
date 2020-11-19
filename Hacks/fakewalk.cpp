//
//  fakewalk.cpp
//  vHook
//
//  Created by Warlauke on 10/20/17.
//  Copyright © 2017 ViKiNG. All rights reserved.
//

#include "fakewalk.h" // Pasted from JingWare

//
//  fakewalk.cpp
//  vHook
//
//  Created by Warlauke on 10/20/17.
//  Copyright © 2017 ViKiNG. All rights reserved.
//
ButtonCode_t keybind2(){
    if (vars.misc.fakewalkkeybind == 0) {
        return KEY_LBRACKET;
    }else if (vars.misc.fakewalkkeybind == 1){
        return KEY_RBRACKET;
    }else if (vars.misc.fakewalkkeybind == 2){
        return KEY_SEMICOLON;
    }else if (vars.misc.fakewalkkeybind == 3){
        return KEY_APOSTROPHE;
    }else if (vars.misc.fakewalkkeybind == 4){
        return KEY_BACKQUOTE;
    }else if (vars.misc.fakewalkkeybind == 5){
        return KEY_COMMA;
    }else if (vars.misc.fakewalkkeybind == 6){
        return KEY_PERIOD;
    }else if (vars.misc.fakewalkkeybind == 7){
        return KEY_SLASH;
    }else if (vars.misc.fakewalkkeybind == 8){
        return KEY_BACKSLASH;
    }else if (vars.misc.fakewalkkeybind == 9){
        return KEY_MINUS;
    }else if (vars.misc.fakewalkkeybind == 10){
        return KEY_EQUAL;
    }else if (vars.misc.fakewalkkeybind == 11){
        return KEY_ENTER;
    }else if (vars.misc.fakewalkkeybind == 12){
        return KEY_TAB;
    }else if (vars.misc.fakewalkkeybind == 13){
        return KEY_CAPSLOCK;
    }else if (vars.misc.fakewalkkeybind == 14){
        return KEY_ESCAPE;
    }else if (vars.misc.fakewalkkeybind == 15){
        return KEY_LSHIFT;
    }else if (vars.misc.fakewalkkeybind == 16){
        return KEY_RSHIFT;
    }else if (vars.misc.fakewalkkeybind == 17){
        return KEY_LALT;
    }else if (vars.misc.fakewalkkeybind == 18){
        return KEY_RALT;
    }else if (vars.misc.fakewalkkeybind == 19){
        return KEY_LCONTROL;
    }else if (vars.misc.fakewalkkeybind == 20){
        return KEY_RCONTROL;
    }
}

void Slowwalk(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    if (!vars.misc.slow_walk)
        return;
    if(vars.misc.fakewalkkey){
       if (!pInputSystem->IsButtonDown(keybind2()))
           return;
   }

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) pEntList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    Vector ViewAngle;
        pEngine->GetViewAngles(ViewAngle);
if ( vars.misc.slowalkmode == 1){
    static Vector oldOrigin = localplayer->GetAbsOrigin( );
        Vector velocity = ( localplayer->GetVecOrigin( )-oldOrigin )
                                                        * (1.f/pGlobals->interval_per_tick);
        oldOrigin = localplayer->GetAbsOrigin( );
        float speed  = velocity.Length( );

    if(speed > vars.misc.slow_walk_amount )
        {
                cmd->forwardmove = 0;
                cmd->sidemove = 0;
                CreateMove::sendPacket = false;
        }
        else {
                CreateMove::sendPacket = true;
        }
}else {
   if (activeWeapon) {


                        float speed = 0.1f;
                                float max_speed = activeWeapon->GetCSWpnData1()->GetMaxPlayerSpeed();
                                float ratio = max_speed / 250.0f;
                                speed *= ratio;


                        cmd->forwardmove *= speed;
                        cmd->sidemove *= speed;
        }
}

}


static bool FirstDuck = false;
int choked;
void FakeDuck(CUserCmd *cmd)
{

    C_BasePlayer* localplayer = (C_BasePlayer*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

   

    if (pInputSystem->IsButtonDown(KEY_X)){
                cmd->buttons |= IN_BULLRUSH;
        int amount = 14;
                if (choked <= amount / 2){
                        cmd->buttons &= ~IN_DUCK;
            if (choked > (amount / 3) + 1 )
                localplayer->GetAnimState()->duckProgress = 0.f;
            else
                localplayer->GetAnimState()->duckProgress = 1.0;
                }else{
                        cmd->buttons |= IN_DUCK;
            localplayer->GetAnimState()->duckProgress = 1.0;
               } if (choked < amount){
            choked++;
                        CreateMove::sendPacket = false;   // choke
        }
                else{
                       CreateMove::sendPacket = true;    // send packet
               choked = 0;
        }

//localplayer->GetAnimState()->duckProgress = 1.0; //memes

}
}

void FakeDuckOverrideView(CViewSetup *pSetup)
{
    if (!vars.misc.fakeduck)
        return;

  if (pInputSystem->IsButtonDown(KEY_X))
        return;

    C_BasePlayer *localplayer = (C_BasePlayer *)pEntList->GetClientEntity(pEngine->GetLocalPlayer());

    if (!localplayer || !localplayer->GetAlive())
        return;

    pSetup->origin.z = localplayer->GetAbsOrigin().z + 64.0f;
}
