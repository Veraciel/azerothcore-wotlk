/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_Azuregos
SD%Complete: 90
SDComment: Teleport not included, spell reflect not effecting dots (Core problem)
SDCategory: Azshara
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Say
{
    SAY_TELEPORT            = 0
};

enum Spells
{
    SPELL_MARKOFFROST       = 23182,
    SPELL_MANASTORM         = 21097,
    SPELL_CHILL             = 21098,
    SPELL_FROSTBREATH       = 21099,
    SPELL_REFLECT           = 22067,
    SPELL_CLEAVE            = 8255,     //Perhaps not right ID
    SPELL_ENRAGE            = 23537
};

class boss_azuregos : public CreatureScript
{
public:
    boss_azuregos() : CreatureScript("boss_azuregos") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_azuregosAI(creature);
    }

    struct boss_azuregosAI : public ScriptedAI
    {
        boss_azuregosAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 MarkOfFrostTimer;
        uint32 ManaStormTimer;
        uint32 ChillTimer;
        uint32 BreathTimer;
        uint32 TeleportTimer;
        uint32 ReflectTimer;
        uint32 CleaveTimer;
        uint32 EnrageTimer;
        bool Enraged;

        void Reset() override
        {
            MarkOfFrostTimer = 35000;
            ManaStormTimer = urand(5000, 17000);
            ChillTimer = urand(10000, 30000);
            BreathTimer = urand(2000, 8000);
            TeleportTimer = 30000;
            ReflectTimer = urand(15000, 30000);
            CleaveTimer = 7000;
            EnrageTimer = 0;
            Enraged = false;
        }

        void EnterCombat(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (TeleportTimer <= diff)
            {
                Talk(SAY_TELEPORT);
                ThreatContainer::StorageType threatlist = me->getThreatMgr().getThreatList();
                for (auto i = threatlist.begin(); i != threatlist.end(); ++i)
                {
                    Unit* unit = ObjectAccessor::GetUnit(*me, (*i)->getUnitGuid());
                    if (unit && (unit->GetTypeId() == TYPEID_PLAYER))
                    {
                        DoTeleportPlayer(unit, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 3, unit->GetOrientation());
                    }
                }

                DoResetThreat();
                TeleportTimer = 30000;
            }
            else TeleportTimer -= diff;

            //        //MarkOfFrostTimer
            //        if (MarkOfFrostTimer <= diff)
            //        {
            //            DoCastVictim(SPELL_MARKOFFROST);
            //            MarkOfFrostTimer = 25000;
            //        } else MarkOfFrostTimer -= diff;

            //ChillTimer
            if (ChillTimer <= diff)
            {
                DoCastVictim(SPELL_CHILL);
                ChillTimer = urand(13000, 25000);
            }
            else ChillTimer -= diff;

            //BreathTimer
            if (BreathTimer <= diff)
            {
                DoCastVictim(SPELL_FROSTBREATH);
                BreathTimer = urand(10000, 15000);
            }
            else BreathTimer -= diff;

            //ManaStormTimer
            if (ManaStormTimer <= diff)
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                    DoCast(target, SPELL_MANASTORM);
                ManaStormTimer = urand(7500, 12500);
            }
            else ManaStormTimer -= diff;

            //ReflectTimer
            if (ReflectTimer <= diff)
            {
                DoCast(me, SPELL_REFLECT);
                ReflectTimer = urand(20000, 35000);
            }
            else ReflectTimer -= diff;

            //CleaveTimer
            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 7000;
            }
            else CleaveTimer -= diff;

            //EnrageTimer
            if (HealthBelowPct(26) && !Enraged)
            {
                DoCast(me, SPELL_ENRAGE);
                Enraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_azuregos()
{
    new boss_azuregos();
}
