//
// Created by nick spallone on 3/27/26.
//

#ifndef PERFUMESCRAPER_FRAGRANCE_H
#define PERFUMESCRAPER_FRAGRANCE_H

#include <string>

class Fragrance {
public:
    Fragrance(std::string fragName, std::string fragUrl) : name(fragName), url(fragUrl) {}

    std::string name;
    std::string url;
};

#endif //PERFUMESCRAPER_FRAGRANCE_H