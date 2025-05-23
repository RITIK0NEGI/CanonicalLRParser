// AugmentedGrammar.cpp
#include "AugmentedGrammar.h"

AugmentedGrammar::AugmentedGrammar(std::map<std::string, std::vector<std::vector<std::string>>> prod)
    : productions(prod) {
    if (!productions.empty()) {
        originalStart = "S" ; //productions.begin()->first;
    }
}

void AugmentedGrammar::addAugmentedRule() {
    std::vector<std::string> rhs = { originalStart };
    productions["S'"] = { rhs };
}

std::map<std::string, std::vector<std::vector<std::string>>> AugmentedGrammar::getAugmentedProductions() {
    return productions;
}