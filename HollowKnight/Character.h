#pragma once
#include "Animation.h"
#include "CollisionBox.h"
#include "MyVector.h"
#include "StateMachine.h"

#include <graphics.h>
#include <string>
#include <unordered_map>

/*注意：图像坐标系是x轴向右为正方向,y轴向下为正方向*/
// 游戏角色基类
class Character
{
public:
	Character();
	~Character();

protected:
	struct AnimationGroup                                   // 角色的左右动画是成对存在的,所以封装在一个动画组中进行同一管理
	{
		Animation left;
		Animation right;
	};

	const float FLOOR_Y = 620;                              // 地板的竖直方向(游戏窗口是竖屏坐标系,Y越大越靠下,屏幕上Y=620这条水平线,就是角色能站立的地面)
	const float GRAVITY = 980 * 2;                          // 重力大小
	int hp = 10;											// 角色生命值
	MyVector position;										// 角色脚底位置
	MyVector velocity;										// 角色速度
	float logic_height = 0;									// 角色的逻辑高度(在现有实现中只会记录玩家脚底处的而为之作为角色位置,那对于碰撞箱居中逻辑的实现,还需要提供逻辑高度来计算得到,简单理解就是角色身高)
	bool is_facing_left = true;								// 当前角色是否朝向左
	StateMachine state_machine;								// 角色逻辑状态机
	bool enable_gravity = true;								// 启用重力模拟
	bool is_invulnerable = false;							// 当前是否无敌
	Timer timer_invulnerable_blink;							// 无敌闪烁状态定时器(控制切换显示效果)
	Timer timer_invulnerable_status;						// 无敌状态定时器(控制无敌状态的时长)
	bool is_blink_invisible = false;						// 当前是否处于闪烁的不可见帧
	std::shared_ptr<CollisionBox> hit_box = nullptr;		// 攻击碰撞箱
	std::shared_ptr<CollisionBox> hurt_box = nullptr;		// 受击碰撞箱
	std::shared_ptr<AnimationGroup> current_animation = nullptr;                     // 当前角色动画
	std::unordered_map<std::string, std::shared_ptr<AnimationGroup>> animation_pool;// 角色动画池 

	void decrease_hp();
	int get_hp() const;
	void set_position(const MyVector& position);
	const MyVector& get_position() const;
	void set_velocity(const MyVector& velocity);
	const MyVector& get_velocity() const;
	MyVector get_logic_center() const;
	void set_gravity_enabled(bool flag);
	std::shared_ptr<CollisionBox> get_hit_box();
	std::shared_ptr<CollisionBox> get_hurt_box();
	bool is_on_floor() const;
	float get_floor_y() const;
	void make_invulnerable();

	virtual void on_input(const ExMessage& msg);
	virtual void on_update(float delta);
	virtual void on_render();
	virtual void on_hurt();
	void switch_state(const std::string& id);
	void set_animation(const std::string& id);
};

