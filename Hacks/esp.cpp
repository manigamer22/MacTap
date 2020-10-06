#include "esp.h"
#include "../Hacks/antiaim.h"
#include "../Backtrack.hpp"

void DrawSkeleton(C_BaseEntity* pEntity, Color color){
    
    studiohdr_t* pStudioModel = pModelInfo->GetStudioModel( pEntity->GetModel() );
    
    if ( pStudioModel ) {
        
        static matrix3x4_t pBoneToWorldOut[128];
        
        if ( pEntity->SetupBones( pBoneToWorldOut, 128, 256, 0) )
        {
            for ( int i = 0; i < pStudioModel->numbones; i++ )
            {
                mstudiobone_t* pBone = pStudioModel->pBone( i );
                
                if ( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
                    continue;
                
                Vector vBone1 = pEntity->GetBonePosition(i);
                Vector vBoneOut1;
                
                Vector vBone2 = pEntity->GetBonePosition(pBone->parent);
                Vector vBoneOut2;
                
                if(WorldToScreen(vBone1, vBoneOut1) && WorldToScreen(vBone2, vBoneOut2)) {
                    draw->drawline(vBoneOut1.x, vBoneOut1.y, vBoneOut2.x, vBoneOut2.y, color);
                }
            }
        }
    }
}


Color GetColorBase(Color& col)
{
    return Color(col.r(), col.g(), col.b(), 255);
}



void DrawHealthbar(int x, int y, int w, int h, int health, Color color)
{
    if(health > 100)
    {
        health = 100;
    }
    int hw = h - ((h) * health) / 100;
    draw->fillrgba(x, y - 1, w, h + 2, Color(0, 0, 0, 200));
    draw->fillrgba(x, y + hw - 1, w, h - hw + 2, color);
    draw->drawbox(x, y - 1, w, h + 2, Color(0, 0, 0, 200));
    
    
}
void drawHealthBar(int x, int y, int w, int h, int health) {
    int Red = 255 - (health*2.55);
    int Green = health*2.55;
    
    int hw = h - ((h) * health) / 100;
    draw->fillrgba(x, y - 1, w, h + 2, Color(0, 0, 0, 200));
    draw->fillrgba(x, y + hw - 1, w, h - hw + 2, Color(Red, Green, 0, 180));
    draw->drawbox(x, y - 1, w, h + 2, Color(0, 0, 0, 200));
}




void box3d(C_BaseEntity* entity, Color color) {
    
    Vector vOrigin = entity->GetVecOrigin();
    Vector min = entity->GetCollideable()->OBBMins() + vOrigin;
    Vector max = entity->GetCollideable()->OBBMaxs() + vOrigin;
    
    Vector points[] = { Vector(min.x, min.y, min.z),
        Vector(min.x, max.y, min.z),
        Vector(max.x, max.y, min.z),
        Vector(max.x, min.y, min.z),
        Vector(min.x, min.y, max.z),
        Vector(min.x, max.y, max.z),
        Vector(max.x, max.y, max.z),
        Vector(max.x, min.y, max.z) };
    
    int edges[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
    };
    
    for (auto it : edges) {
        Vector p1, p2;
        if (pOverlay->ScreenPosition(points[it[0]], p1) || pOverlay->ScreenPosition(points[it[1]], p2))
            return;
        
    }
}

C_BaseCombatWeapon* getWeapon(C_BaseEntity* local) {
    auto hWeapon = local->GetActiveWeapon();
    if (!hWeapon)
        return;
    
    return (C_BaseCombatWeapon*)pEntList->GetClientEntityFromHandle(hWeapon);
}
float get_distance(Vector a, Vector b)
{
    return sqrt( pow( a.x - b.x, 2 ) + pow( a.y - b.y, 2 ) + pow( a.z - b.z, 2 ) ) * 0.0254f;
}

void DrawDroppedWeapons(C_BaseCombatWeapon* weapon) {
    
    C_BaseEntity* local = pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    
    Vector vOrig = weapon->GetVecOrigin();
    int owner = weapon->GetOwner();
    Vector localpos = local->GetEyePosition();
    
    
    if (owner > -1 || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
        return;
    
    if(get_distance(vOrig, localpos) > 15)
        return;
    
    
    string model = GetWeaponName(weapon);
    int ammo     = weapon->GetAmmo();
    
    if (ammo > 0)
        model += " | " + to_string(ammo);
    
    boxstruct wBox;
    
    if(DrawPlayerBox(weapon, wBox)) {
        draw->drawstring(wBox.x + wBox.w / 2, wBox.y, Color::White(), espfont, model.c_str(), true);
    }
}


void grenadeESP(C_BaseEntity* entity){
    
    if(!vars.visuals.grenade)
        return;
    
    if(!strstr(entity->GetClientClass()->m_pNetworkName, "Projectile"))
        return;
    
    const model_t* model = entity->GetModel();
    
    if(!model)
        return;
    
    studiohdr_t* hdr = pModelInfo->GetStudioModel(model);
    
    if(!hdr || (!strstr(hdr->name, "thrown") && !strstr(hdr->name, "dropped")))
        return;
    
    Color color;
    const char* name = nullptr;
    
    IMaterial* mats[32];
    pModelInfo->GetModelMaterials(model, hdr->numtextures, mats);
    for(int i = 0; i < hdr->numtextures; i++){
        IMaterial* mat = mats[i];
        
        if(!mat)
            continue;
        
        if(strstr(mat->GetName(), "flashbang")){
            color = Color(255, 235, 59, 255);
            name = "p";
            break;
        }
        if(strstr(mat->GetName(), "smoke")){
            color = Color(97, 97, 97, 255);
            name = "O";
            break;
        }
        if(strstr(mat->GetName(), "decoy")){
            color = Color(97, 97, 97, 255);
            name = "l";
            break;
        }
        if(strstr(mat->GetName(), "m67_grenade") || strstr(mat->GetName(), "hegrenade")){
            color = Color(244, 67, 54, 255);
            name = "t";
            break;
        }
        if(strstr(mat->GetName(), "incendiary")){
            color = Color(244, 67, 54, 255);
            name = "v";
            break;
        }
        if(strstr(mat->GetName(), "molotov")){
            color = Color(244, 67, 54, 255);
            name = "D";
        }
    }
    
    boxstruct nBox;
    
    if(DrawPlayerBox(entity, nBox)){
        draw->drawstring(nBox.x + nBox.w + 4, nBox.y - (nBox.h/2), Color::Yellow(), csgo_icons, name);
        draw->BoxEspShit(nBox.x, nBox.y, nBox.w, nBox.h, Color::White());
    }
        box3d(entity, color);
}

void DrawBombPlanted(C_BaseEntity* local, C_BasePlantedC4* bomb)
{
    float flBlow    = bomb->GetBombTime();
    float bombTimer = flBlow - pGlobals->curtime;
    if(!bomb->IsBombTicking() || bomb->IsBombDefused() || bombTimer <= 0.1f)
        return;
    
    boxstruct Box;
    
    if(!DrawPlayerBox(bomb, Box))
        return;
    
    int x, y;
    pEngine->GetScreenSize(x, y);//getscreensize in pixels for width of countdown bars
    
    
    float fldefuse = bomb->GetDefuseTime();//time bomb is expected to defuse. if defuse is cancelled and started again this will be changed to the new value
    float DefuseTimeRemaining = fldefuse - pGlobals->curtime;//subtract current time to get time remaining
    char TimeToExplode[64]; sprintf(TimeToExplode, "Explode in: %.1f", bombTimer);//Text we gonna display for explosion
    
    char TimeToDefuse[64]; sprintf(TimeToDefuse, "Defuse in: %.1f", DefuseTimeRemaining);
    
    char buffer[128];
    
    if((local->HasDefuser() && flBlow < 5.25f) || (!local->HasDefuser() && flBlow < 10.25f))
        sprintf(buffer, "No time %.f", bombTimer);
    
    if((local->HasDefuser() && flBlow > 5.25f) || (!local->HasDefuser() && flBlow > 10.25f)){
        sprintf(buffer, "Bomb %.f", bombTimer);
        
        
    }
    
    if (bomb->IsBombDefused() || !bomb->IsBombTicking() || bombTimer <= 0.f)
            sprintf(buffer, "Bomb");
    
        draw->drawstring(Box.x, Box.y, Color(255, 0, 0, 255), espfont, buffer);
    
    
    
}

void DrawOtherESP() {
    // WIP
    if(!vars.visuals.enabled)
        return;
    
    if(vars.misc.antiscreenshot && pEngine->IsTakingScreenshot())
        return;
    
    int getscreenw, getscreenh;
    pEngine->GetScreenSize(getscreenw, getscreenh);
    
    C_BaseEntity* local = pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    
    
    for(int i = 0; i < pEntList->GetHighestEntityIndex(); i++)
    {
        auto* entity = pEntList->GetClientEntity(i);
        
        if(!entity)
            continue;
        
        int classID = entity->GetClientClass()->m_ClassID;
        C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)entity;
        C_BasePlantedC4* pC4 = (C_BasePlantedC4*)entity;
        
        
        if(!weapon || !pC4)
            continue;
        
        if(vars.visuals.weapons) {
            if(classID != EClassIds::CBaseWeaponWorldModel) {
                if (strstr(entity->GetClientClass()->m_pNetworkName, "Weapon") || classID == EClassIds::CDEagle || classID == EClassIds::CAK47) {
                    DrawDroppedWeapons(weapon);
                }
            }
        }
        
        // Draw planted bomb
        
        if(vars.visuals.bombtimer && classID == CPlantedC4)
            DrawBombPlanted(local, pC4);
            //DrawBombBar(local, pC4);
        

        
        if (vars.visuals.grenade)
            grenadeESP(entity);
        
    }
}



void DrawSpread(C_BaseEntity* local) {
    
    if(!pEngine->IsInGame())
        return;
    
    if(!vars.misc.spreadcrosshair)
        return;
    
    if(!local)
        return;
    
    auto weapon = GetActiveWeapon(local);
    
    if(!weapon)
        return;
    
    static int Height, Width;
    pEngine->GetScreenSize(Width, Height);
    
    if ( vars.misc.spreadcrosshair ) {
        float cone = weapon->GetSpread() + weapon->GetInaccuracy();
        if ( cone > 0.0f ) {
            float radius = ( cone * Height ) / 1.5f;
            
            draw->DrawFilledCircle(Vector2D(Width/2, Height/2), Color(24, 24, 24, 124), radius, 60);
            
            
        }
    }
}

void DrawAngles(C_BaseEntity* local)
{
    
    if(!pEngine->IsInGame())
        return;
    
    if(!vars.visuals.antiaim_indicator )
        return;
    
    if(!vars.visuals.aaline)
        return;
    
    Vector src3D, dst3D, forward, src, dst;
    trace_t tr;
    Ray_t ray;
    CTraceFilter filter;
    
    filter.pSkip = local;
    src3D = local->GetVecOrigin();
    
    AngleVectors(Vector(0, AntiAim::fakeAngle.y, 0), &forward);
    dst3D = src3D + (forward * 45.f);
    
    ray.Init(src3D, dst3D);
    
    pEngineTrace->TraceRay(ray, 0, &filter, &tr);
    
    if (!WorldToScreen(src3D, src) || !WorldToScreen(tr.endpos, dst))
        return;
    
    draw->Line(src.x, src.y, dst.x, dst.y, Color::Blue());
    
    if (vars.visuals.anglelinenames)
        draw->drawstring(dst.x, dst.y, Color::Blue(), espfont, "FAKE");
    
    
    AngleVectors(Vector(0, local->GetLowerBodyYawTarget(), 0), &forward);
    dst3D = src3D + (forward * 45.f);
    
    ray.Init(src3D, dst3D);
    
    pEngineTrace->TraceRay(ray, 0, &filter, &tr);
    
    if (!WorldToScreen(src3D, src) || !WorldToScreen(tr.endpos, dst))
        return;
    
    draw->Line(src.x, src.y, dst.x, dst.y, Color(0, 255, 0, 255));
    
    if (vars.visuals.anglelinenames)
        draw->drawstring(dst.x, dst.y, Color(0, 255, 0, 255), espfont, "REAL");
    AngleVectors(Vector(0, local->GetLowerBodyYawTarget(), 0), &forward);
       dst3D = src3D + (forward * 45.f);
       
       ray.Init(src3D, dst3D);
       
       pEngineTrace->TraceRay(ray, 0, &filter, &tr);
       
       if (!WorldToScreen(src3D, src) || !WorldToScreen(tr.endpos, dst))
           return;
       
       draw->Line(src.x, src.y, dst.x, dst.y, Color(135, 235, 169));
       
       if (vars.visuals.anglelinenames)
           draw->drawstring(dst.x, dst.y,Color( 135, 235, 169 ), espfont, "LBY");
}
//}

void LBYInfo(C_BaseEntity* local)
{
    if(!vars.visuals.antiaim_indicator)
        return;
    
    int w, h;
    pEngine->GetScreenSize(w, h);
    
    if (pEngine->IsConnected() && pEngine->IsInGame() && local->GetAlive())
    {
        char angle[50];
       // sprintf(angle, "%f", local->GetLowerBodyYaw());
        
        if (local->GetLowerBodyYawTarget() >= -35 && local->GetLowerBodyYawTarget() <= 35)
        {
            draw->drawstring(20, h - 50, Color(255, 0, 30, 255), lbyfont, "LBY");
        }
        else
        {
            if (local->GetLowerBodyYawTarget() > 120)
            {
                draw->drawstring(20, h - 50, Color(0, 250, 80, 255), lbyfont, "LBY"); // LBY
                
            }
            else
            {
                draw->drawstring(20, h - 50, Color(0, 170, 250, 255), lbyfont, "LBY");
                
            }
        }
      //if (!(local->GetFlags() & FL_ONGROUND) && local->GetHealth() > 0)
         if (!pEngine->GetNetChannel()->m_nChokedPackets)
        {
                   

                   static auto is_breaking_lagcomp = false;

                   static auto last_origin = local->GetVecOrigin();
                   if (local->GetVecOrigin() != last_origin)
                   {
                       is_breaking_lagcomp = (local->GetVecOrigin()).Length2DSqr() > 4096;
                       last_origin = local->GetVecOrigin();
                           draw->drawstring(20, h - 80, Color(0, 250, 80, 255), lbyfont, "LC"); // LBY
                   }
                 /*  static Vector pos = local->GetVecOrigin();
                   if (pos.LengthSqr() > 4096.f) {
                        draw->drawstring(20, h - 80, Color(0, 250, 80, 255), lbyfont, "LC"); // LBY
                       */
                       //int(SETTINGS::settings.arrows_col[0] * 255.f), int(SETTINGS::settings.arrows_col[1] * 255.f), int(SETTINGS::settings.arrows_col[2] * 255.f)
                   }

                 //  pos = local->GetVecOrigin();
    }
}

void DrawWatermark(){
        if (!vars.misc.watermark)
                return;
    static int counter = 0;
    static float colors[3] = { 1.f, 0.f, 0.f };
    
    if (colors[counter] >= 1.0f)
    {
        colors[counter] = 1.0f;
        counter += 1;
        if (counter > 2)
            counter = 0;
    }
    else
    {
        int prev = counter - 1;
        if (prev < 0) prev = 2;
        colors[prev] -= 0.05f;
        colors[counter] += 0.05f;
    }
    C_BaseEntity* local = (C_BaseEntity*)pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    int w, h;
    pEngine->GetScreenSize(w, h);
//int lag = TIME_TO_TICKS(local->GetSimulationTime() - local->GetOldSimulationTime());
            //    int woop = lag;
//std::string bombStr = std::to_string(woop );

             //   Draw::AddRectFilled(1653 + 73, 2, 1653 + 260, 30, Color(40, 40, 40, 225));
    //  Draw::AddRectFilled(1653 + 5 + 73, 1 + 5, 1653 + 255, 30 - 5, Color(10, 10, 10, 225));
   // Draw::AddLine(1653 + 6 + 73, 1 + 5, 1653 + 254, 1 + 5, Color::Red());
    draw->FilledRectangle(10, 2, 10 + 160, 30, Color(40, 40, 40, 225));
    draw->FilledRectangle(10 + 5, 1 + 5, 10 + 155, 30 - 5, Color(10, 10, 10, 225));
    draw->drawline(10 + 5, 1 + 5, 10 + 154, 1 + 5, Color(colors[0] * 255, colors[1] * 255, colors[2] * 255, 255));
    const auto time{ std::time(nullptr) };
    const auto localTime{ std::localtime(&time) };

    const auto timeString{ '[' + std::to_string(localTime->tm_hour) + ':' + std::to_string(localTime->tm_min) + ':' + std::to_string(localTime->tm_sec) + ']' };
  //  SetClanTag(timeString.c_str(), timeString.c_str());
    
               // draw->RectOutlined(1653 - 1 + 73, 1, 1653 + 261, 31, Color(200, 200, 200, 50));
                //draw->AddRect(1652 + 5 + 73, 1 + 5, 1653 + 256, 31 - 5, Color(200, 200, 200, 50));

                int fps = static_cast< int >( 1.f / pGlobals->frametime );
                std::string fps_string = std::to_string(fps);
//std::string name = "eyehook | " + fps_string + " fps | 39ms";
    std::string name = "MacTap.cc | " + fps_string + " fps | " + timeString.c_str();

   // draw->drawstring(1653 + 10 + 73, 11, name.c_str(), Color( 255, 255, 255, 255 ) );
    //draw->drawstring(1653 + 10 + 73 + 21, 11, "hook", Color( 255, 166, 14, 255 ) );
   // draw->drawstring(1653 + 10 + 73 , h - 50, Color(255, 0, 30, 255), lbyfont, "LBY");
    draw->drawstring(10 + 5, 10, Color::White(), hhhfont, name.c_str());

}


void DrawPlayerESP()
{
    if(!vars.visuals.enabled)
        return;
    if(vars.misc.antiscreenshot && pEngine->IsTakingScreenshot())
        return;
    
    int getscreenw, getscreenh;
    pEngine->GetScreenSize(getscreenw, getscreenh);
    
    auto* local = pEntList->GetClientEntity(pEngine->GetLocalPlayer());
    
    static int counter = 0;
    static float colors[3] = { 1.f, 0.f, 0.f };
    
    if (colors[counter] >= 1.0f)
    {
        colors[counter] = 1.0f;
        counter += 1;
        if (counter > 2)
            counter = 0;
    }
    else
    {
        int prev = counter - 1;
        if (prev < 0) prev = 2;
        colors[prev] -= 0.05f;
        colors[counter] += 0.05f;
    }
    
    for(int i = 0; i < pEntList->GetHighestEntityIndex(); i++)
    {
        auto* entity = pEntList->GetClientEntity(i);
        
        if(!entity)
            continue;
        
        if(entity->GetHealth() < 1)
            continue;
        
        if(entity->GetDormant())
            continue;
        
        if(entity->GetTeam() != 2 && entity->GetTeam() != 3)
            continue;
        
        if(vars.visuals.enemyonly && entity->GetTeam() == local->GetTeam())
            continue;
        
        player_info_t info;
        
        pEngine->GetPlayerInfo(i, &info);
        
        auto vis = IsVisible(entity);
        
        Color playercolor = [&]() -> Color
        {
            if(entity->GetTeam() == 3)
                return vis ? GetColorBase(vars.colors.ctbox) : GetColorBase(vars.colors.ctbox_ign);
            else if(entity->GetTeam() == 2)
                return vis ? GetColorBase(vars.colors.tbox) : GetColorBase(vars.colors.tbox_ign);
        }();
        
        boxstruct players;
        char getYaw[255], getPitch[255];
        
        sprintf(getYaw, "Y: %1.0f", entity->GetYawRotation());
        sprintf(getPitch, "X: %1.0f", entity->GetHeadRotation());
        
        Vector vW2s;
        Vector vvvv;
        
        if(DrawPlayerBox(entity, players)) {
            
            /* Draw box */
            
            std::string health = std::to_string(entity->GetHealth()).c_str();
            std::string hp = "HP: ";
            hp += health;
            
            if(vars.visuals.box)
                draw->BoxEspShit(players.x, players.y, players.w, players.h, playercolor);
            
            /* Draw name */
            if(vars.visuals.name)
                draw->drawstring(players.x + players.w / 2, players.y - 12, Color::White(), espfont, info.name, true);
            
            /* Draw health bar */
            if(vars.visuals.health)
                drawHealthBar(players.x - 5, players.y, 3, players.h, entity->GetHealth());
            
            
            /* Draw health as text */
            if(vars.visuals.healthtext)
                draw->drawstring((players.x + 3) + players.w + 3, players.y + 1, Color::White(), espfont, health.c_str(), true);
            
            /* Draw amour bar */
            if(vars.visuals.armour){
                DrawHealthbar(players.x, players.y + players.h + 3, players.w, 2, entity->GetArmor(), Color(72, 136, 189, 255));
                
                
                if (entity->GetArmor() > 0)
                {
                    char hp[50];
                    
                    draw->drawstring(players.x + players.w + 1, players.y + 10, Color(255, 255, 255, 255), espfont, hp);
                    
                    if (entity->HasHelmet())
                        draw->drawstring(players.x + players.w + 1, players.y + 0, Color(255, 255, 255, 255), espfont, "HK");
                    else
                        draw->drawstring(players.x + players.w + 1, players.y + 0, Color(210, 210, 210, 255), espfont, "Kevlar");
                }
                else
                    draw->drawstring(players.x + players.w + 1, players.y + 0, Color(0, 210, 255, 255), espfont, "No Armor");
                
            }
            
            if(vars.visuals.active) {
                string active = GetWeaponName(getWeapon(entity));
                string active1 = GetWeaponName1(getWeapon(entity));
                Vector2D weaponTextSize = draw->GetTextSize( active.c_str(), espfont );
                Vector2D weaponTextSize1 = draw->GetTextSize( active1.c_str(), csgo_icons );
                draw->drawstring((players.x +  (players.w / 2) - (weaponTextSize.x / 2)), players.y + players.h + 8, Color::White(), espfont, active.c_str());
                 draw->drawstring((players.x +  (players.w / 2) - (weaponTextSize1.x / 2)), players.y + players.h + 18, Color::White(), csgo_icons, active1.c_str());
                
                
            }
            
            /* Draw snaplines */
            if(vars.visuals.snapline)
                draw->drawline(getscreenw / 2, getscreenh / 2, players.x + players.w / 2, players.y + players.h, Color::Red());
            
            /* Draw skeleton */
            if(vars.visuals.skeleton)
                DrawSkeleton(entity, Color(colors[0] * 255, colors[1] * 255, colors[2] * 255, 255));
            
            
            if(entity->IsDefusing())
                          draw->drawstring(players.x + players.w / 2, players.y - 27, Color::Blue(), espfont, "Defuse", true);
                      
                      if(entity->IsGrabbingHostage())
                          draw->drawstring(players.x + players.w / 2, players.y - 27, Color::Blue(), espfont, "Rescure", true);
                      
                      if(entity->IsRescuing())
                          draw->drawstring(players.x + players.w / 2, players.y - 27, Color::Blue(), espfont, "Carry", true);
                      
                      if((entity->IsScoped()))
                          draw->drawstring(players.x + players.w / 2, players.y - 27, Color::LightBlue(), espfont, "Zoom", true);
                      
                      if((entity->GetFlashDuration() - pGlobals->curtime > 2.0f))
                          draw->drawstring(players.x + players.w / 2, players.y - 27, Color::Yellow(), espfont, "Flashed");
             
             
                     
                     // draw->drawstring(players.x + players.w, players.y + 25, Color::White(), espfont, getPitch);
             
        }
        if (entity && entity != local && !entity->GetDormant())
        {
            if (entity->GetAlive())
            {
                if (vars.aimbot.backtrack)
                {
                    if (local->GetAlive())
                    {
                        for (int t = 0; t < 12; ++t)
                        {
                            Vector screenbacktrack[64][12];
                            
                            if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > local->GetSimulationTime())
                            {
                                if (WorldToScreen(headPositions[i][t].hitboxPos, screenbacktrack[i][t]))
                                {
                                    
                                    pSurface->DrawSetColor(Color::Black());
                                    pSurface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);
                                    
                                    
                                }
                            }
                        }
                    }
                    else
                    {
                        memset(&headPositions[0][0], 0, sizeof(headPositions));
                    }
                }
            }
        }
        
    }
}



/* Display menu */
auto pwnmymenu() -> void {
    
    if(pInputSystem->IsButtonDown(KEY_LALT)) {
        vars.menu = !vars.menu;
    }
    
}
