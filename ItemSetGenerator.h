//ItemSetGenerator.h
#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <tuple>

struct Item {
    std::string lhs;
    std::vector<std::string> rhs;
    size_t dot;
    std::string lookahead;

    bool operator<(const Item& other) const;
    bool operator==(const Item& other) const;
};

class ItemSetGenerator {
public:
    ItemSetGenerator(const std::map<std::string, std::vector<std::vector<std::string>>>& prod,
                     const std::map<std::string, std::set<std::string>>& first);

    void generateItemSets();
    void displayItemSets(std::ostream& os) const;
    const std::vector<std::set<Item>>& getItemSets() const;
    const std::map<std::pair<int, std::string>, int>& getTransitions() const;

private:
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
    std::map<std::string, std::set<std::string>> first;
    std::vector<std::set<Item>> itemSets;
    std::map<std::pair<int, std::string>, int> transitions;

    std::set<Item> closure(const std::set<Item>& items);
    std::set<Item> gotoFunction(const std::set<Item>& items, const std::string& symbol);
};
