//
// Created by elkhay01 on 4/8/23.
//

#ifndef MEMORYCONTROLLER_ELF_JSONPARSER_H
#define MEMORYCONTROLLER_ELF_JSONPARSER_H

#include <string>
#include <map>

class JsonParser {
public:
    static std::map<std::string, std::string> parse(const std::string& jsonString);};


#endif //MEMORYCONTROLLER_ELF_JSONPARSER_H
