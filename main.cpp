#include <string>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iterator>
using json = nlohmann::json;


int main(int argc, char *argv[]) {
    //open file relative from executable
    std::ifstream file("../../perfume_map.json");

    if (!file.is_open()) {
        std::cerr << "Fail";
        return 1;
    }

    json perfume_map;
    //dump file into json object
    file >> perfume_map;

    std::map<std::string, std::set<std::pair<std::string, std::string>>> dataset;

    for (auto& [note, perfumes] : perfume_map.items()) {
        for (auto& item : perfumes.items()) {
            dataset[note].insert({static_cast<std::string>(item.value()[0]), static_cast<std::string>(item.value()[1])});
        }
    }

    std::string input = " ";
    std::set<std::pair<std::string, std::string>> results;

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
        else if (results.empty()) {
            std::cout << "Adding " << input << std::endl;
            results = dataset[input];
        } else {
            std::cout << "Intersecting " << input << std::endl;
            std::set<std::pair<std::string, std::string>> new_results = {};
            std::set_intersection(results.begin(), results.end(), dataset[input].begin(), dataset[input].end(), std::inserter(new_results, new_results.begin()));
            results = new_results;
        }
    }

    std::cout << "Here are the perfumes that match your criteria:" << std::endl;
    int count = 1;
    for (auto& [name, url] : results) {
        std::cout << count << ". " << name << ": " << url << std::endl;
        count++;
    }
    
    return 0;
    
}