//
//  fakewalk.hpp
//  vHook
//
//  Created by Warlauke on 10/20/17.
//  Copyright © 2017 ViKiNG. All rights reserved.
//
#pragma once

#include "main.h"

void Fakewalk(CUserCmd* cmd, C_BaseEntity* local);
void Slowwalk(CUserCmd* cmd);
void FakeDuck(CUserCmd* cmd);
void FakeDuckOverrideView(CViewSetup *pSetup);
