#include <cpr/cpr.h>
#include <cpr/response.h>
#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <algorithm>

int main(int argc, char *argv[]) {

    cpr::Header headers = {
        {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36"},
        {"Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"},
        {"Accept-Language", "en-US,en;q=0.5"},
        {"Connection", "keep-alive"}
    };
    
    cpr::Response response = cpr::Get(cpr::Url{"https://www.fragrantica.com/notes/"}, headers);

    if(response.status_code !=200) {
        std::cerr << "Failed to fetch the page. Status code: " << response.status_code << std::endl;
        return 1;
    }

    htmlDocPtr doc = htmlReadMemory(response.text.c_str(), static_cast<int>(response.text.length()), nullptr, nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    xmlXPathObjectPtr note_elements = xmlXPathEvalExpression((xmlChar *)"//a[contains(@class, 'rounded-lg')]", context);
    
    std::vector<std::pair<std::string, std::string>> notes;

    for (int i = 0; i < note_elements->nodesetval->nodeNr-1; i++) {
        
        std::string name ="", url ="";
        xmlNodePtr note_node = note_elements->nodesetval->nodeTab[i];
        xmlXPathSetContextNode(note_node, context);

        xmlXPathObjectPtr name_element = xmlXPathEvalExpression((xmlChar *)"./p", context);
        xmlNodePtr name_node = name_element->nodesetval->nodeTab[0];
        name = std::string(reinterpret_cast<char *>(xmlNodeGetContent(name_node)));
        
        name.erase(name.begin(), std::find_if(name.begin(), name.end(), [](unsigned char c){ return !std::isspace(c); })); //trimming via StackOverflow: https://stackoverflow.com/questions/216823/whitespace-trimming-functions-in-c-and-cpp
        name.erase(std::find_if(name.rbegin(), name.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), name.end());

        xmlXPathObjectPtr url_element = xmlXPathEvalExpression((xmlChar *)"./@href", context);
        xmlNodePtr url_node = url_element->nodesetval->nodeTab[0];
        url = std::string(reinterpret_cast<char *>(xmlNodeGetContent(url_node)));

        notes.emplace_back(name, url);
    }

    for (int i =0; i < notes.size(); i++) {
        std::cout << i + 1 << ". " << notes[i].first << std::endl;
    }

    return 0;
    
}