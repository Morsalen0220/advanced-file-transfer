#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"comctl32.lib")

#define DISCOVERY_PORT 7777
#define TCP_PORT 9000
#define BUF 65536
#define MAX_RCV 32

typedef struct{
    char name[64];
    char ip[32];
} Receiver;

Receiver rcvs[MAX_RCV];
int rcv_count = 0;
int selected = -1;

HWND hList, hProg;

int send_all(SOCKET s, const char *buf, int len){
    int sent = 0;
    while(sent < len){
        int n = send(s, buf + sent, len - sent, 0);
        if(n <= 0) return -1;
        sent += n;
    }
    return sent;
}

void discover_receivers(){
    WSADATA w;
    WSAStartup(MAKEWORD(2,2), &w);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    BOOL b = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&b, sizeof(b));

    struct sockaddr_in baddr = {0};
    baddr.sin_family = AF_INET;
    baddr.sin_port = htons(DISCOVERY_PORT);
    baddr.sin_addr.s_addr = INADDR_BROADCAST;

    sendto(s, "UF_DISCOVER", 11, 0,
           (SOCKADDR*)&baddr, sizeof(baddr));

    DWORD start = GetTickCount();

    while(GetTickCount() - start < 3000 && rcv_count < MAX_RCV){
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(s, &fds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200 * 1000;

        if(select(0, &fds, NULL, NULL, &tv) > 0){
            struct sockaddr_in from;
            int fl = sizeof(from);
            char buf[64];

            int n = recvfrom(s, buf, sizeof(buf)-1, 0,
                             (SOCKADDR*)&from, &fl);
            if(n > 0){
                buf[n] = 0;
                strcpy(rcvs[rcv_count].name, buf);
                strcpy(rcvs[rcv_count].ip,
                       inet_ntoa(from.sin_addr));
                rcv_count++;
            }
        }
    }
    closesocket(s);
}

DWORD WINAPI send_file(LPVOID){
    char path[MAX_PATH] = {0};
    OPENFILENAME ofn = {sizeof(ofn)};
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files\0*.*\0";

    if(!GetOpenFileName(&ofn)) return 0;

    FILE *f = fopen(path,"rb");
    _fseeki64(f,0,SEEK_END);
    long long size = _ftelli64(f);
    _fseeki64(f,0,SEEK_SET);

    char *fname = strrchr(path,'\\');
    fname = fname ? fname+1 : path;
    int flen = strlen(fname)+1;

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in a = {0};
    a.sin_family = AF_INET;
    a.sin_port = htons(TCP_PORT);
    a.sin_addr.s_addr = inet_addr(rcvs[selected].ip);

    connect(s,(SOCKADDR*)&a,sizeof(a));

    send_all(s,(char*)&flen,sizeof(int));
    send_all(s,fname,flen);
    send_all(s,(char*)&size,sizeof(long long));

    char buf[BUF];
    long long sent = 0;

    while(!feof(f)){
        int n = fread(buf,1,BUF,f);
        if(n <= 0) break;
        send_all(s,buf,n);
        sent += n;
        int p = (int)((sent * 100LL) / size);
        SendMessage(hProg, PBM_SETPOS, p, 0);
    }

    fclose(f);
    closesocket(s);
    MessageBox(0,"Transfer complete","Sender",MB_OK);
    return 0;
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l){
    if(m == WM_CREATE){
        hList = CreateWindow("LISTBOX","",
            WS_CHILD|WS_VISIBLE|WS_BORDER,
            20,20,340,120,
            h,(HMENU)1,0,0);

        for(int i=0;i<rcv_count;i++){
            char line[128];
            sprintf(line,"%s (%s)",
                    rcvs[i].name,
                    rcvs[i].ip);
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)line);
        }

        CreateWindow("BUTTON","Send File",
            WS_CHILD|WS_VISIBLE,
            130,150,120,30,
            h,(HMENU)2,0,0);

        hProg = CreateWindow(PROGRESS_CLASS,"",
            WS_CHILD|WS_VISIBLE,
            20,190,340,20,
            h,0,0,0);
        SendMessage(hProg, PBM_SETRANGE, 0, MAKELPARAM(0,100));
    }

    if(m == WM_COMMAND && LOWORD(w) == 2){
        selected = SendMessage(hList, LB_GETCURSEL, 0, 0);
        if(selected != LB_ERR)
            CreateThread(0,0,send_file,0,0,0);
    }

    if(m == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(h,m,w,l);
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int){
    INITCOMMONCONTROLSEX ic={sizeof(ic),ICC_PROGRESS_CLASS};
    InitCommonControlsEx(&ic);

    discover_receivers();
    if(rcv_count == 0){
        MessageBox(0,"No receiver found","Sender",MB_OK);
        return 0;
    }

    WNDCLASS wc={0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "UF_P3";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        "UF_P3","Select Receiver & Send",
        WS_OVERLAPPED|WS_SYSMENU,
        500,300,400,260,
        0,0,hInst,0);

    ShowWindow(hWnd,SW_SHOW);

    MSG msg;
    while(GetMessage(&msg,0,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
