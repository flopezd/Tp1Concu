#ifndef TP1_LOGGER_H
#define TP1_LOGGER_H

#include <iostream>

class Logger {
private:
    const char* fecha;
    static Logger* instance;

    Logger();
public:
    static Logger* getInstance();
    void loguear(const char*) ;
};


#endif //TP1_LOGGER_H
