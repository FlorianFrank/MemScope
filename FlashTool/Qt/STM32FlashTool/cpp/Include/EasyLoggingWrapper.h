//
// Created by florianfrank on 28.12.21.
//

#ifndef STM32FLASHTOOL_LOGGING_H
#define STM32FLASHTOOL_LOGGING_H

#include "easylogging++.h"


class EasyLoggingWrapper
{
public:
    static void InitializeLogging(int argc, char *argv[]);
};


#endif //STM32FLASHTOOL_LOGGING_H
