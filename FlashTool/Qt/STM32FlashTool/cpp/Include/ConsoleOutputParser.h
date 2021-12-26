//
// Created by florianfrank on 2021-12-26.
//

#ifndef STM32FLASHTOOL_CONSOLEOUTPUTPARSER_H
#define STM32FLASHTOOL_CONSOLEOUTPUTPARSER_H


#include <QString>

class ConsoleOutputParser
{
public:
    static QString RemoveBracesandPercentageSymbolFromRegexMatch(QString &regexMatch);
    static int getProgressInPercentFromComilationString(QString &compileString);
    static QString searchForCompileProgressInPercent(QString &message);

};


#endif //STM32FLASHTOOL_CONSOLEOUTPUTPARSER_H
