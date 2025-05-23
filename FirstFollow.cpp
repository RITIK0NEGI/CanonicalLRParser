// FirstFollow.cpp
#include "FirstFollow.h"
#include <iostream>
#include <algorithm>

FirstFollow::FirstFollow(const std::map<std::string, std::vector<std::vector<std::string>>>& prod)
    : productions(prod) {}

void FirstFollow::computeFirst() {
    // Initialize FIRST sets for all non-terminals
    for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator it = productions.begin();
         it != productions.end(); ++it) {
        first[it->first] = std::set<std::string>();
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator prodIt = productions.begin();
             prodIt != productions.end(); ++prodIt) {
            const std::string& nonTerminal = prodIt->first;
            const std::vector<std::vector<std::string>>& rules = prodIt->second;

            for (std::vector<std::vector<std::string>>::const_iterator ruleIt = rules.begin();
                 ruleIt != rules.end(); ++ruleIt) {
                const std::vector<std::string>& rule = *ruleIt;
                std::set<std::string> result = computeFirstOfString(rule);
                size_t before = first[nonTerminal].size();
                first[nonTerminal].insert(result.begin(), result.end());
                if (first[nonTerminal].size() > before) {
                    changed = true;
                }
            }
        }
    }
}

std::set<std::string> FirstFollow::computeFirstOfString(const std::vector<std::string>& str) {
    std::set<std::string> result;
    if (str.empty()) {
        result.insert("ε");
        return result;
    }

    for (size_t i = 0; i < str.size(); ++i) {
        const std::string& symbol = str[i];
        
        if (productions.find(symbol) == productions.end()) {
            result.insert(symbol);
            break;
        }
        
        const std::set<std::string>& firstSet = first[symbol];
        result.insert(firstSet.begin(), firstSet.end());
        
        if (firstSet.find("ε") == firstSet.end()) {
            break;
        }
        
        if (i == str.size() - 1) {
            result.insert("ε");
        } else {
            result.erase("ε");
        }
    }
    return result;
}


void FirstFollow::computeFollow(const std::string& startSymbol) {
    // Initialize FOLLOW sets for all non-terminals
    for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator it = productions.begin();
         it != productions.end(); ++it) {
        follow[it->first] = std::set<std::string>();
    }
    follow[startSymbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator prodIt = productions.begin();
             prodIt != productions.end(); ++prodIt) {
            const std::string& lhs = prodIt->first;
            const std::vector<std::vector<std::string>>& rules = prodIt->second;

            for (std::vector<std::vector<std::string>>::const_iterator ruleIt = rules.begin();
                 ruleIt != rules.end(); ++ruleIt) {
                const std::vector<std::string>& rule = *ruleIt;
                
                for (size_t i = 0; i < rule.size(); ++i) {
                    const std::string& B = rule[i];
                    if (productions.find(B) == productions.end()) continue;

                    size_t before = follow[B].size();
                    
                    if (i + 1 < rule.size()) {
                        std::vector<std::string> beta(rule.begin() + i + 1, rule.end());
                        std::set<std::string> firstBeta = computeFirstOfString(beta);
                        
                        for (std::set<std::string>::const_iterator fIt = firstBeta.begin();
                             fIt != firstBeta.end(); ++fIt) {
                            if (*fIt != "ε") {
                                follow[B].insert(*fIt);
                            }
                        }
                        
                        if (firstBeta.find("ε") != firstBeta.end()) {
                            follow[B].insert(follow[lhs].begin(), follow[lhs].end());
                        }
                    } else {
                        follow[B].insert(follow[lhs].begin(), follow[lhs].end());
                    }
                    
                    if (follow[B].size() > before) {
                        changed = true;
                    }
                }
            }
        }
    }
}

void FirstFollow::displayFirstFollow(std::ostream& os) const {
    std::cout << "\nFIRST sets:\n";
    for (std::map<std::string, std::set<std::string>>::const_iterator it = first.begin();
         it != first.end(); ++it) {
        std::cout << "FIRST(" << it->first << ") = { ";
        for (std::set<std::string>::const_iterator symIt = it->second.begin();
             symIt != it->second.end(); ++symIt) {
            std::cout << *symIt << " ";
        }
        std::cout << "}\n";
    }

    std::cout << "\nFOLLOW sets:\n";
    for (std::map<std::string, std::set<std::string>>::const_iterator it = follow.begin();
         it != follow.end(); ++it) {
        std::cout << "FOLLOW(" << it->first << ") = { ";
        for (std::set<std::string>::const_iterator symIt = it->second.begin();
             symIt != it->second.end(); ++symIt) {
            std::cout << *symIt << " ";
        }
        std::cout << "}\n";
    }
}

std::map<std::string, std::set<std::string>> FirstFollow::getFirst() const {
    return first;
}

std::map<std::string, std::set<std::string>> FirstFollow::getFollow() const {
    return follow;
}
