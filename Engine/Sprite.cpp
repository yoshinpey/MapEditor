#include "Sprite.h"

Sprite::Sprite()
    : pVertexBuffer_(nullptr), pIndexBuffer_(nullptr), pConstantBuffer_(nullptr), pTexture_(nullptr),
    vertexNum_(0), indexNum_(0)
{
}

Sprite::~Sprite()
{
}

//初期化
HRESULT Sprite::Initialize()
{
    HRESULT hr = S_OK;

    hr = CreateVertexBuffer();
    if (FAILED(hr))
    {
        MessageBox(NULL, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
        return hr;
    }

    hr = CreateIndexBuffer();
    if (FAILED(hr))
    {
        MessageBox(NULL, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
        return hr;
    }

    hr = CreateConstantBuffer();
    if (FAILED(hr))
    {
        MessageBox(NULL, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
        return hr;
    }

    hr = LoadTexture();
    if (FAILED(hr))
    {
        MessageBox(NULL, "テクスチャの読み込みに失敗しました", "エラー", MB_OK);
        return hr;
    }

    return hr;
}

//描画
void Sprite::Draw(Transform& transform)
{
    Direct3D::SetShader(SHADER_2D);
    transform.Calclation();
    PassDataToCB(transform);
    SetBufferToPipeline();
    Direct3D::pContext_->DrawIndexed(indexNum_, 0, 0);
}

//開放処理
void Sprite::Release()
{
    SAFE_DELETE(pTexture_);
    SAFE_RELEASE(pVertexBuffer_);
    SAFE_RELEASE(pIndexBuffer_);
    SAFE_RELEASE(pConstantBuffer_);
}


// 頂点情報準備
void Sprite::InitVertexData()
{
    // 頂点情報
    vertices_ =
    {
        {XMVectorSet(-1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)},  // 四角形の頂点（左上）
        {XMVectorSet(1.0f,  1.0f, 0.0f, 0.0f),  XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)},  // 四角形の頂点（右上）
        {XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f),  XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f)},  // 四角形の頂点（右下）
        {XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)}   // 四角形の頂点（左下）
    };
    // 頂点数
    vertexNum_ = vertices_.size();
}

// 頂点バッファ作成
HRESULT Sprite::CreateVertexBuffer()
{    
    // 頂点情報読み込み
    InitVertexData();
    // 頂点バッファの作成と初期化
    D3D11_BUFFER_DESC bd_vertex{};
    bd_vertex.ByteWidth = sizeof(VERTEX)* vertexNum_;
    bd_vertex.Usage = D3D11_USAGE_DEFAULT;
    bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd_vertex.CPUAccessFlags = 0;
    bd_vertex.MiscFlags = 0;
    bd_vertex.StructureByteStride = 0;

    // 頂点情報統合
    D3D11_SUBRESOURCE_DATA data_vertex{};
    data_vertex.pSysMem = vertices_.data();

    HRESULT hr;
    hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
    if (FAILED(hr))
    {
        MessageBox(NULL, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
        return hr;
    }
    return S_OK;
}

// インデックス情報を準備
void Sprite::InitIndexData()
{
    // インデックス情報
    index_ = 
    {
        0, 1, 2, 
        2, 3, 0 
    };
    indexNum_ = index_.size();
}

// インデックスバッファ作成
HRESULT Sprite::CreateIndexBuffer()
{
    // インデックス情報読み込み
    InitIndexData();
    // インデックスバッファの作成と初期化
    D3D11_BUFFER_DESC bd_index {};
    bd_index.Usage = D3D11_USAGE_DEFAULT;
    bd_index.ByteWidth = sizeof(int) * indexNum_;
    bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd_index.CPUAccessFlags = 0;
    bd_index.MiscFlags = 0;

    // インデックス情報統合
    D3D11_SUBRESOURCE_DATA data_index{};
    data_index.pSysMem = index_.data();
    data_index.SysMemPitch = 0;
    data_index.SysMemSlicePitch = 0;

    HRESULT hr;
    hr = Direct3D::pDevice_->CreateBuffer(&bd_index, &data_index, &pIndexBuffer_);
    if (FAILED(hr))
    {
        MessageBox(NULL, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
        return hr;
    }
    return S_OK;
}

// コンスタントバッファ作成
HRESULT Sprite::CreateConstantBuffer()
{
    // コンスタントバッファの作成
    D3D11_BUFFER_DESC bd_constant{};
    bd_constant.ByteWidth = sizeof(CONSTANT_BUFFER);
    bd_constant.Usage = D3D11_USAGE_DYNAMIC;
    bd_constant.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd_constant.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd_constant.MiscFlags = 0;
    bd_constant.StructureByteStride = 0;

    HRESULT hr;
    hr = Direct3D::pDevice_->CreateBuffer(&bd_constant, nullptr, &pConstantBuffer_);
    if (FAILED(hr))
    {
        MessageBox(NULL, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
        return hr;
    }
    return S_OK;
}

//テクスチャをロード
HRESULT Sprite::LoadTexture()
{
    HRESULT hr;
    // テクスチャの読み込み
    pTexture_ = new Texture;
    hr = pTexture_->Load("Assets\\Dice.png");
    if (FAILED(hr))
    {
        MessageBox(NULL, "テクスチャ読み込みに失敗しました", "エラー", MB_OK);
        return hr;
    }
    return S_OK;
}

//コンスタントバッファに各情報を渡す
void Sprite::PassDataToCB(Transform transform)
{
    // コンスタントバッファに渡す情報
    CONSTANT_BUFFER cb{};
    cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());

    // コンスタントバッファの更新
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy_s(mappedResource.pData, mappedResource.RowPitch, (void*)(&cb), sizeof(cb));

    ID3D11SamplerState* pSampler = pTexture_->GetSampler();
    Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

    ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
    Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

    Direct3D::pContext_->Unmap(pConstantBuffer_, 0);
}

//各バッファをパイプラインにセット
void Sprite::SetBufferToPipeline()
{
    // 頂点バッファの設定
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

    // インデックスバッファの設定
    stride = sizeof(int);
    offset = 0;
    Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

    // コンスタントバッファの設定
    Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
    Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);
}


