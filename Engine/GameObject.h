#pragma once
#include <list>
#include <string>
#include "Transform.h"
#include "Direct3D.h"

class SphereCollider;

class GameObject
{
protected:
	Transform transform_;				//位置や向きなどを管理するオブジェクト
	std::string	objectName_;			//オブジェクトの名前
	GameObject* pParent_;				//親オブジェクト
	std::list<GameObject*> childList_;	//子オブジェクトリスト
	bool isDead_;						//オブジェクト削除用

	SphereCollider* pCollider_;

public:
	//コンストラクタ
	GameObject();
	GameObject(GameObject* parent, const std::string& name);
	GameObject* FindChildObject(std::string _objName);
	GameObject* GetRootJob();
	GameObject* FindObject(std::string _objName);

	void AddCollider(SphereCollider* pCollider);
	void Collision(GameObject* pTarget);
	void RoundRobin(GameObject* pTarget);

	//デストラクタ
	~GameObject();

	//各オブジェクトで必ず作る関数
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Release() = 0;

	//関数
	void DrawSub();
	void UpdateSub();
	void ReleaseSub();
	bool KillMe();

	void SetPosition(XMFLOAT3 position);
	void SetPosition(float x, float y, float z);

	template <class T>
	GameObject* Instantiate(GameObject* parent)
	{
		T* p;
		p = new T(parent);
		p->Initialize();
		childList_.push_back(p);
		return p;
	}
};