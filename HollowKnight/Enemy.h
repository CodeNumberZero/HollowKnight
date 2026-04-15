#pragma once
#include "Character.h"
#include "Barb.h"
#include "Sword.h"
#include "config.h"

// 敌人类
class Enemy : public Character
{
private:
	bool is_throwing_silk = false;
	bool is_dashing_in_air = false;
	bool is_dashing_on_floor = false;

	Animation animation_silk;
	AnimationGroup animation_dash_in_air_vfx;
	AnimationGroup animation_dash_on_floor_vfx;
	Animation* current_dash_animation = nullptr;				// 当前冲刺特效动画对象的指针

	std::vector<std::shared_ptr<Barb>> barb_list;				// 存储场景中的刺球对象
	std::vector<std::shared_ptr<Sword>> sword_list;				// 存储场景中的剑对象
	std::shared_ptr<CollisionBox> collision_box_silk = nullptr;

public:
	Enemy();
	~Enemy();

	void on_update(float delta) override;
	void on_render() override;
	void on_hurt() override;

	void set_facing_left(bool flag);
	bool get_facing_left() const;
	void set_dashing_in_air(bool flag);
	bool get_dashing_in_air() const;
	void set_dashing_on_floor(bool flag);
	bool get_dashing_on_floor() const;
	void set_throwing_silk(bool flag);
	bool get_throwing_silk() const;

	void throw_barbs();											// 召唤刺球
	void throw_sword();											// 扔剑

	void on_dash();
	void on_throw_silk();
};

