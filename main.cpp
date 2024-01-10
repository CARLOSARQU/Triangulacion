#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"
#include <vector>
#include <algorithm>

using namespace std;

HINSTANCE hInst;

struct Punto
{
    double x, y;
};

struct Triangulo
{
    Punto puntos[3];
};

vector<Punto> puntos;
vector<Triangulo> triangulos;

bool EsCircuncentroEnTriangulo( Punto& p,  Triangulo& t)
{
    double detT = (t.puntos[1].y - t.puntos[2].y) * (t.puntos[0].x - t.puntos[2].x) +
                  (t.puntos[2].x - t.puntos[1].x) * (t.puntos[0].y - t.puntos[2].y);

    double alpha = ((p.y - t.puntos[2].y) * (t.puntos[0].x - t.puntos[2].x) +
                    (t.puntos[2].x - t.puntos[0].x) * (p.y - t.puntos[2].y)) / detT;

    double beta = ((t.puntos[1].y - t.puntos[2].y) * (p.x - t.puntos[2].x) +
                   (t.puntos[2].x - t.puntos[1].x) * (p.y - t.puntos[2].y)) / detT;

    double gamma = 1.0 - alpha - beta;

    return alpha >= 0 && beta >= 0 && gamma >= 0;
}

void InsertarPuntoEnTriangulacion( Punto& nuevoPunto)
{
    vector<Triangulo> nuevosTriangulos;

    for ( auto& triangulo : triangulos)
    {
        if (EsCircuncentroEnTriangulo(nuevoPunto, triangulo))
        {
            Triangulo t1{{triangulo.puntos[0], triangulo.puntos[1], nuevoPunto}};
            Triangulo t2{{triangulo.puntos[1], triangulo.puntos[2], nuevoPunto}};
            Triangulo t3{{triangulo.puntos[2], triangulo.puntos[0], nuevoPunto}};

            nuevosTriangulos.push_back(t1);
            nuevosTriangulos.push_back(t2);
            nuevosTriangulos.push_back(t3);
        }
        else
        {
            nuevosTriangulos.push_back(triangulo);
        }
    }

    triangulos = nuevosTriangulos;
}

void CalcularDelaunay()
{
    triangulos.clear();

    if (puntos.size() >= 3)
    {
        Triangulo primerTriangulo{{puntos[0], puntos[1], puntos[2]}};
        triangulos.push_back(primerTriangulo);

        for (int i = 3; i < puntos.size(); ++i)
        {
            InsertarPuntoEnTriangulacion(puntos[i]);
        }
    }
}
Punto B;

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_PAINT:
        {   PAINTSTRUCT ps;
            HDC hdc;
            hdc = BeginPaint(hwndDlg, &ps);

            CalcularDelaunay();

            for ( auto punto : puntos)
            {
                Ellipse(hdc, punto.x - 5, punto.y - 5, punto.x + 5, punto.y + 5);
            }

            for ( auto triangulo : triangulos)
            {
                MoveToEx(hdc, triangulo.puntos[0].x, triangulo.puntos[0].y, NULL);
                LineTo(hdc, triangulo.puntos[1].x, triangulo.puntos[1].y);
                LineTo(hdc, triangulo.puntos[2].x, triangulo.puntos[2].y);
                LineTo(hdc, triangulo.puntos[0].x, triangulo.puntos[0].y);
            }

            EndPaint(hwndDlg, &ps);
        }
        return TRUE;

        return TRUE;

        case WM_LBUTTONDOWN:
        {   long xPos = GET_X_LPARAM(lParam);
            long yPos = GET_Y_LPARAM(lParam);

            Punto nuevoPunto = { xPos, yPos };
            puntos.push_back(nuevoPunto);

            InvalidateRect(hwndDlg, NULL, true);
        }
        return TRUE;


        case WM_MOUSEMOVE:
        {

            long xPos = GET_X_LPARAM(lParam);
            long yPos = GET_Y_LPARAM(lParam);

            SetDlgItemInt(hwndDlg,EDITX,(int)xPos,FALSE);
            SetDlgItemInt(hwndDlg,EDITY,(int)yPos,FALSE);


            B.x=xPos;
            B.y=yPos;
            InvalidateRect(hwndDlg,NULL,true);



        }
        return TRUE;

        case WM_INITDIALOG:
        {

        }
        return TRUE;

        case WM_CLOSE:
        {

            EndDialog(hwndDlg, 0);
        }
        return TRUE;

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {

            }
        }
        return TRUE;
    }
    return FALSE;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
}
