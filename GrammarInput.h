// GrammarInput.h
#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>

class GrammarInput {
public:
    void readGrammar();
    void displayGrammar(std::ostream& os) const;
    std::map<std::string, std::vector<std::vector<std::string>>> getProductions() const;

private:
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
};
