#pragma once

#include <dInput.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}

namespace Input
{
	//初期化
	HRESULT Initialize(HWND hWnd);

	//更新
	void Update();

	//キーが押されているか調べる
	bool IsKey(int keyCode);

	//キーを今押したか調べる（押しっぱなしは無効）
	bool IsKeyDown(int keyCode);

	//キーを今放したか調べる
	bool IsKeyUp(int keyCode);

	//解放処理
	void Release();
};