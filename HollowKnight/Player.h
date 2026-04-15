#pragma once
#include "Character.h"

// 四个攻击方向:会影响到不同的攻击特效播放以及碰撞箱位置
enum class AttackDirection {                         
	Up, Down, Left, Right
};

// 玩家类
class Player : public Character
{
public:
	Player();
	virtual ~Player();

	void on_input(const ExMessage& msg) override;        // 键鼠消息处理方法
	void on_update(float delta) override;
	void on_render() override;
	void on_hurt() override;

	void set_rolling(bool flag);
	bool get_rolling() const;
	bool can_roll() const;                               // 检查当前是否可以跳转到翻滚状态

	void set_attacking(bool flag);
	bool get_attacking() const;
	bool can_attack() const;

	bool can_jump() const;

	int get_move_axis() const;                          // 获取角色移动方向,-1表示向左,1表示向右,0表示没有移动
	AttackDirection get_attack_direction() const;       // 获取攻击方向

	void on_jump();                                     // 起跳逻辑
	void on_land();										// 落地逻辑
	void on_roll();                                     // 翻滚逻辑
	void on_attack();									// 攻击逻辑
private:
	const float CD_ROLL = 0.75f;
	const float CD_ATTACK = 0.5f;
	const float SPEED_RUN = 300.0f;
	const float SPEED_JUMP = 780.0f;
	const float SPEED_ROLL = 800.0f;

	Timer timer_roll_cd;                                 // 翻滚冷却时间
	bool is_rolling = false;                             // 是否处于翻滚状态
	bool is_roll_cd_comp = true;                         // 是否已经冷却结束

	Timer timer_attack_cd;                               // 攻击冷却时间
	bool is_attacking = false;                           // 是否处于攻击状态
	bool is_attack_cd_comp = true;                       // 是否已经冷却结束

	bool is_left_key_down = false;                       // 定义相应的按键状态。这样做的好处是:对应的按键消息出现时,便可只修改变量的值,其他部分的代码逻辑也只需要获取变量值,而不用关注当前是哪个物理按键被按下,这种思路通过封装就可以实现可配置的自定义按键功能
	bool is_right_key_down = false;
	bool is_jump_key_down = false;
	bool is_roll_key_down = false;
	bool is_attack_key_down = false;

	Animation animation_slash_up;                        // 斩击动画有四个方向,要定义四个动画对象
	Animation animation_slash_down;
	Animation animation_slash_left;
	Animation animation_slash_right;
	AttackDirection attack_direction = AttackDirection::Right;
	Animation* current_slash_animation = nullptr;

	// 角色在起跳和落地时会有烟尘特效,因此定义了两组特效需要的可见性标志和动画对象
	bool is_jump_vfx_visible = false;
	Animation animation_jump_vfx;

	bool is_land_vfx_visible = false;
	Animation animation_land_vfx;

	void update_attack_direction(int x, int y);          // 用来传入鼠标点击的坐标,计算转化为攻击的方向
};

