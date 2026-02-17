#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <shellapi.h>

void putenvpath(char *Folder_name){
    char old_path[8192]; 
    GetEnvironmentVariableA("PATH", old_path, 8192);

    char path[MAX_PATH];
    _fullpath(path, Folder_name, MAX_PATH);
    
    //manuall size count
    int size_old_path = 0;
    int size_path = 0;

    while(old_path[size_old_path] != '\0') size_old_path++;
    for(size_path = 0; path[size_path]!= '\0'; size_path++);
    size_path += size_old_path + 7;

    // Allocate on the HEAP to avoid Stack Overflow
    // Size = folder + semicolon + old + null
    char *new_path = (char*)malloc(sizeof(char)*size_path);
    if (new_path==NULL) return;

    //Add the new folder path
    int buf_i = 0;
    int i;
    for(i = 0; path[i] != '\0'; new_path[buf_i] = path[i]) {
        buf_i++;
        i++;
    }
        // Add semicolon separator
        new_path[buf_i++] = ';'; 

    //add path to the new path string
    for(i = 0; old_path[i] != '\0'; new_path[buf_i] = old_path[i]) {
        buf_i++;
        i++;
    }

    new_path[buf_i] = '\0';
    
    if(SetEnvironmentVariableA("PATH", new_path) == 0) printf("Failed to set PATH environment variable.\n");
    else printf("PATH environment variable set to: %s\n", new_path);
    return;
}

int main(){
    char cwd[MAX_PATH];
    if (_getcwd(cwd, MAX_PATH) != NULL) {
    printf("Launcher is currently running in: %s\n", cwd);
    } else {
        printf("Failed to get current working directory.\n");
        return 1;
    }

    //set path for dlls
    char current_path[MAX_PATH]; 
    GetEnvironmentVariableA("PATH", current_path, MAX_PATH);

    putenvpath(".\\lib");


    //running
    HINSTANCE code = ShellExecuteA(
        NULL, 
        "open", 
        "calculator_core.exe", 
        NULL, 
        "bin", 
        SW_SHOWNORMAL
    );

    if(code > (HINSTANCE)32) printf("Calculator started successfully.\n");
    else {
        printf("Failed to start calculator.exe Error code: %lu\n", GetLastError());
    }

    return 0;
}