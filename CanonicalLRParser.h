#ifndef CANONICALLRPARSER_H
#define CANONICALLRPARSER_H

#include "GrammarInput.h"
#include "AugmentedGrammar.h"
#include "FirstFollow.h"
#include "ItemSetGenerator.h"
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <sstream>

class CanonicalLRParser {
private:
    GrammarInput grammarInput;
    AugmentedGrammar* augmentedGrammar;
    FirstFollow* firstFollow;
    ItemSetGenerator* itemSetGenerator;
    std::ostringstream outputStream;  // Add this line

    std::map<int, std::map<std::string, std::string>> actionTable;
    std::map<int, std::map<std::string, int>> gotoTable;
    /*****************************/
    // Simulation state
    struct SimulationState {
        std::stack<int> stateStack;
        std::stack<std::string> symbolStack;
        size_t inputPointer;
        std::string currentAction;
        bool accepted;
        bool error;
    };

    std::vector<SimulationState> simulationStates;
    size_t currentSimulationStep;
    /********************************/

public:
    CanonicalLRParser();
    ~CanonicalLRParser();

    void run();
    std::string getOutput() const;  // Add this method declaration
    void clearOutput();

    void generateParseTable();
    void simulateParser();
    /*****************************/
    void prepareSimulation();  // Initialize simulation states
    bool hasNextStep() const;
    bool hasPreviousStep() const;
    std::string getCurrentStepOutput() const;
    void nextStep();
    void previousStep();
    void resetSimulation();
    /*****************************/
};

#endif // CANONICALLRPARSER_H
