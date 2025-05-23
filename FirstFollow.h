// FirstFollow.h
#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

class FirstFollow {
public:
    FirstFollow(const std::map<std::string, std::vector<std::vector<std::string>>>& prod);
    void computeFirst();
    void computeFollow(const std::string& startSymbol);
    void displayFirstFollow(std::ostream& os) const;

    std::map<std::string, std::set<std::string>> getFirst() const;
    std::map<std::string, std::set<std::string>> getFollow() const;

private:
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
    std::map<std::string, std::set<std::string>> first;
    std::map<std::string, std::set<std::string>> follow;

    std::set<std::string> computeFirstOfString(const std::vector<std::string>& str);
};
