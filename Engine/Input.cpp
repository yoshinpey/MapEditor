#include "Input.h"

namespace Input
{
	LPDIRECTINPUT8   pDInput = nullptr;			// DirectInputオブジェクト
	LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr;	// キーボードデバイスオブジェクト
	BYTE keyState[256] = { 0 };					// 現在の各キーの状態
	BYTE prevKeyState[256];						// 前フレームでの各キーの状態

	HRESULT Initialize(HWND hWnd)
	{
		HRESULT hr = 0;
		//DirectInputオブジェクトの作成
		hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput, nullptr);
		if (FAILED(hr))
		{
			MessageBox(NULL, "DirectInputオブジェクトの作成に失敗しました", "エラー", MB_OK);
			return hr;
		}

		//キーボードデバイスの作成
		hr = pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr);
		if (FAILED(hr))
		{
			MessageBox(NULL, "キーボードデバイスの作成に失敗しました", "エラー", MB_OK);
			return hr;
		}

		//データフォーマットの設定
		hr = pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr))
		{
			MessageBox(NULL, "データフォーマットの設定に失敗しました", "エラー", MB_OK);
			return hr;
		}

		//協調レベルの設定
		hr = pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		if (FAILED(hr))
		{
			MessageBox(NULL, "協調レベルの設定に失敗しました", "エラー", MB_OK);
			return hr;
		}
		return S_OK;
	}

	void Update()
	{
		//キーボードデバイスの取得
		pKeyDevice->Acquire();

		//前フレームのキー状態を保存
		memcpy(prevKeyState, keyState, sizeof(keyState));

		//現在のキー状態の取得
		pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);
	}

	bool IsKey(int keyCode)
	{
		//指定されたキーコードのキーが押されているかチェック
		if (keyState[keyCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		//キーが現在押されており、前回のフレームでは押されていないかチェック
		if (IsKey(keyCode) && !(prevKeyState[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int keyCode)
	{
		//キーが現在押されておらず、前回のフレームでは押されていたかチェック
		if (!IsKey(keyCode) && prevKeyState[keyCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	void Release()
	{
		//リソースの解放
		SAFE_RELEASE(pDInput);
		SAFE_RELEASE(pKeyDevice);
	}
}

