#pragma once
#include <d3d11.h>
#include <string>
#include <wrl.h>

using namespace Microsoft::WRL;
using std::string;

class Texture
{
	ID3D11SamplerState* pSampler_;		//サンプラー作成
	ID3D11ShaderResourceView* pSRV_;	//シェーダーリソースビュー作成
public:
	Texture();
	~Texture();

	// テクスチャの読み込み
	HRESULT Load(string fileName);

	// テクスチャの解放
	void Release();

	// サンプラーステートの取得
	ID3D11SamplerState* GetSampler() { return pSampler_; }

	// シェーダーリソースビューの取得
	ID3D11ShaderResourceView* GetSRV() { return pSRV_; }

};