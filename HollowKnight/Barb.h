#pragma once
#include "Animation.h"
#include "CollisionBox.h"

#include "config.h"

// 敌人技能Barb道具类
class Barb
{
public:
	Barb();
	~Barb();

	void on_update(float delta);
	void on_render();
	bool check_valid() const;
	void set_position(const MyVector& position);
private:
	Timer timer_idle;                                       // 控制闲置状态的持续时间
	Timer timer_aim;                                        // 控制瞄准状态的持续时间
	int diff_period = 0;                                    // 保存一个随机数,控制浮动的运动周期偏移
	bool is_valid = true;                                   // 有效性用于外部检查对象是否可被移除
	float total_delta_time = 0;                             // 记录刺球生成以来度过的时间,对其取三角函数来实现周期运动

	MyVector velocity;
	MyVector base_position;                                 // 刺球在浮动过程和瞄准的震动过程中,都会在原始位置上进行一定的范围偏移,该变量记录它编译前的原点位置,即基准点
	MyVector current_position;                              // 当前帧的运动位置

	Animation animation_loose;
	Animation animation_break;
	Animation* current_animation = nullptr;

	Stage stage = Stage::Idle;
	std::shared_ptr<CollisionBox> collision_box = nullptr;

	void on_break();
};

