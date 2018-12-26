#include <iostream>
#include <windows.h>
#include <fstream>
#include <ctime>
#include <stdio.h>

using namespace std;

#define FILE_PATH_LOG "C:\\Worktimer"
#define FILE_NAME_LOG "worktimer"
#define SMALL_BUFFER_SIZE (256)
#define PROGRAM_NAME "CountDownTimer"
#define WAIT_SCREEN (500)

// Write Logentry
int writeLog(char* path, char* name, int duration, char* tag, const time_t timeStamp){
	struct tm* timeInfo = localtime(&timeStamp);

	char info[SMALL_BUFFER_SIZE];
	char filePath[SMALL_BUFFER_SIZE] = {0};
	if(path != NULL){
		strcpy(filePath, path);
		strcat(filePath, "\\");
	}
	strcat(filePath, name);
	strcat(filePath, ".log");

	char tmpTime[20];
	//TODO: uniform timeformat
	// or as comments next to the information
	strftime(tmpTime, 20, "%d%m%y;%H%M%S", timeInfo);

	ofstream file;
	file.open(filePath, ios::out | ios::app);
	if(file.is_open()){
		file << tmpTime << ";" << duration << ";" << tag << "\n";
	}else{
		sprintf(info, "ERROR with log\n%s;%s\nPATH:%s, NAME:%s", tmpTime, tag, filePath, name);
		MessageBox(NULL, info, PROGRAM_NAME, MB_SETFOREGROUND);
		return -1;
	}
	file.close();

	return 0;
}

int updateInformation(char* information, char* tag, time_t start, time_t stop){
    char buffer[SMALL_BUFFER_SIZE] = {0};

    // Basic Information
    struct tm * tmTmp;
    tmTmp = localtime(&start);
    sprintf(buffer, "%s\n\nSTART: %02d:%02d:%02d\n", tag, tmTmp->tm_hour, tmTmp->tm_min, tmTmp->tm_sec);
    tmTmp = localtime(&stop);
    sprintf(information, "%sSTOP:  %02d:%02d:%02d\n\nDuration: %d sec", buffer, tmTmp->tm_hour, tmTmp->tm_min, tmTmp->tm_sec, (int) stop - start);

    return 0;
}

int notifyFinish(void){
    // Black Screen as Notificmsg.wParam == 1ation
    HDC screenDC = GetDC(NULL);
    HBRUSH brush = CreateSolidBrush(RGB(0,0,0));
    SelectObject(screenDC, brush);
    Rectangle(screenDC, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    int i = 0;
    while(!GetAsyncKeyState(VK_ESCAPE)){
        if(WAIT_SCREEN < i++) break;
        Sleep(20);
    }

    InvalidateRect(NULL, NULL, FALSE);
    return 0;
}

int main(int argc, char* argv[])
{
    // Create a global Hotkey
    if(!RegisterHotKey(NULL, 1, MOD_ALT | MOD_SHIFT, VK_F9) ||
       !RegisterHotKey(NULL, 2, MOD_ALT | MOD_SHIFT, VK_F10)){
        cout << "Hotkey already exists (ALT + SHIFT + F9 or ALT + SHIFT + F10)" << endl;
        return -1;
    }

    // Information
    if(argc != 2 && argc != 3){
        cout <<  argv[0] << " [DURATION] ([TAG]) \n" <<
                "\n" <<
                "Show Console: Hotkey: ALT + SHIFT + F9\n" <<
                "Stop Counter: Hotkey: ALT + SHIFT + F10\n" <<
                "30 min\t1800\n" <<
                "40 min\t2400\n" <<
                "45 min\t2700\n" <<
                "50 min\t3000\n" <<
                "60 min\t3600\n" << endl;
        return -1;
    }

    // Get the Duration
    int duration = atoi(argv[1]);
    if(duration <= 0){
        cout << "wrong argument!" << endl;
        return -1;
    }

    // Get the Work Tag
    char tag[SMALL_BUFFER_SIZE] = {0};
    if(argc == 3) strcpy(tag, argv[2]);

    time_t start = time(NULL);
    time_t stop = start + duration;
    long int diff;
    struct tm * tmDiff;
    char information[SMALL_BUFFER_SIZE] = {0};
    bool isStopped = false;

    updateInformation(information, tag, start, stop);

    // Find the Console Window
    bool consoleVisible = true;
    HWND consoleHWND = FindWindowA("ConsoleWindowClass", NULL);

    // Mesage buffer
    MSG msg = {0};

    //while((diff = difftime(time(NULL), start)) <= duration){#
    while((diff = time(NULL) - start) <= duration){
        // Check for messages
        PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

        if(msg.message == WM_HOTKEY ){
            // Show Console
            if(msg.wParam == 1){
                if(consoleVisible){
                    consoleVisible = false;
                    ShowWindow(consoleHWND, SW_HIDE);
                }else{
                    consoleVisible = true;
                    ShowWindow(consoleHWND, SW_RESTORE);
                    SetForegroundWindow(consoleHWND);
                }
            // Stop Program
            } else if(msg.wParam == 2) {
                duration = diff;
                isStopped = true;
                updateInformation(information, tag, start, (stop = start + duration));
            }
        }
        // Reset Messagebuffer
        msg.message = 0;
        msg.wParam = 0;

        // Print passed time on Console
        if(consoleVisible){
            system("cls");

            tmDiff = localtime(&diff);
            cout << information << endl;
            fprintf(stdout, "%02d:%02d\n", tmDiff->tm_min, tmDiff->tm_sec);
        }

        Sleep(1000);
    }

    // Notify and put Console back to front
    if(!isStopped) notifyFinish();
    if(!consoleVisible) ShowWindow(consoleHWND, SW_RESTORE);
    SetForegroundWindow(consoleHWND);

    // Write Log Entry
    writeLog(FILE_PATH_LOG, FILE_NAME_LOG, duration, tag, start);

    //Final Information
    system("cls");
    diff--; // correct error in output
    tmDiff = localtime(&diff);
    cout << information << endl;
    fprintf(stdout, "%02d:%02d\n", tmDiff->tm_min, tmDiff->tm_sec);

    return 0;
}
