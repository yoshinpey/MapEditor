#include "Quad.h"

Quad::Quad()
    : pVertexBuffer_(nullptr), pIndexBuffer_(nullptr), pConstantBuffer_(nullptr), pTexture_(nullptr), indexNum_(0), vertexNum_(0)
{
}

Quad::~Quad()
{
    Release();
}

HRESULT Quad::Initialize()
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


void Quad::Draw(Transform& transform)
{
    Direct3D::SetShader(SHADER_3D);
    transform.Calclation();                     //トランスフォームを計算
    PassDataToCB(transform);	//コンスタントバッファに情報を渡す
    SetBufferToPipeline();                      //頂点バッファ、インデックスバッファ、コンスタントバッファをパイプラインにセット

    //描画
    Direct3D::pContext_->DrawIndexed(indexNum_, 0, 0);
}

//開放処理
void Quad::Release()
{
    SAFE_DELETE(pTexture_);
    SAFE_RELEASE(pVertexBuffer_);
    SAFE_RELEASE(pIndexBuffer_);
    SAFE_RELEASE(pConstantBuffer_);
}

void Quad::InitVertexData()
{
    // 頂点情報
    vertices_ =
    {
        {XMVectorSet(-1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f,0.0f,-1.0, 0.0f)},  // 四角形の頂点（左上）
        {XMVectorSet(1.0f,  1.0f, 0.0f, 0.0f),  XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f,0.0f,-1.0, 0.0f)},  // 四角形の頂点（右上）
        {XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f),  XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(0.0f,0.0f,-1.0, 0.0f)},  // 四角形の頂点（右下）
        {XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(0.0f,0.0f,-1.0, 0.0f)}   // 四角形の頂点（左下）
    };
    // 頂点数
    vertexNum_ = vertices_.size();
}

// 頂点バッファの作成
HRESULT Quad::CreateVertexBuffer()
{
    InitVertexData();
    // 頂点バッファの作成と初期化
    D3D11_BUFFER_DESC bd_vertex{};
    bd_vertex.ByteWidth = sizeof(VERTEX) * vertexNum_;
    bd_vertex.Usage = D3D11_USAGE_IMMUTABLE;
    bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd_vertex.CPUAccessFlags = 0;
    bd_vertex.MiscFlags = 0;
    bd_vertex.StructureByteStride = 0;

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

//インデックス情報を準備
void Quad::InitIndexData()
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
HRESULT Quad::CreateIndexBuffer()
{
    // インデックス情報読み込み
    InitIndexData();
    // インデックスバッファの作成と初期化
    D3D11_BUFFER_DESC bd_index{};
    bd_index.Usage = D3D11_USAGE_IMMUTABLE;
    bd_index.ByteWidth = sizeof(UINT) * indexNum_;
    bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd_index.CPUAccessFlags = 0;
    bd_index.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data_index{};
    data_index.pSysMem = index_.data();
    data_index.SysMemPitch = 0;
    data_index.SysMemSlicePitch = 0;

    HRESULT hr;
    hr= Direct3D::pDevice_->CreateBuffer(&bd_index, &data_index, &pIndexBuffer_);
    if (FAILED(hr))
    {
        MessageBox(NULL, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
        return hr;
    }
    return S_OK;
}

// コンスタントバッファの作成
HRESULT Quad::CreateConstantBuffer()
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
HRESULT Quad::LoadTexture()
{
    // テクスチャの読み込み
    HRESULT hr;
    pTexture_ = new Texture;
    hr = pTexture_->Load("Assets\\Dice.png");
    if (FAILED(hr))
    {
        MessageBox(NULL, "テクスチャの読み込みに失敗しました", "エラー", MB_OK);
        return hr;
    }
    return S_OK;
}

//コンスタントバッファに各種情報を渡す
void Quad::PassDataToCB(Transform transform)
{
    // コンスタントバッファに渡す情報
    CONSTANT_BUFFER cb{};
    cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
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
void Quad::SetBufferToPipeline()
{
    using Direct3D::pContext_;

    // 頂点バッファの設定
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

    // インデックスバッファの設定
    stride = sizeof(int);
    offset = 0;
    pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

    // コンスタントバッファの設定
    pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
    pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);

}
