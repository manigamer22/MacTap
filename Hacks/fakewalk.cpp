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

void Fakewalk(CUserCmd* cmd, C_BaseEntity* local)
{
    
    if(!vars.aimbot.fakewalk)
        return;
    
    if(!pEngine->IsInGame())
        return;
    
    C_BaseEntity* localplayer = (C_BaseEntity*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if(!localplayer)
        return;
    
    if(vars.misc.fakewalkkey){
        if (!pInputSystem->IsButtonDown(keybind2()))
            return;
    }
    
    if(vars.aimbot.fakewalktype == 1){
        static int iChoked = -1;
        iChoked++;
        
        if (iChoked < 3)
        {
            CreateMove::sendPacket = false;
            cmd->tick_count += 10;
            cmd += 7 + cmd->tick_count % 2 ? 0 : 1;
            cmd->forwardmove = cmd->sidemove = 0.f;
        }else{
            CreateMove::sendPacket = true;
            iChoked = -1;
            pGlobals->frametime *= (local->GetVelocity().Length2D()) / 1.f;
            cmd->buttons |= local->GetMoveType() == IN_FORWARD;
        }
    }
    
    if(vars.aimbot.fakewalktype == 2){
        static int iChoked = 0;
        
        iChoked = iChoked > 7  ? 0 : iChoked + 1;
        cmd->forwardmove = iChoked < 2 || iChoked > 5 ? 0 : cmd->forwardmove;
        cmd->sidemove = iChoked < 2 || iChoked > 5 ? 0 : cmd->sidemove;
        
        CreateMove::sendPacket = iChoked < 1;
    }
    
    if(vars.aimbot.fakewalktype == 3){
        static int iChoked = -1;
        iChoked++;
        
        if (iChoked < 1)
        {
            CreateMove::sendPacket = false;
            cmd->tick_count += 555;
            cmd->command_number += 7 + cmd->tick_count % 10 ? 0 : 1; // 5
            cmd->forwardmove = cmd->sidemove = 0.f;
        }
        else
        {
            CreateMove::sendPacket = true;
            iChoked = -1;
            pGlobals->frametime *= (local->GetVelocity().Length2D()) / 5.f; // 10
            cmd->buttons |= local->GetMoveType() == IN_FORWARD;
        }
    }
    
}

void Slowwalk(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    if (!vars.misc.slow_walk)
        return;
    if (!pInputSystem->IsButtonDown(KEY_LSHIFT))
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) pEntList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
        if (activeWeapon) {


                        float speed = 0.1f;
                                float max_speed = activeWeapon->GetCSWpnData1()->GetMaxPlayerSpeed();
                                float ratio = max_speed / 250.0f;
                                speed *= ratio;
                        


                        cmd->forwardmove *= speed;
                        cmd->sidemove *= speed;
        }
}

void FakeDuck(CUserCmd *cmd)
{

    if (!pInputSystem->IsButtonDown(KEY_X))
        return;

    CreateMove::sendPacket = false;

    static bool counter = false;
    static int counters = 0;

    if (counters % 14 == 0)
        counter = true;
    else if (counters % 14 == 6)
        CreateMove::sendPacket = true;
    else if (counters % 14 == 7)
        counter = false;

    counters++;

    if (counter)
        cmd->buttons |= IN_DUCK;
    else
        cmd->buttons &= ~IN_DUCK;
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
