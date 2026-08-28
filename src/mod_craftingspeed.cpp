/*
 * Crafting Speed Module for Tortoise (Ported from AzerothCore)
 *
 * Credits:
 *   - FiraVT: Port Maintainer (https://github.com/FiraVT/Craftingspeed)
 *   - Day36512: Original author and concept (https://github.com/Day36512/mod-craftspeed)
 *   - mod-crafting-casttime: Original AzerothCore module
 *   - Penqle & AI: Ported to Tortoise (Tortoise-core)
 *
 * Licensed under GNU AGPL v3
 */

#include "ScriptObjects.h"
#include "Config/Config.h"
#include "SpellMgr.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "Log.h"
#include "Chat.h"
#include "Player.h"
#include "ObjectMgr.h"

/**
 * Identify if a spell is a "crafting" spell.
 * Based on the logic from the original mod-craftspeed.
 */
bool IsCraftingSpell(SpellEntry const* spellEntry)
{
    if (!spellEntry)
        return false;

    // Crafting spells consume reagents. 
    // In Tortoise-core, Reagent[0] is the first reagent.
    if (spellEntry->Reagent[0] <= 0)
        return false;

    // Check for relevant effects
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        uint32 effect = spellEntry->Effect[i];

        switch (effect)
        {
        case SPELL_EFFECT_TRADE_SKILL:
        case SPELL_EFFECT_CREATE_ITEM:
        case SPELL_EFFECT_ENCHANT_ITEM:
        case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
        case SPELL_EFFECT_ENCHANT_HELD_ITEM:
            return true;
        default:
            break;
        }
    }

    return false;
}

class CraftingCastTimeWorldScript : public WorldScript
{
public:
    CraftingCastTimeWorldScript() : WorldScript("mod_crafting_cast_time", { WORLDHOOK_ON_STARTUP }) {}

    /**
     * This hook runs after spells are loaded and initialized.
     * We iterate through all spells and modify those identified as crafting spells.
     */
    void OnStartup() override
    {
        bool enable = sConfig.GetBoolDefault("CraftingCastTime.Enable", true);
        if (!enable)
            return;

        float multiplier = sConfig.GetFloatDefault("CraftingCastTime.Multiplier", 0.25f);
        if (multiplier < 0.0f)
            multiplier = 0.0f;

        sLog.outString(">> [Mod-CraftingSpeed] Modifying crafting spells (Multiplier: %.2f)...", multiplier);

        uint32 modifiedSpells = 0;
        uint32 maxSpellId = sSpellMgr.GetMaxSpellId();

        for (uint32 spellId = 1; spellId < maxSpellId; ++spellId)
        {
            SpellEntry const* spellEntryConst = sSpellMgr.GetSpellEntry(spellId);
            if (!spellEntryConst || !IsCraftingSpell(spellEntryConst))
                continue;

            // Get original base cast time (nullptr passed for unit means no bonuses)
            uint32 originalCastTime = spellEntryConst->GetCastTime(nullptr);
            if (originalCastTime == 0)
                continue;

            uint32 targetCastTime = static_cast<uint32>(originalCastTime * multiplier);
            uint32 bestIndex = 1; // Default to ID 1 (Instant Cast)

            // If we want a non-instant cast time, find the closest match in the DBC store
            if (targetCastTime > 0)
            {
                uint32 minDiff = 0xFFFFFFFF;
                for (uint32 i = 0; i < sSpellCastTimesStore.GetNumRows(); ++i)
                {
                    SpellCastTimesEntry const* entry = sSpellCastTimesStore.LookupEntry(i);
                    if (!entry)
                        continue;

                    // Calculate difference
                    uint32 diff = (entry->CastTime > (int32)targetCastTime) ? 
                                 (entry->CastTime - targetCastTime) : 
                                 (targetCastTime - entry->CastTime);

                    if (diff < minDiff)
                    {
                        minDiff = diff;
                        bestIndex = entry->ID;
                    }

                    if (diff == 0)
                        break;
                }
            }

            // Apply the new casting time index
            const_cast<SpellEntry*>(spellEntryConst)->CastingTimeIndex = bestIndex;
            modifiedSpells++;
        }

        sLog.outString(">> [Mod-CraftingSpeed] Done. Modified %u crafting spells.", modifiedSpells);
    }
};

class CraftingCastTimePlayerScript : public PlayerScript
{
public:
    CraftingCastTimePlayerScript() : PlayerScript("mod_crafting_cast_time_player", { PLAYERHOOK_ON_LOGIN }) {}

    void OnLogin(Player* player) override
    {
        if (sConfig.GetBoolDefault("CraftingCastTime.Enable", true) && 
            sConfig.GetBoolDefault("CraftingCastTime.ShowMessageOnLogin", true))
        {
            float multiplier = sConfig.GetFloatDefault("CraftingCastTime.Multiplier", 0.25f);
            const char* msg = sObjectMgr.GetModuleString("mod-craftingspeed", 1, player->GetSession()->GetSessionDbLocaleIndex());
            ChatHandler(player).PSendSysMessage(msg, multiplier);
        }
    }
};

// Loader function name based on module folder 'mod-craftingspeed'
void Addmod_craftingspeedScripts()
{
    new CraftingCastTimeWorldScript();
    new CraftingCastTimePlayerScript();
}
