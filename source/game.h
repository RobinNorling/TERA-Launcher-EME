#ifndef GAME_H
#define GAME_H
#include <QPushButton>
#include <Windows.h>
#include <string>

void launchGame() {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char currentDirectory[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDirectory);

    std::string processToRun = std::string(currentDirectory) + "\\Client\\TL.exe";
    std::string workingDirectory = std::string(currentDirectory) + "\\Client";
    CreateProcessA(processToRun.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, workingDirectory.c_str(), &si, &pi);
}

#endif // GAME_H
