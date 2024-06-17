#ifndef LOG_H
#define LOG_H

/* Log prefixes */

#define TEXT_BOLD "\e[1m"
#define TEXT_RESET "\e[0m"

#define LOG_PREFIX_INFO          "\x1b[34;1m" "[INFO]: " TEXT_RESET    /* Information messages */
#define LOG_PREFIX_WARN          "\x1b[33;1m" "[WARN]: " TEXT_RESET    /* Warning messages */
#define LOG_PREFIX_ERROR         "\x1b[31;1m" "[ERROR]: " TEXT_RESET     /* Error messages */
#define LOG_PREFIX_GAME          "\x1b[32;1m" "[GAME]: " TEXT_RESET   /* Game output */

#define log_info(...) printf(LOG_PREFIX_INFO __VA_ARGS__ "\n")
#define log_error(...) printf(LOG_PREFIX_ERROR __VA_ARGS__ "\n")
#define log_warn(...) printf(LOG_PREFIX_WARN __VA_ARGS__ "\n")
#define log_game(...) printf(LOG_PREFIX_GAME __VA_ARGS__ "\n")

#endif
