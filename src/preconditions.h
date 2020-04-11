//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_PRECONDITIONS_H
#define DOTS_CLIENT_PRECONDITIONS_H
#include <stdlib.h>
#include "log.h"

#define check_valid(condition, ...) ({ if (!(condition)) {log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__); exit(EXIT_FAILURE);}})
#define panic(...) ({log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__); exit(EXIT_FAILURE);})
#endif //DOTS_CLIENT_PRECONDITIONS_H
