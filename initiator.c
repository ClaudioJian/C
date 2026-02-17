//this file is for developers uses


#include <stdlib.h>
#include <stdio.h>


//compiling .c files into .exe/.dll and running it
int main(){
    printf("Starting Build Process...\n");
    int return_code;
    

    //kill any existing instances (.exe) of the calculator to avoid file access issues during compilation
    system("taskkill /IM calculator.exe /F >nul 2>&1");
    system("taskkill /IM initiator.exe /F >nul 2>&1");
    system("del /Q user\\lib\\*.dll >nul 2>&1");
    system("del /Q devlib\\*.a >nul 2>&1");

    //CustomErrorcheck.dll
    return_code = system(
        "gcc -shared -o user/lib/CustomErrorcheck.dll"
        " devlib/CustomErrorcheck.c"         " -Idevlib" 
        " -Wl,--out-implib devlib/libCustomErrorcheck.a"
    );
    if(return_code == 0) printf("CustomErrorcheck.dll compiled successfully.\n");
    else printf("Failed to compile CustomErrorcheck.dll with return code: %d\n", return_code);    

    //CustomMath.dll
    return_code = system(
        "gcc -shared -o user/lib/CustomMath.dll" 
        " devlib/CustomMath.c"         " -Idevlib" 
        " -Ldevlib" " -lCustomErrorcheck"
        " -Wl,--out-implib devlib/libCustomMath.a"
    );
    if(return_code == 0) printf("CustomMath.dll compiled successfully.\n");
    else printf("Failed to compile CustomMath.dll with return code: %d\n", return_code);



    //calculator.exe
    return_code = system(
        "gcc main.c -o user/calculator.exe" 
        " -Ldevlib "  " -lCustomMath -lCustomErrorcheck" 
        " -Idevlib "
    );
    if(return_code == 0){
        printf("Compilation successful. Starting calculator.exe...\n");
        system("start cmd /k \"cd user && calculator.exe\"");
    }
    else printf("Compilation failed with return code: %d\n", return_code);

    return 0;
};