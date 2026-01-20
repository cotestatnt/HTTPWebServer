#pragma once
#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DEBUG_PORT Serial

#define LOG_VERBOSE     3
#define LOG_DEBUG       2
#define LOG_ERROR       1
#define LOG_LEVEL       0


#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define PRINT_TIME() DEBUG_PORT.print(millis());
#define PRINT_FILE_LINE() {PRINT_TIME() DEBUG_PORT.print(" [");DEBUG_PORT.print(__FILENAME__); \
    DEBUG_PORT.print(":");DEBUG_PORT.print(__LINE__);DEBUG_PORT.print(" "); DEBUG_PORT.print(__FUNCTION__); DEBUG_PORT.print("()]\t");}


#if LOG_LEVEL == LOG_VERBOSE
#define log_error(format, args...) { PRINT_FILE_LINE() char logBuffer[250]; snprintf(logBuffer, 250, "[E]: " format, args); DEBUG_PORT.println(logBuffer);}
#define log_debug(format, args...) { PRINT_FILE_LINE() char logBuffer[250]; snprintf(logBuffer, 250, "[D]: " format, args); DEBUG_PORT.println(logBuffer);}
#define log_verbose(x) {PRINT_FILE_LINE() DEBUG_PORT.print("[I]: "); DEBUG_PORT.println(x);}

#elif LOG_LEVEL == LOG_DEBUG
#define log_error(format, args...) { PRINT_FILE_LINE() char logBuffer[250]; snprintf(logBuffer, 250, "[E]: " format, args); DEBUG_PORT.println(logBuffer);}
#define log_debug(format, args...) { PRINT_FILE_LINE() char logBuffer[250]; snprintf(logBuffer, 250, "[D]: " format, args); DEBUG_PORT.println(logBuffer);}
#define log_verbose(x)

#elif LOG_LEVEL == LOG_ERROR
#define log_error(format, args...) { PRINT_FILE_LINE() char logBuffer[250]; snprintf(logBuffer, 250, "[E]: " format, args); DEBUG_PORT.println(logBuffer);}
#define log_debug(format, args...)
#define log_verbose(x)

#else
#define log_error(format, args...)
#define log_debug(format, args...)
#define log_verbose(x)
#endif

#ifdef __cplusplus
}
#endif

#endif