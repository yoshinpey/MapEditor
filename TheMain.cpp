//インクルード
#include <Windows.h>
#include <d3d11.h>
#include <stdlib.h>
#include <DirectXCollision.h>

#include "Engine/Direct3D.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"
#include "Engine/Rootjob.h"
#include "Engine/Model.h"

#include "resource.h"
#include "Stage.h"

//リンカ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")

//定数宣言
const char* WIN_CLASS_NAME = "SampleGame";	//ウィンドウクラス名

//ウィンドウサイズを自分で設定したいとき用
const int WINDOW_WIDTH = 1400;				//ウィンドウの幅
const int WINDOW_HEIGHT = 1000;				//ウィンドウの高さ

//グローバル変数
RootJob* pRootjob = nullptr;

//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //ウィンドウプロシージャ
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

//エントリーポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{

	//////ディスプレイのサイズを取得
	int displayWidth = GetSystemMetrics(SM_CXSCREEN);
	int displayHeight = GetSystemMetrics(SM_CYSCREEN);


	//ウィンドウクラス（設計図）を作成
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);					//この構造体のサイズ
	wc.hInstance = hInstance;						//インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;				//ウィンドウクラス名
	wc.lpfnWndProc = WndProc;						//ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;				//スタイル（デフォルト）
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);		//アイコン
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);		//小さいアイコン
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);		//マウスカーソル
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);	//メニュー
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);	//背景（灰色）
	RegisterClassEx(&wc);									//クラスを登録


	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	//RECT winRect = { 0, 0, displayWidth, displayHeight };
	AdjustWindowRect(&winRect, WS_POPUP, TRUE);
	int winW = winRect.right - winRect.left;     //ウィンドウ幅
	int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ


	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,			//ウィンドウクラス名
		"実行中",				//タイトルバーに表示する内容
		WS_POPUP,				//スタイル（普通のウィンドウ WS_OVERLAPPEDWINDOW フルスク WS_POPUP）
		CW_USEDEFAULT,			//表示位置左（おまかせ）
		CW_USEDEFAULT,			//表示位置上（おまかせ）
		winW,					//ウィンドウ幅
		winH,					//ウィンドウ高さ
		NULL,					//親ウインドウ（なし）
		NULL,					//メニュー（なし）
		hInstance,				//インスタンス
		NULL					//パラメータ（なし）
	);

	//ウィンドウを表示
	ShowWindow(hWnd, nCmdShow);

	//Direct3D初期化
	Direct3D::Initialize(winW, winH, hWnd);

	//入力初期化
	Input::Initialize(hWnd);

	//ルートジョブ作成
	pRootjob = new RootJob(nullptr);
	pRootjob->Initialize();

	//カメラ初期化
	Camera::Initialize();
	Camera::SetPosition(XMFLOAT3(0, 4, -10));
	Camera::SetTarget(XMFLOAT3(0, 0, 0));

	//ダイアログ作成
	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)DialogProc);

	//メッセージループ（何か起きるのを待つ）
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//メッセージあり
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//メッセージなし
		else
		{
			timeBeginPeriod(1);

			static DWORD countFps = 0;
			static DWORD startTime = timeGetTime();
			DWORD nowTime = timeGetTime();
			static DWORD lastUpdateTime = nowTime;

			if (nowTime - startTime >= 1000)
			{
				char str[16];
				wsprintf(str, "%u", countFps);
				SetWindowText(hWnd, str);

				countFps = 0;
				startTime = nowTime;
			}

			if ((nowTime - lastUpdateTime) * 60 <= 1000)
			{
				continue;
			}
			lastUpdateTime = nowTime;

			countFps++;

			timeEndPeriod(1);

			Camera::Update();
			pRootjob->UpdateSub();

			//ゲームの処理
			Direct3D::BeginDraw();

			//描画処理
			pRootjob->DrawSub();


			Direct3D::EndDraw();

			//入力情報の更新
			Input::Update();

		}
	}

	//解放処理
	Model::Release();	
	pRootjob->ReleaseSub();
	SAFE_DELETE(pRootjob);

	Input::Release();	
	Direct3D::Release();

	return 0;
}

//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int a = 0;
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);		//プログラム終了
		return 0;

	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));		//マウス座標セット
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_MENU_NEW:
			a++;
			break;
		case ID_MENU_OPEN:
			a++;
			break;
		case ID_MENU_SAVE:
			((Stage*)pRootjob->FindObject("Stage"))->Save();
			return 0;
		}

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) 
		{
			int result = MessageBox(hWnd, "アプリケーションを終了しますか？", "終了確認", MB_YESNO | MB_ICONQUESTION);
			if (result == IDYES) PostQuitMessage(0);
		}



	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//ダイアログプロシージャ
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    Stage* pStage = (Stage*)pRootjob->FindObject("Stage");
    return pStage->DialogProc(hDlg, msg, wp, lp);
}