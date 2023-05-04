//
// Created by elkhay01 on 4/8/23.
//

#include "cpp/uart_frame_protocol/JsonParser.h"
#include <iostream>
#include <string>
#include <sstream>
#include<vector>
#include <algorithm>
#include <iterator>

std::map<std::string, std::string>  JsonParser::parse(const std::string& jsonString) {
    std::map<std::string, std::string> keyValues;

    std::string key;
    std::string value;
    bool inString = false;
    bool inKey = true;
    bool inStringValue = false;

    for (size_t i = 0; i < jsonString.size(); i++) {
        char c = jsonString[i];
        //std::cout << "-----C----" << c << std::endl;
        if (c != '"' && c!=',' && c != '}' && c != ' ' && !inKey && !inStringValue) {
            value += c;
            //std::cout << "-> VALUE int " << " <=> " << value << std::endl;
        }
        else if (c == '"' && jsonString[i-1] != '\\') {
            //std::cout << "> c == \"  " << " <=> " << c << std::endl;
            inString = !inString;
            if (!inKey){
                inStringValue = true;
            }
        } else if (!inString) {
            //std::cout << "> c else  " << " <=> " << c << " | inKey " << inKey << " | inString " << inString  << std::endl;
            if (c == ':') {
                //std::cout << ">> c == :  " << " <=> " << c << std::endl;
                inKey = false;
            } else if (c == ',' || c == '}') {
                //std::cout << ">> c == , or } " << " <=> " << c << std::endl;
                keyValues[key] = value;
                key = "";
                value = "";
                inKey = true;
                inStringValue = false;
            }
        } else {
            if (inKey) {
                key += c;
                //std::cout << "-> KEY " << " <=> " << key << std::endl;

            } else {
                value += c;
                //std::cout << "-> VALUE " << " <=> " << value << std::endl;

            }
        }
        //std::cout << "--------------- "<< std::endl;


    }



    return keyValues;
}
