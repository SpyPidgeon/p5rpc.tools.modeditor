#pragma once
#include "panels.h"
#include "encounterstructs.h"
#include "exportable.h"

enum class EncounterTab
{
	ENCOUNTER,
	PARTY,
	CHALLENGE
};

class EncounterPanel : Panel, Exportable
{
public:
	EncounterPanel() { this->label = "Encounter Table"; sortingPriority = 10; }
	static EncounterPanel* GetInstance() { static EncounterPanel* instance = new EncounterPanel(); return instance; }

	void RenderLogic() override;
	void InspectorLogic() override;
	void ScanValues() override;
	void ApplyChanges() override;
	void Refresh() override;

	void ExportFile(const std::string& directory) override;

	EncounterTab tab = EncounterTab::ENCOUNTER;

	std::array<EnemyEncounter, EnemyEncounter::ENCOUNTER_COUNT> enemyEncounters;
	std::array<EnemyEncounter, EnemyEncounter::ENCOUNTER_COUNT>* enemyEncountersPTR;

	std::array<ForcedPartyData, EnemyEncounter::ENCOUNTER_COUNT> forcedPartyData;
	std::array<ForcedPartyData, EnemyEncounter::ENCOUNTER_COUNT>* forcedPartyDataPTR;

	std::array<ChallengeBattle, ChallengeBattle::CHALLENGE_COUNT> challenges;
	std::array<ChallengeBattle, ChallengeBattle::CHALLENGE_COUNT>* challengesPTR;

	std::array<std::string, 783> enemyNames;
};
static EncounterPanel* encounterPanel = EncounterPanel::GetInstance();