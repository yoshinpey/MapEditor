#include "Model.h"
#include "Direct3D.h"

namespace Model {

	//モデルのポインタをいれておくベクタ
	std::vector<ModelData*> modelList;
}

int Model::Load(std::string fileName)
{
	ModelData* pData;
	pData = new ModelData;
	pData->filename_ = fileName;
	pData->pfbx_ = nullptr;

	//ファイルネームが同じだったら読まない
	for (auto& e : modelList)
	{
		if (e->filename_ == fileName) {
			pData->pfbx_ = e->pfbx_;
			break;
		}
	}

	if (pData->pfbx_ == nullptr)
	{
		pData->pfbx_ = new Fbx;
		pData->pfbx_->Load(fileName);
	}

	modelList.push_back(pData);
	return(modelList.size() - 1);
}

void Model::SetTransform(int hModel, Transform transform)
{
	modelList[hModel]->transform_ = transform;
	//モデル番号は、modelListのインデックス
}
void Model::Draw(int hModel) {
	//モデル番号は、modelListのインデックス
	modelList[hModel]->pfbx_->Draw(modelList[hModel]->transform_);
}

void Model::Release()
{
	bool isReffered = false; //参照されてる？
	for (int i = 0; i < modelList.size(); i++)
	{
		for (int j = i + 1; j < modelList.size(); j++)
		{
			if (modelList[i]->pfbx_ == modelList[j]->pfbx_)
			{
				isReffered = true;
				break;
			}
		}
		if (isReffered == false)
		{
			SAFE_DELETE(modelList[i]->pfbx_);
		}
		SAFE_DELETE(modelList[i]);
	}
	modelList.clear();
}

void Model::RayCast(int hModel_, RayCastData& rayData)
{
	//モデルのトランスフォームをカリキュレーション
	modelList[hModel_]->transform_.Calclation();
	//ワールド行列の逆行列
	XMMATRIX wInv = XMMatrixInverse(nullptr, modelList[hModel_]->transform_.GetWorldMatrix());
	//レイの通過点を求める(モデル空間でのレイの方向ベクトルを求める)
	XMVECTOR vpass{
		rayData.start.x + rayData.dir.x,
		rayData.start.y + rayData.dir.y,
		rayData.start.z + rayData.dir.z,
		rayData.start.w + rayData.dir.w,
	};
	//rayData.Startをモデル空間に変換
	XMVECTOR vstart = XMLoadFloat4(&rayData.start);
	vstart = XMVector3TransformCoord(vstart, wInv);
	XMStoreFloat4(&rayData.start, vstart);

	//始点から方向ベクトルを伸ばした先(通過点)
	vpass = XMVector3TransformCoord(vpass, wInv);

	//ベクトルにする
	vpass = vpass - vstart;
	XMStoreFloat4(&rayData.dir, vpass);

	//指定したモデル番号FBXにレイキャスト
	modelList[hModel_]->pfbx_->RayCast(&rayData);
}
