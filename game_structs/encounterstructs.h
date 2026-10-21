#pragma once
#include "guistructs.h"

enum class EncounterFlags : uint32_t
{
	NoEscape = 1 << 0,
	bit1 = 1 << 1,
	bit2 = 1 << 2,
	ForceNeutralAdvantage = 1 << 3,
	ForceEnemyAdvantage = 1 << 4,
	LoadBattleBFL = 1 << 5,
	bit6 = 1 << 6,
	DisableEnemyEXPScaling = 1 << 7,
	LoadBattleScript = 1 << 8,
	NeutralAdvantageEnemyFirst = 1 << 9,
	NoNavigator = 1 << 10,
	ForcePlayerAdvantage = 1 << 11,
	EnemyDoesntDisappear = 1 << 12,
	NoHoldUp = 1 << 13,
	DisablePositionHack = 1 << 14,
	PreventKnockdown = 1 << 15,
	bit16 = 1 << 16,
	bit17 = 1 << 17,
	bit18 = 1 << 18,
	NonNegotiable = 1 << 19,
	bit20 = 1 << 20,
	bit21 = 1 << 21,
	bit22 = 1 << 22,
	bit23 = 1 << 23,
	bit24 = 1 << 24,
	bit25 = 1 << 25,
	bit26 = 1 << 26,
	bit27 = 1 << 27,
	bit28 = 1 << 28,
	bit29 = 1 << 29,
	bit30 = 1 << 30,
	bit31 = 1 << 31
};

template<>
struct magic_enum::customize::enum_range<EncounterFlags>
{
	static constexpr bool is_flags = true;
};

struct BattleUnit
{
	uint16_t unitID;
};
IMGUI_REFLECT(BattleUnit, unitID);

struct EnemyReplacementData
{
	BattleUnit enemyID;
	uint8_t replacementChance;
	std::array<uint8_t, 5> slotChance;
};
IMGUI_REFLECT(EnemyReplacementData, enemyID, replacementChance, slotChance);

struct DisasterShadowData
{
	uint8_t disasterChance;
	std::array<uint8_t, 5> slotChance;
	uint8_t maxDisasterShadows;
};
IMGUI_REFLECT(DisasterShadowData, disasterChance, slotChance, maxDisasterShadows);

struct EnemyEncounter
{
	static constexpr uint16_t ENCOUNTER_COUNT = 1000;

	static void SwapBytes(std::array<EnemyEncounter, ENCOUNTER_COUNT>* arr)
	{
		for (int i = 0; i < sizeof(*arr); i += sizeof(EnemyEncounter))
		{
			DWORD_PTR byteBase = (DWORD_PTR)arr + i;

			uint32_t* bit32Swap = (uint32_t*)byteBase;
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			uint16_t* bit16Swap = (uint16_t*)(byteBase + 0x8);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0xa);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0xc);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0xe);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x10);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x12);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x14);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x4);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x6);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x16);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x18);
			*bit16Swap = _byteswap_ushort(*bit16Swap);
		}
	}

	EncounterFlags flags;
	uint16_t field4;
	uint16_t field6;
	std::array<BattleUnit, 5> battleUnits;
	uint16_t majorFieldID;
	uint16_t minorFieldID;
	uint16_t musicID;
	EnemyReplacementData enemyReplacementData;
	DisasterShadowData disasterShadowData;
	uint8_t field16;
	uint8_t field17;
	uint8_t field18;
	uint8_t field19;
	uint8_t field20;
};
IMGUI_REFLECT(EnemyEncounter, flags, field4, field6, battleUnits, majorFieldID, minorFieldID, musicID, enemyReplacementData, disasterShadowData,
	field16, field17, field18, field19, field20);

enum class PartySlot : uint16_t
{
	AnyOrNone,
	Joker,
	Ryuji,
	Morgana,
	Ann,
	Yusuke,
	Makoto,
	Haru,
	Futaba,
	Akechi,
	Kasumi
};

struct ForcedPartyData
{
	static void SwapBytes(std::array<ForcedPartyData, EnemyEncounter::ENCOUNTER_COUNT>* arr)
	{
		for (int i = 0; i < sizeof(*arr); i += sizeof(ForcedPartyData))
		{
			DWORD_PTR byteBase = (DWORD_PTR)arr + i;

			uint16_t* bit16Swap = (uint16_t*)(byteBase);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x2);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x4);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x6);
			*bit16Swap = _byteswap_ushort(*bit16Swap);
		}
	}

	std::array<PartySlot, 4> partySlot;
};
IMGUI_REFLECT(ForcedPartyData, partySlot);

struct EncounterBonus
{
	uint32_t target;
	uint32_t type;
	float multiplier;
};
IMGUI_REFLECT(EncounterBonus, target, type, multiplier);

struct ItemID
{
	uint16_t itemID;
};
IMGUI_REFLECT(ItemID,itemID);

struct EncounterReward
{
	uint32_t requiredScore;
	uint16_t _1;
	uint16_t _2;
	ItemID itemID;
	uint16_t _3;
};
IMGUI_REFLECT(EncounterReward, requiredScore, itemID);

struct ChallengeBattle
{
	static constexpr uint16_t CHALLENGE_COUNT = 16;

	static void SwapBytes(std::array<ChallengeBattle, CHALLENGE_COUNT>* arr)
	{
		for (int i = 0; i < sizeof(*arr); i += sizeof(ChallengeBattle))
		{
			DWORD_PTR byteBase = (DWORD_PTR)arr + i;

			uint16_t* bit16Swap = (uint16_t*)(byteBase);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			bit16Swap = (uint16_t*)(byteBase + 0x2);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			uint32_t* bit32Swap = (uint32_t*)(byteBase + 0x4);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x8);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit16Swap = (uint16_t*)(byteBase + 0xc);
			*bit16Swap = _byteswap_ushort(*bit16Swap);

			//bit32Swap = (uint32_t*)(byteBase + 0x10);
			//*bit32Swap = _byteswap_ulong(*bit32Swap);

			DWORD_PTR byteBase2 = (DWORD_PTR)arr + i + 0x18;
			DWORD_PTR j_offset = 0;
			for (int j = 5; j > 0; j--)
			{
				bit32Swap = (uint32_t*)(byteBase2 - 0x4);
				*bit32Swap = _byteswap_ulong(*bit32Swap);

				bit32Swap = (uint32_t*)(byteBase2);
				*bit32Swap = _byteswap_ulong(*bit32Swap);

				bit32Swap = (uint32_t*)(byteBase + 0x1c + j_offset);
				*bit32Swap = _byteswap_ulong(*bit32Swap);

				byteBase2 += 0xc;
				j_offset += 0xc;
			}

			bit32Swap = (uint32_t*)(byteBase + 0x50);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x54);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x58);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x5c);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x60);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x64);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x68);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x6c);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x70);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x10);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x74);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x78);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			bit32Swap = (uint32_t*)(byteBase + 0x7c);
			*bit32Swap = _byteswap_ulong(*bit32Swap);

			byteBase += 0x84;

			for (int j = 3; j != 0; j--)
			{
				bit32Swap = (uint32_t*)(byteBase - 0x4);
				*bit32Swap = _byteswap_ulong(*bit32Swap);

				byteBase += 0xc;

				bit32Swap = (uint32_t*)(byteBase - 0xc);
				*bit32Swap = _byteswap_ulong(*bit32Swap);

				bit32Swap = (uint32_t*)(byteBase - 0x8);
				*bit32Swap = _byteswap_ulong(*bit32Swap);
			}
		}
	}

	uint16_t category;
	uint16_t categoryIndex;
	uint32_t _1;
	uint32_t flag;
	uint16_t turnBonusCount;
	uint16_t _2;
	uint32_t turnBonus;
	std::array<EncounterBonus, 5> encounterBonuses;
	std::array<uint32_t, 5> waveEncounter;
	std::array<uint32_t, 5> _3;
	uint32_t level;
	uint32_t iconCount;
	std::array<EncounterReward, 3> rewards;
};
IMGUI_REFLECT(ChallengeBattle, category, categoryIndex, flag, turnBonusCount, turnBonus, encounterBonuses, waveEncounter, level, iconCount, rewards);