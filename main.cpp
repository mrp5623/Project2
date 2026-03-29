#include <string>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iterator>
#include "Fragrance.h"
#include "FragranceHeap.h"
using json = nlohmann::json;

void set_implementation(json perfume_map) {
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

}

void heap_implementation(json perfume_map) {
    // storage for all of our fragrances so that we can add pointers to them in our sets safely for memory purposes
    std::vector<Fragrance> allFrags;

    // map that stores note keys and pointers to all fragrances containing those notes
    std::unordered_map<std::string, std::unordered_set<Fragrance*>> noteToFrags;

    for (auto& [note, perfumes] : perfume_map.items()) {
        for (auto& item : perfumes.items()) {
            allFrags.push_back(Fragrance(static_cast<std::string>(item.value()[0]), static_cast<std::string>(item.value()[1])));
            noteToFrags[note].insert(&allFrags.back());
        }
    }

    std::string input = " ";

    std::unordered_set<std::string> currNotes;

    while (true) {
        if (!currNotes.empty()) {
            std::cout << "Selected notes: ";
            bool first = true;
            for (auto n : currNotes) {
                if (!first) {
                    std::cout << ", ";
                }
                std::cout << n;
                first = false;
            }
            std::cout << std::endl;
        }
        std::cout << "Choose between the following options:" << std::endl;
        std::cout << "1. Add a note\n2. Remove a note\n3. Continue to results" << std::endl;
        std::getline(std::cin, input);

        if (input == "1") {
            std::cout << "Enter note to add: ";
            std::getline(std::cin, input);

            if (noteToFrags.find(input) == noteToFrags.end()) {
                std::cout << "Not a valid note, try again\n" << std::endl;
            } else {
                currNotes.insert(input);
                std::cout << std::endl;
            }
        }
        else if (input == "2") {
            std::cout << "Enter note to remove: ";
            std::getline(std::cin, input);

            if (currNotes.find(input) == currNotes.end()) {
                std::cout << "You have not selected that note yet" << std::endl;
            }
            else {
                currNotes.erase(input);
                std::cout << std::endl;
            }
        }
        else if (input == "3") {
            break;
        }
        else {
            std::cout << "Error, try again" << std::endl;
        }
    }

    // frequency map to rank fragrance appearance based on selected notes
    std::unordered_map<Fragrance*, int> freq;
    // loop through selected notes, add fragrance and increment its count in freq map to mark it as having a match
    for (auto n : currNotes) {
        for (auto* f : noteToFrags[n]) {
            freq[f]++;
        }
    }

    // min heap of size 5 to give 5 best recommendations
    FragranceHeap heap;
    int k = 5;
    for (auto& [frag, score] : freq) {
        if (heap.size() < k) {
            heap.push(score, frag);
        }
        else if (score > heap.top().score) {
            heap.pop();
            heap.push(score, frag);
        }
    }

    // pop from heap and push to res vector for results
    std::vector<FragranceHeap::FragNode> res;
    while (!heap.empty()) {
        res.push_back(heap.top());
        heap.pop();
    }

    if (res.empty()) {
        std::cout << "We" << std::endl;
    }

    // reversed because min heap displays recommendations from lowest to highest score after popping into res
    std::reverse(res.begin(), res.end());

    std::cout << "Here are the perfumes that match your criteria:" << std::endl;
    int count = 1;
    for (auto& n : res) {
        std::cout << count << ". " << n.frag->name << ": " << n.frag->url << std::endl;
        count++;
    }
}


int main(int argc, char *argv[]) {
    //open file relative from executable
    // std::ifstream file("../../perfume_map.json");

    /* file wouldn't open for me with method above on clion so i just went into clion and did
     * run -> edit configurations -> and set the working directory to my project folder which fixed it
     * but idk if you are all on clion so you could comment that out as well
     */
    std::ifstream file("perfume_map.json");

    if (!file.is_open()) {
        std::cerr << "Fail";
        return 1;
    }

    json perfume_map;
    //dump file into json object
    file >> perfume_map;

    set_implementation(perfume_map);
    heap_implementation(perfume_map);

    return 0;

}