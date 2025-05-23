//CanonicalLRParser.cpp
#include "CanonicalLRParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iomanip>

CanonicalLRParser::CanonicalLRParser()
    : augmentedGrammar(nullptr), firstFollow(nullptr), itemSetGenerator(nullptr) {}

CanonicalLRParser::~CanonicalLRParser() {
    delete augmentedGrammar;
    delete firstFollow;
    delete itemSetGenerator;
}

// Add this new method implementation (before run()):
std::string CanonicalLRParser::getOutput() const {
    return outputStream.str();
}

void CanonicalLRParser::clearOutput() {
    std::ostringstream().swap(outputStream);  // Efficiently clears the stream
}


// Modify the run() method to use outputStream instead of cout:
void CanonicalLRParser::run() {
    outputStream.str(""); // Clear the stream

    // Step 1: Read and display grammar
    grammarInput.readGrammar();
    outputStream << "=== Grammar ===\n";
    grammarInput.displayGrammar(outputStream);

    // Step 2: Create augmented grammar
    augmentedGrammar = new AugmentedGrammar(grammarInput.getProductions());
    augmentedGrammar->addAugmentedRule();

    // Step 3: Compute FIRST and FOLLOW sets
    firstFollow = new FirstFollow(augmentedGrammar->getAugmentedProductions());
    firstFollow->computeFirst();
    firstFollow->computeFollow("S'");
    outputStream << "\n=== First and Follow Sets ===\n";
    firstFollow->displayFirstFollow(outputStream);

    // Step 4: Generate item sets
    itemSetGenerator = new ItemSetGenerator(
        augmentedGrammar->getAugmentedProductions(),
        firstFollow->getFirst()
        );
    itemSetGenerator->generateItemSets();
    outputStream << "\n=== Item Sets ===\n";
    itemSetGenerator->displayItemSets(outputStream);

    // Step 5: Generate parse tables
    //generateParseTable();

    // Step 6: Parse input
    //outputStream << "\n=== Parsing ===\n";
    //simulateParser();
}
void CanonicalLRParser::generateParseTable() {
    const auto& itemSets = itemSetGenerator->getItemSets();
    const auto& transitions = itemSetGenerator->getTransitions();
    const auto& productions = augmentedGrammar->getAugmentedProductions();
    const auto& follow = firstFollow->getFollow();

    // Create production index mapping
    std::map<std::pair<std::string, std::vector<std::string>>, int> productionIndex;
    int productionNumber = 0;
    for (const auto& production : productions) {
        const std::string& lhs = production.first;
        for (const auto& rhs : production.second) {
            productionIndex[{lhs, rhs}] = productionNumber++;
        }
    }

    // Build ACTION and GOTO tables
    for (size_t stateId = 0; stateId < itemSets.size(); ++stateId) {
        for (const auto& item : itemSets[stateId]) {
            if (item.dot < item.rhs.size()) {
                // Shift or Goto operation
                const std::string& symbol = item.rhs[item.dot];
                auto transitionIter = transitions.find({static_cast<int>(stateId), symbol});

                if (transitionIter != transitions.end()) {
                    if (productions.find(symbol) != productions.end()) {
                        // Non-terminal - GOTO
                        gotoTable[stateId][symbol] = transitionIter->second;
                    } else {
                        // Terminal - SHIFT
                        actionTable[stateId][symbol] = "s" + std::to_string(transitionIter->second);
                    }
                }
            } else {
                // Reduce or Accept operation
                if (item.lhs == "S'" && item.rhs.size() == 1 && item.rhs[0] == augmentedGrammar->originalStart) {
                    // Accept action
                    actionTable[stateId]["$"] = "acc";
                } else {
                    // Reduce action
                    auto productionKey = std::make_pair(item.lhs, item.rhs);
                    int ruleNumber = productionIndex[productionKey];

                    for (const auto& lookahead : follow.at(item.lhs)) {
                        actionTable[stateId][lookahead] = "r" + std::to_string(ruleNumber);
                    }
                }
            }
        }
    }

    // Display ACTION table
    outputStream << "\nACTION Table:\n";
    for (const auto& stateActions : actionTable) {
        outputStream << "State " << stateActions.first << ": ";
        for (const auto& symbolAction : stateActions.second) {
            outputStream << symbolAction.first << "=" << symbolAction.second << " ";
        }
        outputStream << "\n";
    }

    // Display GOTO table
    outputStream << "\nGOTO Table:\n";
    for (const auto& stateGotos : gotoTable) {
        outputStream << "State " << stateGotos.first << ": ";
        for (const auto& symbolTarget : stateGotos.second) {
            outputStream << symbolTarget.first << "=" << symbolTarget.second << " ";
        }
        outputStream << "\n";
    }
}

/*void CanonicalLRParser::simulateParser() {
    outputStream << "\n=== Parsing ===\n";
    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        std::cout << "Error: Could not open input.txt file\n";
        return;
    }
    std::string input;
    std::getline(inputFile, input);
    inputFile.close();
    outputStream << "Parsing input string: " << input << "\n";


    // Tokenize input
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    // Initialize parsing stack
    std::stack<int> stateStack;
    std::stack<std::string> symbolStack;
    stateStack.push(0); // Initial state

    size_t inputPointer = 0;
    while (true) {
        int currentState = stateStack.top();
        std::string currentSymbol = (inputPointer < tokens.size()) ? tokens[inputPointer] : "$";

        // Check for valid action
        if (actionTable[currentState].count(currentSymbol) == 0) {
            outputStream << "\nError: No action defined for state " << currentState
                         << " and symbol '" << currentSymbol << "'\n";
            return;
        }

        std::string action = actionTable[currentState][currentSymbol];
        outputStream << "State: " << currentState << ", Symbol: " << currentSymbol
                     << ", Action: " << action << "\n";

        if (action == "acc") {
            outputStream << "\nParsing successful - input accepted!\n";
            return;
        }
        else if (action[0] == 's') {
            // Shift action
            int nextState = std::stoi(action.substr(1));
            stateStack.push(nextState);
            symbolStack.push(currentSymbol);
            inputPointer++;
        }
        else if (action[0] == 'r') {
            // Reduce action
            int ruleNumber = std::stoi(action.substr(1));

            // Find the production to reduce by
            bool found = false;
            int currentRule = 0;
            for (const auto& production : augmentedGrammar->getAugmentedProductions()) {
                const std::string& lhs = production.first;
                for (const auto& rhs : production.second) {
                    if (currentRule == ruleNumber) {
                        // Pop symbols from stacks
                        for (size_t i = 0; i < rhs.size(); ++i) {
                            if (!symbolStack.empty()) symbolStack.pop();
                            if (!stateStack.empty()) stateStack.pop();
                        }

                        // Push LHS and new state
                        symbolStack.push(lhs);
                        int newState = stateStack.top();
                        stateStack.push(gotoTable[newState][lhs]);

                        found = true;
                        break;
                    }
                    currentRule++;
                }
                if (found) break;
            }
        }
    }
}*/
/********************************************/
void CanonicalLRParser::prepareSimulation() {
    simulationStates.clear();
    currentSimulationStep = 0;

    // Tokenize input
    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        throw std::runtime_error("Could not open input.txt file");
    }
    std::string input;
    std::getline(inputFile, input);
    inputFile.close();

    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    // Initialize first state
    SimulationState initialState;
    initialState.stateStack.push(0);
    initialState.inputPointer = 0;
    initialState.accepted = false;
    initialState.error = false;
    simulationStates.push_back(initialState);
}

bool CanonicalLRParser::hasNextStep() const {
    if (simulationStates.empty()) return false;
    return currentSimulationStep < simulationStates.size() - 1 ||
           (!simulationStates.back().accepted && !simulationStates.back().error);
}

bool CanonicalLRParser::hasPreviousStep() const {
    return currentSimulationStep > 0;
}
/*
std::string CanonicalLRParser::getCurrentStepOutput() const {
    if (simulationStates.empty() || currentSimulationStep >= simulationStates.size()) {
        return "No simulation data available";
    }

    const SimulationState& state = simulationStates[currentSimulationStep];
    std::ostringstream oss;

    // Display stack contents
    oss << "Stack:\n";
    std::stack<int> stateStackCopy = state.stateStack;
    std::stack<std::string> symbolStackCopy = state.symbolStack;
    std::vector<int> states;
    std::vector<std::string> symbols;

    while (!stateStackCopy.empty()) {
        states.push_back(stateStackCopy.top());
        stateStackCopy.pop();
    }
    while (!symbolStackCopy.empty()) {
        symbols.push_back(symbolStackCopy.top());
        symbolStackCopy.pop();
    }

    // Print states and symbols in correct order
    oss << "States: ";
    for (auto it = states.rbegin(); it != states.rend(); ++it) {
        oss << *it << " ";
    }
    oss << "\nSymbols: ";
    for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) {
        oss << *it << " ";
    }
    oss << "\n\n";

    // Display input pointer
    oss << "Remaining input: ";
    std::ifstream inputFile("input.txt");
    std::string input;
    if (inputFile.is_open()) {
        std::getline(inputFile, input);
        inputFile.close();
    }
    std::istringstream iss(input);
    std::string token;
    size_t pos = 0;
    while (iss >> token) {
        if (pos >= state.inputPointer) {
            oss << token << " ";
        }
        pos++;
    }
    oss << "$\n\n";  // End marker

    // Display action
    if (!state.currentAction.empty()) {
        oss << "Action: " << state.currentAction << "\n";
    }

    if (state.accepted) {
        oss << "\nParsing successful - input accepted!\n";
    } else if (state.error) {
        oss << "\nError: No action defined for current state and symbol\n";
    }

    return oss.str();
}*/

std::string CanonicalLRParser::getCurrentStepOutput() const {
    if (simulationStates.empty() || currentSimulationStep >= simulationStates.size()) {
        return "No simulation data available";
    }

    const SimulationState& state = simulationStates[currentSimulationStep];
    std::ostringstream oss;

    // Display input pointer
    oss << "Remaining input: ";
    std::ifstream inputFile("input.txt");
    std::string input;
    if (inputFile.is_open()) {
        std::getline(inputFile, input);
        inputFile.close();
    }
    std::istringstream iss(input);
    std::string token;
    size_t pos = 0;
    while (iss >> token) {
        if (pos >= state.inputPointer) {
            oss << token << " ";
        }
        pos++;
    }
    oss << "$\n\n";  // End marker

    // Display action
    if (!state.currentAction.empty()) {
        oss << "Action: " << state.currentAction << "\n\n";
    }

    // Display stack in a visual format
    oss << "STACK:\n";
    oss << "┌─────────────┐\n";

    std::stack<int> stateStackCopy = state.stateStack;
    std::stack<std::string> symbolStackCopy = state.symbolStack;
    std::vector<std::pair<int, std::string>> stackContents;

    // Collect stack contents in reverse order (top to bottom)
    while (!stateStackCopy.empty() && !symbolStackCopy.empty()) {
        stackContents.emplace_back(stateStackCopy.top(), symbolStackCopy.top());
        stateStackCopy.pop();
        symbolStackCopy.pop();
    }

    // Print stack from bottom to top
    for (auto it = stackContents.rbegin(); it != stackContents.rend(); ++it) {
        oss << "│ " << std::setw(3) << it->first << " │ " << std::setw(6) << it->second << " │\n";
        oss << "├─────────────┤\n";
    }

    // Print bottom of stack
    oss << "│     $     │\n";
    oss << "└─────────────┘\n\n";

    if (state.accepted) {
        oss << "\nParsing successful - input accepted!\n";
    } else if (state.error) {
        oss << "\nError: No action defined for current state and symbol\n";
    }

    return oss.str();
}

void CanonicalLRParser::nextStep() {
    if (!hasNextStep()) return;

    if (currentSimulationStep == simulationStates.size() - 1) {
        // Need to compute next step
        const SimulationState& currentState = simulationStates.back();
        SimulationState newState = currentState;

        int currentStackState = newState.stateStack.top();
        std::string currentSymbol;

        // Get next input symbol
        std::ifstream inputFile("input.txt");
        std::string input;
        if (inputFile.is_open()) {
            std::getline(inputFile, input);
            inputFile.close();
        }
        std::istringstream iss(input);
        std::string token;
        size_t pos = 0;
        while (iss >> token) {
            if (pos == newState.inputPointer) {
                currentSymbol = token;
                break;
            }
            pos++;
        }
        if (currentSymbol.empty()) {
            currentSymbol = "$";
        }

        // Check for valid action
        if (actionTable[currentStackState].count(currentSymbol) == 0) {
            newState.error = true;
            simulationStates.push_back(newState);
            currentSimulationStep++;
            return;
        }

        std::string action = actionTable[currentStackState][currentSymbol];
        newState.currentAction = action;

        if (action == "acc") {
            newState.accepted = true;
        }
        else if (action[0] == 's') {
            // Shift action
            int nextState = std::stoi(action.substr(1));
            newState.stateStack.push(nextState);
            newState.symbolStack.push(currentSymbol);
            newState.inputPointer++;
        }
        else if (action[0] == 'r') {
            // Reduce action
            int ruleNumber = std::stoi(action.substr(1));

            // Find the production to reduce by
            bool found = false;
            int currentRule = 0;
            for (const auto& production : augmentedGrammar->getAugmentedProductions()) {
                const std::string& lhs = production.first;
                for (const auto& rhs : production.second) {
                    if (currentRule == ruleNumber) {
                        // Pop symbols from stacks
                        for (size_t i = 0; i < rhs.size(); ++i) {
                            if (!newState.symbolStack.empty()) newState.symbolStack.pop();
                            if (!newState.stateStack.empty()) newState.stateStack.pop();
                        }

                        // Push LHS and new state
                        newState.symbolStack.push(lhs);
                        int newStackState = newState.stateStack.top();
                        newState.stateStack.push(gotoTable[newStackState][lhs]);

                        found = true;
                        break;
                    }
                    currentRule++;
                }
                if (found) break;
            }
        }

        simulationStates.push_back(newState);
    }

    currentSimulationStep++;
}

void CanonicalLRParser::previousStep() {
    if (hasPreviousStep()) {
        currentSimulationStep--;
    }
}

void CanonicalLRParser::resetSimulation() {
    currentSimulationStep = 0;
}

void CanonicalLRParser::simulateParser() {
    prepareSimulation();
}
