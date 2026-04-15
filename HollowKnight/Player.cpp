#include "BulletTimeMgr.h"
#include "Player.h"
#include "PlayerStateNode.h"
#include "ResourcesMgr.h"
#include <cmath>

Player::Player()
{
	is_facing_left = false;                                               // 游戏开始时角色面朝右方
	position = {250, 200};                                                // 设置角色的初始位置
	logic_height = 150;                                                   // 设置角色的逻辑高度

	// 初始化攻击碰撞箱和受击碰撞箱
	hit_box->set_size({ 150, 150 });
	hurt_box->set_size({ 40, 80 });

	hit_box->set_layer_src(CollisionLayer::None);
	hit_box->set_layer_dst(CollisionLayer::Enemy);

	hurt_box->set_layer_src(CollisionLayer::Player);
	hurt_box->set_layer_dst(CollisionLayer::None);

	hit_box->set_enabled(false);                                          // 默认状态下,攻击碰撞箱是不会开启碰撞检测的,只有攻击状态下才会启用
	hurt_box->set_on_collide([&]() {                                      // 受击碰撞箱的回调逻辑是扣血
		decrease_hp();
	});

	// 初始化翻滚冷却定时器和攻击冷却定时器
	timer_roll_cd.set_wait_time(CD_ROLL);
	timer_roll_cd.set_one_shot(true);
	timer_roll_cd.set_on_timeout([&]() {
		is_roll_cd_comp = true;
	});

	timer_attack_cd.set_wait_time(CD_ATTACK);
	timer_attack_cd.set_one_shot(true);
	timer_attack_cd.set_on_timeout([&]() {
		is_attack_cd_comp = true;
	});

	 /*
		动画对象的初始化
		注意:1、不同动画的帧数量是不同的 
			2、animation_attack、animation_attack_left等变量必须是引用,否则对他们的设置和修改只是对局部变量操作,而局部变量在离开作用于后就回收了,
			   无法应用到animation_pool内,后续对animation_pool进行索引访问时会出现内存问题(踩过坑)
	 */
	// 角色攻击动画
	AnimationGroup& animation_attack = animation_pool["attack"];
	Animation& animation_attack_left = animation_attack.left;
	animation_attack_left.set_interval(0.05f);                                                         // 设置帧间隔
	animation_attack_left.set_loop(false);                                                             // 不循环播放
	animation_attack_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_attack_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_attack_left"), 5); // 将资源管理器的图片添加为动画对象的帧

	Animation& animation_attack_right = animation_attack.right;
	animation_attack_right.set_interval(0.05f);
	animation_attack_right.set_loop(false);
	animation_attack_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_attack_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_attack_right"), 5);

	// 角色死亡动画
	AnimationGroup& animation_dead = animation_pool["dead"];
	Animation& animation_dead_left = animation_dead.left;
	animation_dead_left.set_interval(0.1f);
	animation_dead_left.set_loop(false);
	animation_dead_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_dead_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_dead_left"), 6);

	Animation& animation_dead_right = animation_dead.right;
	animation_dead_right.set_interval(0.1f);
	animation_dead_right.set_loop(false);
	animation_dead_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_dead_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_dead_right"), 6);

	// 角色落下动画
	AnimationGroup& animation_fall = animation_pool["fall"];
	Animation& animation_fall_left = animation_fall.left;
	animation_fall_left.set_interval(0.15f);
	animation_fall_left.set_loop(true);
	animation_fall_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_fall_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_fall_left"), 5);

	Animation& animation_fall_right = animation_fall.right;
	animation_fall_right.set_interval(0.15f);
	animation_fall_right.set_loop(true);
	animation_fall_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_fall_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_fall_right"), 5);

	// 角色空闲动画
	AnimationGroup& animation_idle = animation_pool["idle"];
	Animation& animation_idle_left = animation_idle.left;
	animation_idle_left.set_interval(0.15f);
	animation_idle_left.set_loop(true);
	animation_idle_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_idle_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_idle_left"), 5);

	Animation& animation_idle_right = animation_idle.right;
	animation_idle_right.set_interval(0.15f);
	animation_idle_right.set_loop(true);
	animation_idle_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_idle_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_idle_right"), 5);

	// 角色跳跃动画
	AnimationGroup& animation_jump = animation_pool["jump"];
	Animation& animation_jump_left = animation_jump.left;
	animation_jump_left.set_interval(0.15f);
	animation_jump_left.set_loop(false);
	animation_jump_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_jump_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_jump_left"), 5);

	Animation& animation_jump_right = animation_jump.right;
	animation_jump_right.set_interval(0.15f);
	animation_jump_right.set_loop(false);
	animation_jump_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_jump_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_jump_right"), 5);

	// 角色翻滚动画
	AnimationGroup& animation_roll = animation_pool["roll"];
	Animation& animation_roll_left = animation_roll.left;
	animation_roll_left.set_interval(0.05f);
	animation_roll_left.set_loop(false);
	animation_roll_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_roll_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_roll_left"), 7);

	Animation& animation_roll_right = animation_roll.right;
	animation_roll_right.set_interval(0.05f);
	animation_roll_right.set_loop(false);
	animation_roll_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_roll_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_roll_right"), 7);

	// 角色奔跑动画
	AnimationGroup& animation_run = animation_pool["run"];
	Animation& animation_run_left = animation_run.left;
	animation_run_left.set_interval(0.075f);
	animation_run_left.set_loop(true);
	animation_run_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_run_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_run_left"), 10);

	Animation& animation_run_right = animation_run.right;
	animation_run_right.set_interval(0.075f);
	animation_run_right.set_loop(true);
	animation_run_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_run_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_run_right"), 10);

	// 初始化玩家角色特有的特效动画对象
	animation_slash_up.set_interval(0.07f);
	animation_slash_up.set_loop(false);
	animation_slash_up.set_anchor_mode(AnchorMode::Centered);
	animation_slash_up.add_frame(ResourcesMgr::GetInstance()->find_image("player_vfx_attack_up"), 5);

	animation_slash_down.set_interval(0.07f);
	animation_slash_down.set_loop(false);
	animation_slash_down.set_anchor_mode(AnchorMode::Centered);
	animation_slash_down.add_frame(ResourcesMgr::GetInstance()->find_image("player_vfx_attack_down"), 5);

	animation_slash_left.set_interval(0.07f);
	animation_slash_left.set_loop(false);
	animation_slash_left.set_anchor_mode(AnchorMode::Centered);
	animation_slash_left.add_frame(ResourcesMgr::GetInstance()->find_image("player_vfx_attack_left"), 5);

	animation_slash_right.set_interval(0.07f);
	animation_slash_right.set_loop(false);
	animation_slash_right.set_anchor_mode(AnchorMode::Centered);
	animation_slash_right.add_frame(ResourcesMgr::GetInstance()->find_image("player_vfx_attack_right"), 5);

	animation_jump_vfx.set_interval(0.05f);
	animation_jump_vfx.set_loop(false);
	animation_jump_vfx.set_anchor_mode(AnchorMode::BottomCentered);
	animation_jump_vfx.add_frame(ResourcesMgr::GetInstance()->find_image("player_vfx_jump"), 5);
	animation_jump_vfx.set_on_finished([&]() {is_jump_vfx_visible = false; });                             // 起跳和落地的烟尘特效动画的播放结束回调逻辑都是将对应的可见性标志设为false

	animation_land_vfx.set_interval(0.1f);
	animation_land_vfx.set_loop(false);
	animation_land_vfx.set_anchor_mode(AnchorMode::BottomCentered);
	animation_land_vfx.add_frame(ResourcesMgr::GetInstance()->find_image("player_vfx_land"), 2);
	animation_land_vfx.set_on_finished([&]() {is_land_vfx_visible = false; });

	// 状态机初始化,注册不同的状态节点实例,并设置状态机的入口为闲置状态
	state_machine.register_state("attack", std::make_shared<PlayerAttackState>());
	state_machine.register_state("dead", std::make_shared<PlayerDeadState>());
	state_machine.register_state("fall", std::make_shared<PlayerFallState>());
	state_machine.register_state("idle", std::make_shared<PlayerIdleState>());
	state_machine.register_state("jump", std::make_shared<PlayerJumpState>());
	state_machine.register_state("roll", std::make_shared<PlayerRollState>());
	state_machine.register_state("run", std::make_shared<PlayerRunState>());

	state_machine.set_entry("idle");
}

Player::~Player()
{
}

void Player::on_input(const ExMessage& msg)
{
	if (hp <= 0)                                // 如果角色死亡则不再支持任何操作
		return;
	switch (msg.message)                        // message 返回当前发生的输入事件类型,它是一个整数,用宏来表示,表示有事件发生(鼠标？键盘？按下？松开？)
	{
	case WM_KEYDOWN:                            // 键盘按下
		switch (msg.vkcode)                     // vkcode返回虚拟键码,返回按下的具体是哪个键
		{
		case 0x41:                              // 虚拟键码,A = 0x41一直递增到Z = 0x5A
		case VK_LEFT:                           // 左方向键
			is_left_key_down = true;            // 设为true, 表示'左方向键按住了'
			break;
		case 0x44:								// 'D'
		case VK_RIGHT:                          // 右方向键
			is_right_key_down = true;
			break;
		case 0x57:								// 'W'
		case VK_UP:                             // 上方向键
		case VK_SPACE:                          // 空格键
			is_jump_key_down = true;
			break;
		case 0x53:								// 's'
		case VK_DOWN:                           // 下方向键
			is_roll_key_down = true;
			break;
		}
		break;
	case WM_KEYUP:                              // 键盘松开
		switch (msg.vkcode)
		{
		case 0x41:                              // 'A'
		case VK_LEFT:
			is_left_key_down = false;           // 设为false,表示按键已松开
			break;
		case 0x44:								// 'D'
		case VK_RIGHT:
			is_right_key_down = false;
			break;
		case 0x57:								// 'W'
		case VK_UP:
		case VK_SPACE:
			is_jump_key_down = false;
			break;
		case 0x53:								// 'S'
		case VK_DOWN:
			is_roll_key_down = false;
			break;
		}
		break;
	case WM_LBUTTONDOWN:                        // 鼠标左键按下
		is_attack_key_down = true;
		update_attack_direction(msg.x, msg.y);
		break;
	case WM_LBUTTONUP:
		is_attack_key_down = false;
		break;
	case WM_RBUTTONDOWN:                        // 鼠标右键按下
		play_audio(_T("bullet_time"), false);
		BulletTimeMgr::GetInstance()->set_status(Status::Entering);
		break;
	case WM_RBUTTONUP:
		stop_audio(_T("bullet_time"));
		BulletTimeMgr::GetInstance()->set_status(Status::Exiting);
		break;
	default:
		break;
	}
}

void Player::on_update(float delta)
{
	if (hp > 0 && !is_rolling)
		velocity.x = get_move_axis() * SPEED_RUN;                 // 存活且不处于翻滚状态时,移动速度就是标准的计算公式
	if (get_move_axis() != 0)
		is_facing_left = (get_move_axis() < 0);

	// 更新计时器和特效动画
	timer_roll_cd.on_update(delta);
	timer_attack_cd.on_update(delta);

	animation_jump_vfx.on_update(delta);
	animation_land_vfx.on_update(delta);

	if (is_attacking) {                                           // 当处于攻击状态时需要设置攻击特效动画的位置始终跟随玩家角色
		current_slash_animation->set_position(get_logic_center());
		current_slash_animation->on_update(delta);
	}
	Character::on_update(delta);
}

void Player::on_render()
{
	// 依次渲染起跳落地特效,玩家角色动画和攻击特效动画
	if (is_jump_vfx_visible) {
		animation_jump_vfx.on_render();
	}
	if (is_land_vfx_visible) {
		animation_land_vfx.on_render();
	}

	Character::on_render();
	if (is_attacking) {
		current_slash_animation->on_render();
	}
}

void Player::on_hurt()
{
	play_audio(_T("player_hurt"), false);                          // 直接调用封装好的音频播放函数
}

void Player::set_rolling(bool flag)
{
	is_rolling = flag;
}

bool Player::get_rolling() const
{
	return is_rolling;
}

bool Player::can_roll() const
{
	return is_roll_cd_comp && !is_rolling && is_roll_key_down;
}

void Player::set_attacking(bool flag)
{
	is_attacking = flag;
}

bool Player::get_attacking() const
{
	return is_attacking;
}

bool Player::can_attack() const
{
	return is_attack_cd_comp && !is_attacking && is_attack_key_down;
}

bool Player::can_jump() const
{
	return is_on_floor() && is_jump_key_down;
}

int Player::get_move_axis() const
{
	return is_right_key_down - is_left_key_down;
}

AttackDirection Player::get_attack_direction() const
{
	return attack_direction;
}

void Player::on_jump()
{
	// 设置特效的可见性和位置,并重置特效动画让它从头开始播放
	velocity.y -= SPEED_JUMP;                                             // 起跳时给角色一个竖直向上的速度
	is_jump_vfx_visible = true;                                      
	animation_jump_vfx.set_position(position);
	animation_jump_vfx.reset();
}

void Player::on_land()
{
	is_land_vfx_visible = true;
	animation_land_vfx.set_position(position);
	animation_land_vfx.reset();
}

void Player::on_roll()
{
	// 重置冷却随时间定时器和翻滚状态,并根据当前角色的朝向设置其水平方向的移动速度
	timer_roll_cd.restart();
	is_roll_cd_comp = false;
	velocity.x = is_facing_left ? -SPEED_ROLL : SPEED_ROLL;
}

void Player::on_attack()
{
	timer_attack_cd.restart();                                                          // 先重置冷却
	is_attack_cd_comp = false; 
	switch (attack_direction)                                                           // 再根据朝向选择不同方向的特效动画
	{
	case AttackDirection::Up:
		current_slash_animation = &animation_slash_up;
		break;
	case AttackDirection::Down:
		current_slash_animation = &animation_slash_down;
		break;
	case AttackDirection::Left:
		current_slash_animation = &animation_slash_left;
		break;
	case AttackDirection::Right:
		current_slash_animation = &animation_slash_right;
		break;
	}
	current_slash_animation->set_position(get_logic_center());                          // 设置攻击动画的初始位置在角色的逻辑中心处
	current_slash_animation->reset();                                                   // 重置动画状态
}

void Player::update_attack_direction(int x, int y)
{
	static const float PI = 3.141592654f;
	float angle = std::atan2(y - position.y, x - position.x);							// [-PI，PI],求鼠标相对于角色的方向角
	if (angle >= -PI / 4 && angle < PI / 4)
		attack_direction = AttackDirection::Right;
	else if (angle >= PI / 4 && angle < 3 * PI / 4)                                     // 注意游戏使用的坐标系y轴正方向是向下的,因此atan2算出来正角度 = 向下,负角度 = 向上
		attack_direction = AttackDirection::Down;
	else if ((angle >= 3 * PI / 4 && angle <= PI) || (angle >= -PI && angle < -3 * PI / 4))
		attack_direction = AttackDirection::Left;
	else
		attack_direction = AttackDirection::Up;
}
