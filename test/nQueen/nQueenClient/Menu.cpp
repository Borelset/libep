//
// Created by borelset on 2/2/18.
//

#include "Menu.h"
#include <cstring>

MenuItem::MenuItem(const std::string & name,
                   const std::string &describe,
                   const Callback& callback):
        mName(name),
        mDescribe(describe),
        mCallback(callback)
{

}

std::string MenuItem::getName() {
    return mName;
}

std::string MenuItem::getDescribe() {
    return mDescribe;
}

MenuItem::Callback MenuItem::getCallback() {
    return mCallback;
}

Menu::Menu():
    mExit(false)
{
    MenuItem items[] = {
            {
                    std::string("exit"),
                    std::string("Exit from program"),
                    [this](std::vector<std::string>){mExit = true;}
            },
            {
                    std::string("help"),
                    std::string("List all commands"),
                    std::bind(&Menu::help, this)
            }
    };
    for(auto item : items){
        addCommand(item);
    }
}

void Menu::init() {
    mInitCallback();
}

void Menu::help() {
    printf("==============help==============\n");
    printf("Command\tDescribe\n");
    printf("--------------------------------\n");
    for(MenuItem i : mCommands){
        printf("%s\t%s\n", i.getName().c_str(), i.getDescribe().c_str());
    }
    printf("================================\n");
}

void Menu::startMenu() {
    init();
    char buffer[1024];
    bzero(buffer, sizeof buffer);
    while(!mExit){
        printf("$");
        std::vector<std::string> args;
        for(int i=0; i<1024; i++){
            buffer[i] = getchar();
            if(buffer[i] == ' '){
                if(strlen(buffer)!=0){
                    buffer[i] = '\0';
                    args.push_back(buffer);
                    bzero(buffer, sizeof buffer);
                    i = -1;
                }
            }
            else if(buffer[i] == '\n'){
                buffer[i] = '\0';
                args.push_back(buffer);
                bzero(buffer, sizeof buffer);
                break;
            }
        }
        bool found = false;
        for(auto i : mCommands){
            if(!strcmp(i.getName().c_str(), args.begin()->c_str())){
                i.getCallback()(args);
                args.clear();
                found = true;
                break;
            }
        }
        if(!found){
            printf("Undefined command. Type help to find available commands\n");
        }
    }
}

void Menu::setInit(const std::function<void()>& callback) {
    mInitCallback = callback;
}

void Menu::addCommand(const MenuItem & item) {
    mCommands.push_front(item);
}

Menu::~Menu() {
}
