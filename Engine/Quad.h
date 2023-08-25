#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Transform.h"
#include "Direct3D.h"
#include "Texture.h"
#include "Camera.h"

using namespace DirectX;

//コンスタントバッファー
struct CONSTANT_BUFFER
{
	XMMATRIX	matWVP;			//ワールド行列
	XMMATRIX	matNormal;		
};

//頂点情報
struct VERTEX
{
	XMVECTOR position;			//位置
	XMVECTOR uv;				//UV
	XMVECTOR normal;			
};

class Quad
{
private:
	//--------------Initilaizeから呼ばれる関数

	virtual void InitVertexData();		//頂点情報準備
	virtual void InitIndexData();		//インデックス情報を準備

	HRESULT CreateVertexBuffer();		//頂点バッファ作成
	HRESULT CreateIndexBuffer();		//インデックスバッファ作成
	HRESULT CreateConstantBuffer();		//コンスタントバッファ作成

	HRESULT LoadTexture();				//テクスチャをロード

	//---------Draw関数から呼ばれる関数---------

	void PassDataToCB(Transform transform);	//コンスタントバッファに各種情報を渡す
	void SetBufferToPipeline();

	//------------------------------------------

protected:
	UINT64 vertexNum_;					//頂点数
	std::vector<VERTEX> vertices_;		//頂点情報
	
	UINT64 indexNum_;					//インデックス数
	std::vector<int> index_;			//インデックス情報

	ID3D11Buffer* pVertexBuffer_;		//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;		//インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;		//コンスタントバッファ

	Texture* pTexture_;					//テクスチャ

public:
	Quad();
	~Quad();
	HRESULT Initialize();				//各情報初期化、戻り値：失敗/成功
	void Draw(Transform& transform);	//描画
	void Release();						//解放
};

