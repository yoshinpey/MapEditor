#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Transform.h"
#include "Direct3D.h"
#include "Texture.h"
#include "Camera.h"

using namespace DirectX;

#define SAFE_DELETE_ARRAY(p) if(p != nullptr){ delete p; p = nullptr;}

class Sprite
{
private:
	//コンスタントバッファー
	struct CONSTANT_BUFFER
	{
		XMMATRIX	matNormal;		//ワールド行列
	};

	//頂点情報
	struct VERTEX
	{
		XMVECTOR position;			//位置
		XMVECTOR uv;				//UV
	};

	//--------------Initilaizeから呼ばれる関数
	virtual void InitVertexData();		//頂点情報準備
	HRESULT CreateVertexBuffer();		//頂点バッファ作成
	virtual void InitIndexData();		//インデックス情報を準備
	HRESULT CreateIndexBuffer();		//インデックスバッファ作成
	HRESULT CreateConstantBuffer();		//コンスタントバッファ作成
	HRESULT LoadTexture();				//テクスチャをロード

	//--------------Draw関数から呼ばれる関数
	void PassDataToCB(Transform transformmatNormal);		//コンスタントバッファに各種情報を渡す
	void SetBufferToPipeline();								//各バッファをパイプラインにセット

protected:
	uint64_t vertexNum_;				//頂点数
	std::vector<VERTEX> vertices_;		//頂点情報
	ID3D11Buffer* pVertexBuffer_;		//頂点バッファ

	uint64_t indexNum_;					//インデックス数
	std::vector<int> index_;			//インデックス情報
	ID3D11Buffer* pIndexBuffer_;		//インデックスバッファ

	ID3D11Buffer* pConstantBuffer_;		//コンスタントバッファ

	Texture* pTexture_;					//テクスチャ

public:
	Sprite();
	~Sprite();

	//各情報初期化、戻り値：失敗/成功
	HRESULT Initialize();				

	//引数：transform	トランスフォームクラスオブジェクト
	void Draw(Transform& transform);	

	//解放
	void Release();						
};

