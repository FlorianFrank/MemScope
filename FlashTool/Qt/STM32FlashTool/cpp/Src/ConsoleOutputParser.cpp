//
// Created by florianfrank on 2021-12-26.
//

#include "cpp/Include/ConsoleOutputParser.h"
#include <regex>


/*static*/ QString ConsoleOutputParser::RemoveBracesandPercentageSymbolFromRegexMatch(QString &regexMatch)
{
    std::string match = regexMatch.toStdString();
    std::regex replaceExpr( R"(\[|\]|%|\s)");
    QString percentageStrCleaned(std::regex_replace(match,replaceExpr, "").c_str());
    return percentageStrCleaned;
}

/*static*/ int ConsoleOutputParser::getProgressInPercentFromComilationString(QString &compileString)
{
    QString regexMatch = searchForCompileProgressInPercent(compileString);
    if (regexMatch.size() > 0)
    {
        QString percentageInString = ConsoleOutputParser::RemoveBracesandPercentageSymbolFromRegexMatch(regexMatch);
        return percentageInString.toInt();
    }
    return -1;
}

/*static*/ QString ConsoleOutputParser::searchForCompileProgressInPercent(QString &message)
{
    std::regex searchExpr(R"(\[\s?\d{1,3}%\])");
    std::smatch regexMatch;
    std::string messageStdString = message.toStdString();
    std::regex_search(messageStdString, regexMatch, searchExpr);
    QString firstRegexMatch(regexMatch[0].str().c_str());
    return firstRegexMatch;
}
