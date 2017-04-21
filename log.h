#ifndef LOG_H
#define LOG_H
#include <fstream>

typedef class log
{
    std::fstream fout;
    int flag = 0;
public:
    log();
    log& operator<<(char* input);
    log& operator<<(log& input);
}_log;

#endif // LOG_H
