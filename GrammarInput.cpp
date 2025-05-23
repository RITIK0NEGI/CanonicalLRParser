// GrammarInput.cpp
#include "GrammarInput.h"
#include <iostream>
#include <sstream>
#include <fstream>

void GrammarInput::readGrammar() {
    std::string filename = "grammar.txt";  // Default filename or get from config
    std::ifstream infile(filename);
    
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return;
    }

    std::cout << "Reading grammar from file: " << filename << "\n";
    std::string line;
    
    while (std::getline(infile, line)) {
        // Skip empty lines and comments (lines starting with #)
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string lhs, arrow;
        
        // Read LHS and arrow
        if (!(iss >> lhs >> arrow)) {
            std::cerr << "Invalid format in line: " << line << "\n";
            continue;
        }

        if (arrow != "->") {
            std::cerr << "Invalid production arrow in line: " << line << "\n";
            continue;
        }

        // Get the rest of the line after ->
        std::string productionPart;
        std::getline(iss, productionPart);
        
        // Trim leading whitespace
        productionPart.erase(0, productionPart.find_first_not_of(" \t"));
        
        // Split productions by | and process each
        std::istringstream prodIss(productionPart);
        std::string production;
        
        while (std::getline(prodIss, production, '|')) {
            // Trim whitespace from each production
            production.erase(0, production.find_first_not_of(" \t"));
            production.erase(production.find_last_not_of(" \t") + 1);

            if (production.empty()) {
                continue;
            }

            std::istringstream rhsStream(production);
            std::string symbol;
            std::vector<std::string> symbols;
            
            while (rhsStream >> symbol) {
                symbols.push_back(symbol);
            }
            
            if (!symbols.empty()) {
                productions[lhs].push_back(symbols);
            }
        }
    }
    
    infile.close();
}

void GrammarInput::displayGrammar(std::ostream& os) const {
    os << "\nGrammar:\n";
    for (const auto& entry : productions) {
        os << entry.first << " -> ";
        for (size_t i = 0; i < entry.second.size(); ++i) {
            for (const std::string& sym : entry.second[i]) {
                os << sym << " ";
            }
            if (i != entry.second.size() - 1) os << "| ";
        }
        os << "\n";
    }
}

std::map<std::string, std::vector<std::vector<std::string>>> GrammarInput::getProductions() const {
    return productions;
}
