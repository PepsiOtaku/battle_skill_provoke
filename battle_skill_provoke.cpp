// Provoke Skill
// by PepsiOtaku
// Version 3.1

#include <DynRPG/DynRPG.h>
#define NOT_MAIN_MODULE
#include <vector>

std::map<std::string, std::string> configuration;

int g_targetMonster[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int targetDigit[2];
int varDigitValue;
int partyMember;
bool forceProvoke = false;
int forceProvokeId[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // monster's ID

int confConditionId;
std::string confSkillSet;

std::vector<int> provokeVect; // stores the skill IDs that Provoke will use


bool onStartup (char *pluginName) {
    // Get configuration settings for key variables
    configuration = RPG::loadConfiguration(pluginName);
    // Provoke Condition ID
    confConditionId = atoi(configuration["ConditionId"].c_str()); // Convert String to Int
    // Provoke Skill ID
    confSkillSet = configuration["SkillId"]; // Convert String to Int
    // Parses the skill ID's for Provoke
    std::string delimiter = ",";
    size_t pos = 0;
    int token;
    while ((pos = confSkillSet.find(delimiter)) != std::string::npos) {
        token = atoi(confSkillSet.substr(0, pos).c_str());
        provokeVect.push_back(token);
        confSkillSet.erase(0, pos + delimiter.length());
    }
    token = atoi(confSkillSet.c_str());
    provokeVect.push_back(token);

    return true;
}

bool __cdecl onDoBattlerAction (RPG::Battler* battler, bool firstTry){
    if (battler->isMonster() == false && firstTry == true) { // if battler is a Hero
        for (unsigned int j=0; j<provokeVect.size(); j++)
        {
            if (battler->action->skillId == provokeVect[j]) { // if battler is using Provoke skill
                // to save variable space, the following combines the attacker's hero id
                // with the monster's id and stores it in the appropriate variable
                targetDigit[0] = ((battler->id) * 10);
                targetDigit[1] = (battler->action->targetId+1);
                varDigitValue = (targetDigit[0] + targetDigit[1]);
                g_targetMonster[battler->action->targetId+1] = varDigitValue; // Save the hero's ID for use later
            }
        }
    }
    // if monster has Provoke condition for more than one turn
    if (battler->isMonster() == true && (battler->conditions[confConditionId] != 0 || forceProvokeId[battler->id] != 0)) {
        varDigitValue = g_targetMonster[battler->id];
        targetDigit[0] = ((varDigitValue - battler->id)/10);
        // the following loop extracts the party member's id, so that they can be
        // targeted by the monster
        int i;
        for (i=0; i<4; i++){
            if (RPG::Actor::partyMember(i)->id == targetDigit[0]){
                partyMember = i;
                break;
            }
        }
        if (forceProvokeId[battler->id] != 0) forceProvokeId[battler->id] = 0;
        battler->action->targetId = partyMember;
    }
    return true;
}

bool onComment(const char* text, const RPG::ParsedCommentData* parsedData, RPG::EventScriptLine* nextScriptLine,
		       RPG::EventScriptData* scriptData, int eventId, int pageId, int lineId, int* nextLineId)
{
    std::string cmd = parsedData->command;
    // Forces a monster to attack the hero during the turn this is called
    if(!cmd.compare("provoke_monster")) {
        int m = parsedData->parameters[0].number; // monster ID
        int h = RPG::Actor::partyMember(parsedData->parameters[1].number-1)->id; // hero's database ID
        targetDigit[0] = (h*10);
        targetDigit[1] = (m);
        varDigitValue = (targetDigit[0] + targetDigit[1]);
        g_targetMonster[m] = varDigitValue; // Save the hero's ID for use later
        //forceProvoke = true;
        forceProvokeId[m-1] = h;
    }
    return true;
}
