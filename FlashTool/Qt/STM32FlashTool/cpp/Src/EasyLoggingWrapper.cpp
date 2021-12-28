//
// Created by florianfrank on 28.12.21.
//

#include <EasyLoggingWrapper.h>

void EasyLoggingWrapper::InitializeLogging(int argc, char *argv[])
{
    // START LOGGING
    START_EASYLOGGINGPP(argc, argv);

    el::Configurations defaultConf;
    defaultConf.setToDefault();
    // Values are always std::string
    defaultConf.set(el::Level::Info,
                    el::ConfigurationType::Format, "%datetime %level %msg");
    // default logger uses default configurations
    el::Loggers::reconfigureLogger("default", defaultConf);
    LOG(INFO) << "Start logging into file: " << "STM32FlashTool.log";
    // To set GLOBAL configurations you may use
}
