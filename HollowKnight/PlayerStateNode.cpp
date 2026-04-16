#include "PlayerStateNode.h"
#include "Player.h"
#include "CharacterMgr.h"

// 更新攻击碰撞箱的位置
void PlayerAttackState::update_hit_box_position()
{
	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	MyVector pos_center = player->get_logic_center();
	auto hit_box = player->get_hit_box();
	const MyVector& size_hit_box = hit_box->get_size();
	MyVector pos_hit_box;
	switch (player->get_attack_direction())
	{
	case AttackDirection::Up:
		pos_hit_box = { pos_center.x, pos_center.y - size_hit_box.y / 2 };   // 水平不变,竖直向上移动半个攻击盒高度,攻击盒贴在角色头顶上方
		break;
	case AttackDirection::Down:
		pos_hit_box = { pos_center.x, pos_center.y + size_hit_box.y / 2 };
		break;
	case AttackDirection::Left:
		pos_hit_box = { pos_center.x - size_hit_box.x / 2, pos_center.y };
		break;
	case AttackDirection::Right:
		pos_hit_box = { pos_center.x + size_hit_box.x / 2, pos_center.y };
		break;
	}
	hit_box->set_position(pos_hit_box);
}

PlayerAttackState::PlayerAttackState()
{
	timer.set_wait_time(0.3f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&]() {
		/* 
			1、父类指针不能隐式转成子类指针,必须显式转换,使用智能指针专用转换函数将基类指针转换为派生类指针;
			2、这里能确定返回的一定是Player类型的指针,所以可以用static_pointer_cast;如果不确定返回类型就需要使用dynamic_pointer_cast进行安全判断 
		*/
		std::shared_ptr<Player> player = std::static_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
		player->set_attacking(false);
	});
}

// 进入攻击状态节点的初始化逻辑
void PlayerAttackState::on_enter()
{
	CharacterMgr::GetInstance()->get_player()->set_animation("attack");         // 玩家进入攻击状态时,先设置当前动画为attack

	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	player->get_hit_box()->set_enabled(true);                                   // 设置玩家的相应状态
	player->set_attacking(true);
	update_hit_box_position();
	player->on_attack();
	timer.restart();

	switch (range_random(1, 3))
	{
	case 1:
		play_audio(_T("player_attack_1"), false);
		break;
	case 2:
		play_audio(_T("player_attack_2"), false);
		break;
	case 3:
		play_audio(_T("player_attack_3"), false);
		break;
	}
}

// 攻击状态更新的逻辑
void PlayerAttackState::on_update(float delta)
{
	timer.on_update(delta);
	update_hit_box_position();

	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());

	// 除去上述更新定时器和攻击碰撞箱的逻辑,还需要处理状态跳出的逻辑
	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (!player->get_attacking()) {
		if (player->get_velocity().y > 0)
			player->switch_state("fall");
		else if (player->get_move_axis() == 0)
			player->switch_state("idle");
		else if (player->is_on_floor() && player->get_move_axis() != 0)
			player->switch_state("run");
	}
}

// 退出攻击状态的逻辑
void PlayerAttackState::on_exit()
{
	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	player->get_hit_box()->set_enabled(false);
	player->set_attacking(false);
}

PlayerDeadState::PlayerDeadState()
{
	// 玩家死亡时是有死亡动画的,所以进入死亡状态后要隔一段时间再结束游戏
	timer.set_wait_time(2.0f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&]() {
		MessageBox(GetHWnd(), _T("不对...\n这样不行。"), _T("挑战失败！"), MB_OK);	// MessageBox是WindowsAPI函数, 原生弹窗函数, 显示对话框;GetHWnd()获取父窗口句柄,弹窗会居中在游戏窗口上;MB_OK表示显示一个"确定"按钮
		exit(0);																// 0 = 正常退出;点击确定后游戏直接关闭，结束运行
		});
}

void PlayerDeadState::on_enter()
{
	CharacterMgr::GetInstance()->get_player()->set_animation("dead");
	play_audio(_T("player_dead"), false);
}

void PlayerDeadState::on_update(float delta)
{
	timer.on_update(delta);
}

void PlayerFallState::on_enter()
{
	CharacterMgr::GetInstance()->get_player()->set_animation("fall");
}

void PlayerFallState::on_update(float delta)
{
	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->is_on_floor()) {
		player->switch_state("idle");
		player->on_land();

		play_audio(_T("player_land"), false);
	}
	else if (player->can_attack()) {
		player->switch_state("attack");
	}
}

void PlayerIdleState::on_enter()
{
	CharacterMgr::GetInstance()->get_player()->set_animation("idle");
}

// 闲置状态为角色的默认状态,如果不是强关联状态可以用闲置状态进行过渡,因此闲置状态的出口有很多
void PlayerIdleState::on_update(float delta)
{
	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	// 根据优先级依次检查与其他状态进行连接
	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->can_attack()) {
		player->switch_state("attack");
	}
	else if (player->get_velocity().y > 0) {
		player->switch_state("fall");
	}
	else if (player->can_jump()) {
		player->switch_state("jump");
	}
	else if (player->can_roll()) {
		player->switch_state("roll");
	}
	else if (player->is_on_floor() && player->get_move_axis() != 0) {
		player->switch_state("run");
	}
}

void PlayerJumpState::on_enter()
{
	CharacterMgr::GetInstance()->get_player()->set_animation("jump");

	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	player->on_jump();

	play_audio(_T("player_jump"), false);
}

void PlayerJumpState::on_update(float delta)
{
	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());

	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->get_velocity().y > 0) {
		player->switch_state("fall");
	}
	else if (player->can_attack()) {
		player->switch_state("attack");
	}
}

// 翻滚与攻击类似,都是通过定时器来控制退出时机
PlayerRollState::PlayerRollState()
{
	timer.set_wait_time(0.35f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&]() {
		std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
		player->set_rolling(false);
	});
}

void PlayerRollState::on_enter()
{
	CharacterMgr::GetInstance()->get_player()->set_animation("roll");

	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	player->get_hurt_box()->set_enabled(false);                               // 由于翻滚期间是无敌的,所以进入翻滚状态先禁用受击碰撞箱,退出时再启用
	player->set_rolling(true);
	player->on_roll();
	timer.restart();

	play_audio(_T("player_roll"), false);

}

void PlayerRollState::on_update(float delta)
{
	timer.on_update(delta);

	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	// 由于翻滚的无敌效果,翻滚状态是唯一不需要跳转到死亡状态的节点;只需要在结束时根据玩家输入操作决定是否跑跳或闲置即可
	if (!player->get_rolling()) {
		if (player->get_move_axis() != 0) {
			player->switch_state("run");
		}
		else if (player->can_jump()) {
			player->switch_state("jump");
		}
		else {
			player->switch_state("idle");
		}
	}
}

void PlayerRollState::on_exit()
{
	CharacterMgr::GetInstance()->get_player()->get_hurt_box()->set_enabled(true);
}

void PlayerRunState::on_enter()
{
	CharacterMgr::GetInstance()->get_player()->set_animation("run");

	play_audio(_T("player_run"), true);
}

void PlayerRunState::on_update(float delta)
{
	std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(CharacterMgr::GetInstance()->get_player());
	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->get_move_axis() == 0) {
		player->switch_state("idle");
	}
	else if (player->can_jump()) {
		player->switch_state("jump");
	}
	else if (player->can_attack()) {
		player->switch_state("attack");
	}
	else if (player->can_roll()) {
		player->switch_state("roll");
	}
}

void PlayerRunState::on_exit()
{
	stop_audio(_T("player_run"));
}
