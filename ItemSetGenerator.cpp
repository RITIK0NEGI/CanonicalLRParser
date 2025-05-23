//ItemSetGenerator.cpp
#include "ItemSetGenerator.h"
#include <iostream>
#include <algorithm>

bool Item::operator<(const Item& other) const {
    return std::tie(lhs, rhs, dot, lookahead) < std::tie(other.lhs, other.rhs, other.dot, other.lookahead);
}

bool Item::operator==(const Item& other) const {
    return lhs == other.lhs && rhs == other.rhs && dot == other.dot && lookahead == other.lookahead;
}

ItemSetGenerator::ItemSetGenerator(
    const std::map<std::string, std::vector<std::vector<std::string>>>& prod,
    const std::map<std::string, std::set<std::string>>& firstMap
) : productions(prod), first(firstMap) {}

void ItemSetGenerator::generateItemSets() {
    // Initialize with augmented start symbol
    Item startItem = {"S'", productions.at("S'")[0], 0, "$"};
    std::set<Item> startSet = closure({startItem});
    itemSets.push_back(startSet);

    // Collect all grammar symbols
    std::set<std::string> grammarSymbols;
    for (const auto& production : productions) {
        grammarSymbols.insert(production.first); // Non-terminals
        for (const auto& rule : production.second) {
            for (const auto& symbol : rule) {
                grammarSymbols.insert(symbol); // Terminals
            }
        }
    }

    // Generate all item sets
    bool changed = true;
    while (changed) {
        changed = false;
        std::vector<std::set<Item>> currentItemSets = itemSets;
        
        for (size_t stateId = 0; stateId < itemSets.size(); ++stateId) {
            for (const auto& symbol : grammarSymbols) {
                std::set<Item> newState = gotoFunction(itemSets[stateId], symbol);
                
                if (!newState.empty()) {
                    // Check if this state already exists
                    auto existingState = std::find(
                        currentItemSets.begin(),
                        currentItemSets.end(),
                        newState
                    );
                    
                    int targetStateId;
                    if (existingState == currentItemSets.end()) {
                        currentItemSets.push_back(newState);
                        targetStateId = currentItemSets.size() - 1;
                        changed = true;
                    } else {
                        targetStateId = std::distance(currentItemSets.begin(), existingState);
                    }
                    
                    // Record the transition
                    transitions[{static_cast<int>(stateId), symbol}] = targetStateId;
                }
            }
        }
        
        itemSets = currentItemSets;
    }
}

const std::vector<std::set<Item>>& ItemSetGenerator::getItemSets() const {
    return itemSets;
}

const std::map<std::pair<int, std::string>, int>& ItemSetGenerator::getTransitions() const {
    return transitions;
}

std::set<Item> ItemSetGenerator::closure(const std::set<Item>& items) {
    std::set<Item> closureSet = items;
    bool changed = true;

    while (changed) {
        changed = false;
        std::set<Item> newItems;
        
        for (const auto& item : closureSet) {
            if (item.dot < item.rhs.size()) {
                const std::string& nextSymbol = item.rhs[item.dot];
                
                if (productions.find(nextSymbol) != productions.end()) {
                    // Get remaining symbols after the dot
                    std::vector<std::string> remainingSymbols(
                        item.rhs.begin() + item.dot + 1,
                        item.rhs.end()
                    );
                    remainingSymbols.push_back(item.lookahead);

                    // Compute FIRST set of remaining symbols
                    std::set<std::string> firstRemaining;
                    for (const auto& sym : remainingSymbols) {
                        if (productions.find(sym) == productions.end()) {
                            // Terminal symbol
                            firstRemaining.insert(sym);
                            break;
                        } else {
                            // Non-terminal symbol
                            firstRemaining.insert(
                                first.at(sym).begin(),
                                first.at(sym).end()
                            );
                            if (first.at(sym).find("ε") == first.at(sym).end()) {
                                break;
                            }
                        }
                    }

                    // Add new items for each production of the next symbol
                    for (const auto& production : productions.at(nextSymbol)) {
                        for (const auto& lookahead : firstRemaining) {
                            Item newItem = {
                                nextSymbol,
                                production,
                                0,  // Dot at beginning
                                lookahead
                            };
                            
                            if (closureSet.find(newItem) == closureSet.end()) {
                                newItems.insert(newItem);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
        
        closureSet.insert(newItems.begin(), newItems.end());
    }
    
    return closureSet;
}
std::set<Item> ItemSetGenerator::gotoFunction(const std::set<Item>& items, const std::string& symbol) {
    std::set<Item> movedItems;
    
    for (const auto& item : items) {
        if (item.dot < item.rhs.size() && item.rhs[item.dot] == symbol) {
            Item movedItem = item;
            movedItem.dot++;
            movedItems.insert(movedItem);
        }
    }
    
    return closure(movedItems);
}

void ItemSetGenerator::displayItemSets(std::ostream& os) const {
    for (size_t stateId = 0; stateId < itemSets.size(); ++stateId) {
        std::cout << "I" << stateId << ":\n";
        for (const auto& item : itemSets[stateId]) {
            std::cout << item.lhs << " -> ";
            
            // Print production with dot
            for (size_t pos = 0; pos < item.rhs.size(); ++pos) {
                if (pos == item.dot) std::cout << ". ";
                std::cout << item.rhs[pos] << " ";
            }
            
            // Handle dot at end case
            if (item.dot == item.rhs.size()) std::cout << ". ";
            
            std::cout << ", " << item.lookahead << "\n";
        }
        std::cout << "\n";
    }
}
