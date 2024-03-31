#include <windows.h>
#include <gl/gl.h>
#include "buttons.h"
#include <string.h>

#define true 1
#define false 0

_Bool removeTriangle = 1;

_Bool rotateTriangle = 0;


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

float rotationAngle = 0.0f;

void Init()
{
    AddButton("Add Triangle", 10, 10, 200, 30, 2);
    AddButton("Remove Triangle", 10, 50, 200, 30, 2);
    AddButton("Rotate Triangle", 10, 90, 200, 30, 2);
    AddButton("Quit", 10, 130, 100, 30, 2);
}


void MouseClick()
{
    int buttonId = MouseDown();
    if (buttonId < 0) return;

    char *name = GetButtonName(buttonId);
    if (strcmp(name, "Quit") == 0)
    {
        PostQuitMessage(0);
    }
    else if (strcmp(name, "Add Triangle") == 0)
    {
        removeTriangle = false;
    }
    else if (strcmp(name, "Remove Triangle") == 0)
    {
        removeTriangle = true;
    }
    else if (strcmp(name, "Rotate Triangle") == 0)
    {
        rotateTriangle = !rotateTriangle; // Toggle rotation on each click
        if (!rotateTriangle)
        {
            rotationAngle = 0.0f; // Reset rotation angle if rotation is toggled off
        }
    }
}



int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          500,
                          500,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd, &rct);
    glOrtho(0, rct.right, rct.bottom, 0, 1, -1);

    Init();
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glPushMatrix();

            if (rotateTriangle)
            {
                glTranslatef(275.0f, 250.0f, 0.0f);
                glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
                glTranslatef(-275.0f, -250.0f, 0.0f);
            }

            if (!removeTriangle)
            {
                glBegin(GL_TRIANGLES);
                glColor3f(1, 0, 0);
                glVertex2f(250, 200);
                glColor3f(0, 1, 0);
                glVertex2f(200, 300);
                glColor3f(0, 0, 1);
                glVertex2f(300, 300);
                glEnd();

            }

            glPopMatrix();

            ShowMenu();

            SwapBuffers(hDC);

            rotationAngle += 1.0f;
            if (rotationAngle >= 360.0f)
            {
                rotationAngle -= 360.0f;
            }


            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_MOUSEMOVE:
            MouseMove(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_LBUTTONDOWN:
            MouseClick();
        break;

        case WM_LBUTTONUP:
            MouseUp();
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

