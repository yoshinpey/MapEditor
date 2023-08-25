#pragma once

//インクルード
#include <d3d11.h>

//リンカ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//マクロ定義
#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}
#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}

//シェーダーの種類
enum SHADER_TYPE
{
	SHADER_2D,		//2Dシェーダー
	SHADER_3D,		//3Dシェーダー
	SHADER_MAX		//シェーダーの最大数
};

namespace Direct3D
{
	//デバイスとデバイスコンテキスト
	extern ID3D11Device* pDevice_;
	extern ID3D11DeviceContext* pContext_;

	//初期化
	HRESULT Initialize(int winW, int winH, HWND hWnd);

	//シェーダー準備
	HRESULT InitShader();
	HRESULT InitShader3D();
	HRESULT InitShader2D();

	//使用するシェーダーを設定
	void SetShader(SHADER_TYPE type);

	//描画開始
	void BeginDraw();

	//描画終了
	void EndDraw();

	//解放
	void Release();
};

