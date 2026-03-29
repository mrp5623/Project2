#include <string>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iterator>
#include "Set.hpp"
#include "Fragrance.h"
using json = nlohmann::json;

void set_implementation(json perfume_map) {
    //std::map<std::string, std::set<std::pair<std::string, std::string>>> dataset;
    std::map<std::string, Set> dataset;

    for (auto& [note, perfumes] : perfume_map.items()) {
        for (auto& item : perfumes.items()) {
            dataset[note].insert(new Fragrance{static_cast<std::string>(item.value()[0]), static_cast<std::string>(item.value()[1])});
        }
    }

    std::string input = " ";
    Set results;

    while(true){
        std::cout << "Enter a desired note, or enter Done to continue!" << std::endl;
        std::getline(std::cin, input);
        if (input=="Done"){
            break;
        } 
        else if(dataset.find(input)==dataset.end()){
            std::cout << "Not a note!" << std::endl;
            continue;
        }
        else if (results.isEmpty()) {
            std::cout << "Adding " << input << std::endl;
            results = dataset[input];
        } else {
            std::cout << "Intersecting " << input << std::endl;
            results = results.intersect(dataset[input]);
        }
    }

    std::cout << "Here are the perfumes that match your criteria:" << std::endl;
    results.printResults();

}

void heap_implementation(json perfume_map) {
    //TODO
}   


int main(int argc, char *argv[]) {
    //open file relative from executable
    //std::ifstream file("../../perfume_map.json");
    std::ifstream file("perfume_map.json");

    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    json perfume_map;
    //dump file into json object
    file >> perfume_map;

    set_implementation(perfume_map);
    //heap_implementation(perfume_map);
    
    return 0;
    
}