//
// Created by borelset on 2/2/18.
//

#ifndef EP_MENU_H
#define EP_MENU_H

#include <list>
#include <functional>
#include <vector>


class MenuItem{
public:
    typedef std::function<void(std::vector<std::string>)> Callback;
    MenuItem(const std::string&, const std::string&, const Callback&);
    std::string getName();
    std::string getDescribe();
    Callback getCallback();
private:
    std::string mName;
    Callback mCallback;
    std::string mDescribe;
};

class Menu{
public:
    typedef std::function<void()> Callback;
    Menu();
    ~Menu();
    void setInit(const std::function<void()>&);
    void addCommand(const MenuItem&);
    void startMenu();
private:
    std::function<void()> mInitCallback;
    std::list<MenuItem> mCommands;
    bool mExit;

    void init();
    void help();
};

#endif //EP_MENU_H
