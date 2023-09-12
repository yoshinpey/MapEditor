#include "Model.h"
#include "Direct3D.h"

namespace Model {

	//���f���̃|�C���^������Ă����x�N�^
	std::vector<ModelData*> modelList;
}

int Model::Load(std::string fileName)
{
	ModelData* pData;
	pData = new ModelData;
	pData->filename_ = fileName;
	pData->pfbx_ = nullptr;

	//�t�@�C���l�[����������������ǂ܂Ȃ�
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
	//���f���ԍ��́AmodelList�̃C���f�b�N�X
}
void Model::Draw(int hModel) {
	//���f���ԍ��́AmodelList�̃C���f�b�N�X
	modelList[hModel]->pfbx_->Draw(modelList[hModel]->transform_);
}

void Model::Release()
{
	bool isReffered = false; //�Q�Ƃ���Ă�H
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
	//���f���̃g�����X�t�H�[�����J���L�����[�V����
	modelList[hModel_]->transform_.Calclation();
	//���[���h�s��̋t�s��
	XMMATRIX wInv = XMMatrixInverse(nullptr, modelList[hModel_]->transform_.GetWorldMatrix());
	//���C�̒ʉߓ_�����߂�(���f����Ԃł̃��C�̕����x�N�g�������߂�)
	XMVECTOR vpass{
		rayData.start.x + rayData.dir.x,
		rayData.start.y + rayData.dir.y,
		rayData.start.z + rayData.dir.z,
		rayData.start.w + rayData.dir.w,
	};
	//rayData.Start�����f����Ԃɕϊ�
	XMVECTOR vstart = XMLoadFloat4(&rayData.start);
	vstart = XMVector3TransformCoord(vstart, wInv);
	XMStoreFloat4(&rayData.start, vstart);

	//�n�_��������x�N�g����L�΂�����(�ʉߓ_)
	vpass = XMVector3TransformCoord(vpass, wInv);

	//�x�N�g���ɂ���
	vpass = vpass - vstart;
	XMStoreFloat4(&rayData.dir, vpass);

	//�w�肵�����f���ԍ�FBX�Ƀ��C�L���X�g
	modelList[hModel_]->pfbx_->RayCast(&rayData);
}
