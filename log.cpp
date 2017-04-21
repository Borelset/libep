#include <iostream>
#include <fstream>
#include <time.h>
#include <string.h>

#include "log.h"

using namespace std;

log::log()
{
    time_t current_time = time(0);
    struct tm *_time;
    _time = localtime(&current_time);

    char time_string[128];
    sprintf(time_string, "%d%02d%02d_%02d%02d%02d",
            _time->tm_year+1900, _time->tm_mon+1, _time->tm_mday, _time->tm_hour, _time->tm_min, _time->tm_sec);
    char name[50] = "./log/";
    strcat(name, time_string);
    strcat(name, ".log");

    fout.open(name, ios::out);
}

log& log::operator<<(char* input)
{
    if(input == nullptr) return *this;

    if(this->flag == 0)
    {
        time_t current_time = time(0);
        struct tm *_time;
        _time = localtime(&current_time);

        char time_string[128];
        sprintf(time_string, "%d/%02d/%02d-%02d:%02d:%02d",
                _time->tm_year+1900, _time->tm_mon+1, _time->tm_mday, _time->tm_hour, _time->tm_min, _time->tm_sec);
        cout << time_string << "\t";
        fout << time_string << "\t";
        this->flag = 1;
    }

    cout << input;
    fout << input;

    return *this;
}

log& log::operator<<(log& input)
{
    cout << endl;
    fout << endl;
    this->flag = 0;
    return *this;
}
