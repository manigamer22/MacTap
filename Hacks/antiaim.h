//
//  antiaim.hpp
//  vHook
//
#pragma once

#include "main.h"

void DoAntiAim(CUserCmd* cmd, C_BaseCombatWeapon* weapon);


namespace AntiAim
{
    extern Vector realAngle;
    extern Vector fakeAngle;
}
