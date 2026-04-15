#pragma once
#include "Animation.h"
#include "CollisionBox.h"
#include "config.h"

// 敌人技能Sword道具类
class Sword
{
private:
	MyVector position;
	MyVector velocity;
	Animation animation;
	bool is_valid = true;                                   // 有效性用于外部检查对象是否可被移除
	std::shared_ptr<CollisionBox> collision_box = nullptr;

public:
	Sword(const MyVector& position, bool move_left);
	~Sword();

	void on_update(float delta);
	void on_render();
	bool check_valid() const;
};

