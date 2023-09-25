//�C���N���[�h
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

//�����J
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")

//�萔�錾
const char* WIN_CLASS_NAME = "SampleGame";	//�E�B���h�E�N���X��

//�E�B���h�E�T�C�Y�������Őݒ肵�����Ƃ��p
const int WINDOW_WIDTH = 1400;				//�E�B���h�E�̕�
const int WINDOW_HEIGHT = 1000;				//�E�B���h�E�̍���

//�O���[�o���ϐ�
RootJob* pRootjob = nullptr;

//�v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //�E�B���h�E�v���V�[�W��
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

//�G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{

	//////�f�B�X�v���C�̃T�C�Y���擾
	int displayWidth = GetSystemMetrics(SM_CXSCREEN);
	int displayHeight = GetSystemMetrics(SM_CYSCREEN);


	//�E�B���h�E�N���X�i�݌v�}�j���쐬
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);					//���̍\���̂̃T�C�Y
	wc.hInstance = hInstance;						//�C���X�^���X�n���h��
	wc.lpszClassName = WIN_CLASS_NAME;				//�E�B���h�E�N���X��
	wc.lpfnWndProc = WndProc;						//�E�B���h�E�v���V�[�W��
	wc.style = CS_VREDRAW | CS_HREDRAW;				//�X�^�C���i�f�t�H���g�j
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);		//�A�C�R��
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);		//�������A�C�R��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);		//�}�E�X�J�[�\��
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);	//���j���[
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);	//�w�i�i�D�F�j
	RegisterClassEx(&wc);									//�N���X��o�^


	//�E�B���h�E�T�C�Y�̌v�Z
	RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	//RECT winRect = { 0, 0, displayWidth, displayHeight };
	AdjustWindowRect(&winRect, WS_POPUP, TRUE);
	int winW = winRect.right - winRect.left;     //�E�B���h�E��
	int winH = winRect.bottom - winRect.top;     //�E�B���h�E����


	//�E�B���h�E���쐬
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,			//�E�B���h�E�N���X��
		"���s��",				//�^�C�g���o�[�ɕ\��������e
		WS_POPUP,				//�X�^�C���i���ʂ̃E�B���h�E WS_OVERLAPPEDWINDOW �t���X�N WS_POPUP�j
		CW_USEDEFAULT,			//�\���ʒu���i���܂����j
		CW_USEDEFAULT,			//�\���ʒu��i���܂����j
		winW,					//�E�B���h�E��
		winH,					//�E�B���h�E����
		NULL,					//�e�E�C���h�E�i�Ȃ��j
		NULL,					//���j���[�i�Ȃ��j
		hInstance,				//�C���X�^���X
		NULL					//�p�����[�^�i�Ȃ��j
	);

	//�E�B���h�E��\��
	ShowWindow(hWnd, nCmdShow);

	//Direct3D������
	Direct3D::Initialize(winW, winH, hWnd);

	//���͏�����
	Input::Initialize(hWnd);

	//���[�g�W���u�쐬
	pRootjob = new RootJob(nullptr);
	pRootjob->Initialize();

	//�J����������
	Camera::Initialize();
	Camera::SetPosition(XMFLOAT3(0, 4, -10));
	Camera::SetTarget(XMFLOAT3(0, 0, 0));

	//�_�C�A���O�쐬
	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)DialogProc);

	//���b�Z�[�W���[�v�i�����N����̂�҂j
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//���b�Z�[�W����
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//���b�Z�[�W�Ȃ�
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

			//�Q�[���̏���
			Direct3D::BeginDraw();

			//�`�揈��
			pRootjob->DrawSub();


			Direct3D::EndDraw();

			//���͏��̍X�V
			Input::Update();

		}
	}

	//�������
	Model::Release();	
	pRootjob->ReleaseSub();
	SAFE_DELETE(pRootjob);

	Input::Release();	
	Direct3D::Release();

	return 0;
}

//�E�B���h�E�v���V�[�W���i�������������ɂ�΂��֐��j
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int a = 0;
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);		//�v���O�����I��
		return 0;

	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));		//�}�E�X���W�Z�b�g
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
			int result = MessageBox(hWnd, "�A�v���P�[�V�������I�����܂����H", "�I���m�F", MB_YESNO | MB_ICONQUESTION);
			if (result == IDYES) PostQuitMessage(0);
		}



	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//�_�C�A���O�v���V�[�W��
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    Stage* pStage = (Stage*)pRootjob->FindObject("Stage");
    return pStage->DialogProc(hDlg, msg, wp, lp);
}