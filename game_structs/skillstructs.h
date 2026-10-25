#pragma once
#include "guistructs.h"

//-------------------------------------------
// ActiveSkill
//-------------------------------------------
enum class AreaTypes : uint8_t
{
	Unusable,
	UsableInField,
	UsableInBattle,
	UsableInBoth
};

enum class Conditions : uint8_t
{
	None,
	UnknownShowtimeRelated = 1,
	StopContinuousUsageForExplosion = 2,
	UnknownForMaruki = 4,
	UnknownForSegamiRice = 8,
	UsableOnlyForAmbush = 16
};

enum class CasterEffect2 : uint8_t
{
	NoCasterEffect,
	KOSelf,
	MeleeContact,
	SummonAlly_CRASH_WARNING = 4,
	SupportRelated = 8,
	KillSelfAfterCast = 9,
	UsePercentageSP = 16,
	RecarmdraRelated = 24,
	SaeRouletteSkill = 32,
	TrafuriRelated = 40,
	FollowUpRelated = 66,
	RouletteRelated1 = 128,
	UnknownForMaruki = 130,
	RouletteRelated2 = 136,
	UnknownCasterEffect_CRASH_WARNING = 160
};

enum class DamageHealType : uint8_t
{
	NoDamageOrHealing,
	DealDamage,
	RestoreHealth,
	SetHP,
	DealExactDamage,
	RestoreExactHealth,
	UnkDamage6,
	UnkHealing7,
	DamagePercentCurrentHP,
	RestorePercentCurrentHP,
	DamagePercentMaxHP,
	RestorePercentMaxHP,
	DrainDamage,
	DrainExactDamage,
	UnkDamage14,
	RestoreHealthWhiffChance,
	DamageBasedOnHealthyParty
};

enum class SPEffect : uint8_t
{
	NoEffect,
	DepleteFixedSP = 4,
	RestoreSP = 5,
	DepleteSP = 8,
	UnkRestoreSP = 9,
	DepletePercentMaxSP = 10,
	RestorePercentMaxSP = 11,
	DrainSPBasedOnMagic = 12,
	DrainFixedSP = 13
};

enum class CasterEffect1 : uint8_t
{
	NoCasterEffect,
	SpecialMoveRelated,
	ModifiedByEquippedGun,
	ReduceOwnHPTo1 = 4,
	StealInfoRelated = 32
};

enum class DamageStatType : uint8_t
{
	NoStat,
	Strength,
	Magic
};

enum class CostType : uint8_t
{
	NoCost,
	HP,
	SP
};

enum class SkillType : uint8_t
{
	MagicSkill,
	PhysicalSkill
};

enum class TargetType : uint8_t
{
	SingleTarget,
	MultiTarget,
	MultiTargetExceptSelf,
	PlumeOfDuskRelated
};

enum class TargetFlags : int8_t
{
	Allies = 1 << 0,
	Enemies = 1 << 1
};

enum class CommonAilments : int8_t
{
	Burn = 1 << 0,
	Freeze = 1 << 1,
	Shock = 1 << 2,
	Dizzy = 1 << 3,
	Confuse = 1 << 4,
	Fear = 1 << 5,
	Forget = 1 << 6,
	Hunger = 1 << 7
};

enum class CommonAilments2 : int8_t
{
	Sleep = 1 << 0,
	Rage = 1 << 1,
	Despair = 1 << 2,
	Brainwash = 1 << 3,
	Desperation = 1 << 4,
	Berserk = 1 << 5,
	Panic = 1 << 6,
	Lust = 1 << 7
};

enum class SpecialAilments : int8_t
{
	Wrath = 1 << 0,
	Jealousy = 1 << 1,
	WeakToAll = 1 << 2,
	Unconscious = 1 << 3,
	KnockDown = 1 << 4,
	Gluttony = 1 << 5,
	Mouse = 1 << 6,
	Counter = 1 << 7
};

enum class BuffsAndDebuffs : int8_t
{
	AtkUp = 1 << 0,
	AtkDown = 1 << 1,
	EvadeUp = 1 << 2,
	EvadeDown = 1 << 3,
	DefUp = 1 << 4,
	DefDown = 1 << 5,
	AccuracyUp = 1 << 6,
	AccuracyDown = 1 << 7
};

enum class CommonBuffs : int8_t
{
	Charge = 1 << 0,
	Concentrate = 1 << 1,
	RemoveBuffs = 1 << 2,
	RemoveDebuffs = 1 << 3,
	CrtiUp = 1 << 4,
	CritWayUp = 1 << 5,
	PhysShield = 1 << 6,
	MagShield = 1 << 7
};

enum class BreakSkills : int8_t
{
	Fire = 1 << 0,
	Ice = 1 << 1,
	Electric = 1 << 2,
	Wind = 1 << 3,
	Nuke = 1 << 4,
	Psy = 1 << 5,
	AilmentSusception = 1 << 6,
	BreakPhysShield = 1 << 7
};

enum class Shield : int8_t
{
	BreakMagShield = 1 << 0,
	InstakillShield = 1 << 1,
	Fire = 1 << 2,
	Ice = 1 << 3,
	Electric = 1 << 4,
	Wind = 1 << 5,
	Nuke = 1 << 6,
	Psy = 1 << 7
};

enum class OtherBuffs : int8_t
{
	None,
	UniversalShieldDrain = 2
};

enum class ExtraEffects : uint8_t
{
	NoEffect,
	OnlyHitFear,
	Escape,
	BonusDamageOnDown,
	ThrowMoney = 6,
	MaleAlliesOnly,
	FemaleAlliesOnly,
	KillIfDizzy_UNTESTED,
	DizzyDownOneMore_UNTESTED,
	RandomResistChange_UNTESTED,
	FoesWeakToBlessOrCurseTo1HP_UNTESTED,
	AlwaysKill,
	KillFoesWithAilments,
	BigBangBurgerVisual_UNTESTED,
	PowerUpIfSurrounded,
	BatonPassDamageUp,
	IgnoreResist,
	TargetedChanceUp,
	TargetedChanceWayUp,
	IfHungerHealandCure,
	CasterAttackWayDown,
	TargetedChanceDown
};

enum class TargetRestrictions : uint8_t
{
	NoRestrictions,
	RandomTwo,
	DreadfulScreamRelated,
	WingBlastRelated = 4,
	SelfOnly = 16,
	SelfNotAllowed = 32,
	UnconsciousOnly = 64
};

enum class EffectType : uint8_t
{
	None,
	SingleOrMultiple,
	Cure,
	Multiple
};

#pragma pack(push,1)
struct ActiveSkill
{
	CasterEffect2 casterEffect2;
	CasterEffect1 casterEffect1;
	Conditions conditionUsage;
	uint8_t unkr0;
	AreaTypes areaType;
	uint8_t unkr1;
	DamageStatType damageStatType;
	CostType costType;
	uint16_t cost;
	SkillType skillType;
	uint8_t multiplyCost;
	TargetType targetType;
	TargetFlags validTargetFlags;
	TargetRestrictions targetRestrictions;
	uint8_t unk0;
	uint8_t unk1;
	uint8_t unk2;
	uint8_t unk3;
	uint8_t unk4;
	uint8_t accuracy;
	uint8_t minHits;
	uint8_t maxHits;
	DamageHealType damageOrHeal;
	uint16_t damageValue;
	SPEffect spEffect;
	uint8_t unk5;
	uint16_t spRestoreValue;
	EffectType effectType;
	uint8_t effectChance;
	CommonAilments commonAilments1;
	CommonAilments2 commonAilments2;
	SpecialAilments specialAilments;
	uint8_t unk6;
	BuffsAndDebuffs buffsAndDebuffs;
	CommonBuffs commonBuffs;
	BreakSkills breakSkills;
	Shield shields;
	OtherBuffs otherBuffs;
	uint16_t reserve;
	uint8_t unkr2;
	ExtraEffects extraEffects;
	uint8_t critChance;
	bool forItem;
	uint8_t unk7;
};
#pragma pack(pop)

static_assert(sizeof(ActiveSkill) == 48, "ActiveSkill size must be 48 bytes.");

IMGUI_REFLECT
(ActiveSkill, casterEffect2, casterEffect1, conditionUsage, unkr0, areaType, unkr1, damageStatType, costType, cost, skillType, multiplyCost, targetType,
	validTargetFlags, targetRestrictions, unk0, unk1, unk2, unk3, unk4, accuracy, minHits, maxHits, damageOrHeal, damageValue, spEffect, unk5, spRestoreValue,
	effectType, effectChance, unk6,commonAilments1, specialAilments, commonAilments2, buffsAndDebuffs, commonBuffs, breakSkills, shields, otherBuffs,
	reserve, unkr2, extraEffects, critChance, forItem, unk7);

//-------------------------------------------
// Skill Elements
//-------------------------------------------

enum class Elements : uint8_t
{
	Physical,
	Gun,
	Fire,
	Ice,
	Electric,
	Wind,
	Psy,
	Nuke,
	Bless,
	Curse,
	Almighty,
	Dizzy,
	Confuse,
	Fear,
	Forget,
	Hunger,
	Sleep,
	Rage,
	Despair,
	Brainwash,
	Healing,
	Support,
	EnemySupport_UNKNOWN,
	Passive = 255
};

enum class ElementSkillType : uint8_t
{
	Active,
	Passive,
	NULLSkill = 255
};

struct SkillElement
{
	Elements skillElement;
	ElementSkillType skillType;
	uint8_t inheritable;
	uint8_t unk0;
	uint8_t unusedBitfield;
	std::array<BYTE, 3> unk;
};

IMGUI_REFLECT(SkillElement, skillElement, skillType, inheritable, unk0, unusedBitfield,unk);


//-------------------------------------------
// Technicals
//-------------------------------------------

enum class ApplicableAilments : int
{
	Burn = 1 << 0,
	Freeze = 1 << 1,
	Shock = 1 << 2,
	Dizzy = 1 << 3,
	Confused = 1 << 4,
	Fear = 1 << 5,
	Forget = 1 << 6,
	Hunger = 1 << 7,
	Sleep = 1 << 8,
	Rage = 1 << 9,
	Despair = 1 << 10,
	Brainwash = 1 << 11
};

template<>
struct magic_enum::customize::enum_range<ApplicableAilments>
{
	//static constexpr uint32_t min = 0;
	//static constexpr uint32_t max = 2048;
	static constexpr bool is_flags = true;
};

enum class TechnicalAffinity : int32_t
{
	Blank = -1,
	Physical,
	Gun,
	Fire,
	Ice,
	Electric,
	Wind,
	Psy,
	Nuke,
	Bless,
	Curse,
	Almighty
};

struct Technical
{
	ApplicableAilments applicableAilments;
	bool allAffinitiesTechnical;
	std::array<TechnicalAffinity, 5> affinities;
	float damageMultiplier;
	uint32_t unk;
	int requireKnowingHeart;
};
IMGUI_REFLECT(Technical, applicableAilments, allAffinitiesTechnical, affinities, damageMultiplier, unk);

//-------------------------------------------
// Traits
//-------------------------------------------

enum class TraitFlags : int
{
	Unique = 1 << 0,
	Treasure = 1 << 1,
	UseSubTrait = 1 << 2
};

template<>
struct magic_enum::customize::enum_range<TraitFlags>
{
	static constexpr bool is_flags = true;
};

enum class TraitElementFlags : int
{
	Unknown = 1 << 0,
	Physical = 1 << 1,
	Gun = 1 << 2,
	Fire = 1 << 3,
	Ice = 1 << 4,
	Elec = 1 << 5,
	Wind = 1 << 6,
	Psy = 1 << 7,
	Nuke = 1 << 8,
	Bless = 1 << 9,
	Curse = 1 << 10,
	Almighty = 1 << 11,
	Dizzy = 1 << 12,
	Confuse = 1 << 13,
	Fear = 1 << 14,
	Forget = 1 << 15,
	Hunger = 1 << 16,
	Sleep = 1 << 17,
	Rage = 1 << 18,
	Despair = 1 << 19,
	Brainwash = 1 << 20,
	Healing = 1 << 21,
	Support = 1 << 22
};

template<>
struct magic_enum::customize::enum_range<TraitElementFlags>
{
	static constexpr bool is_flags = true;
};

struct Trait
{
	short effect;
	short field2;
	int effectRate;
	TraitElementFlags elementFlags;
	float effectMagnitude;
	std::array<int, 10> subTraits;
	TraitFlags flags;
};
IMGUI_REFLECT(Trait, effect, field2, effectRate, effectMagnitude, subTraits, flags, elementFlags);