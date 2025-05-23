// AugmentedGrammar.h
#pragma once
#include <map>
#include <vector>
#include <string>

class AugmentedGrammar {
public:
    AugmentedGrammar(std::map<std::string, std::vector<std::vector<std::string>>> prod);
    void addAugmentedRule();
    std::map<std::string, std::vector<std::vector<std::string>>> getAugmentedProductions();
    std::string originalStart;
private:
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
};
