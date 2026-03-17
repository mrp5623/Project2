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
        //Mimics chrome browser
        {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36"},
        //Specifies HTML as response format
        {"Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"},
        //Specifies English
        {"Accept-Language", "en-US,en;q=0.5"},
        //If multiple requests needed
        {"Connection", "keep-alive"}
    };
    
    //Access site
    cpr::Response response = cpr::Get(cpr::Url{"https://www.fragrantica.com/notes/"}, headers);

    //200 = pass, 403 = blocked, 404 = not found, 429 = rate limited (wait like an hour)
    if(response.status_code !=200) {
        std::cerr << "Failed to fetch the page. Status code: " << response.status_code << std::endl;
        return 1;
    }

    //Get html and make parsable
    htmlDocPtr doc = htmlReadMemory(response.text.c_str(), static_cast<int>(response.text.length()), nullptr, nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
    xmlXPathContextPtr context = xmlXPathNewContext(doc);

    //Store all node elements by unique text within their class attribute
    //TODO: confirm that this is unqiue enough to return what we want 
    xmlXPathObjectPtr note_elements = xmlXPathEvalExpression((xmlChar *)"//a[contains(@class, 'rounded-lg')]", context);
    
    //Vector of name, url pairs for each note
    std::vector<std::pair<std::string, std::string>> notes;

    //Iterate through each node and extract the name and url (minus last one which is empty for some reason)
    for (int i = 0; i < note_elements->nodesetval->nodeNr-1; i++) {
        
        std::string name ="", url ="";

        //Get current node and set it as "parent" for relative xpath queries
        xmlNodePtr note_node = note_elements->nodesetval->nodeTab[i];
        xmlXPathSetContextNode(note_node, context);

        //Get name
        xmlXPathObjectPtr name_element = xmlXPathEvalExpression((xmlChar *)"./p", context);
        xmlNodePtr name_node = name_element->nodesetval->nodeTab[0];
        name = std::string(reinterpret_cast<char *>(xmlNodeGetContent(name_node)));

        //Trim whitespace
        name.erase(name.begin(), std::find_if(name.begin(), name.end(), [](unsigned char c){ return !std::isspace(c); })); //via StackOverflow: https://stackoverflow.com/questions/216823/whitespace-trimming-functions-in-c-and-cpp
        name.erase(std::find_if(name.rbegin(), name.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), name.end());

        //Get url
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