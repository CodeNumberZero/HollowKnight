#include "Enemy.h"
#include "EnemyStateNode.h"
#include "CharacterMgr.h"
#include "Player.h"

// 瞄准状态是空中冲刺的前置状态,敌人悬停在空中等待0.5s,然后跳转到冲刺状态冲向玩家
EnemyAimState::EnemyAimState()
{
	timer.set_wait_time(0.5f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&](){                                                                  // 瞄准状态结束后触发回调,启用重力并切换到空中冲刺状态
			auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
			enemy->set_gravity_enabled(true);
			enemy->switch_state("dash_in_air");
	});
}

void EnemyAimState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("aim");

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_gravity_enabled(false);                                                           // 禁用重力并将速度置零实现悬停效果
	enemy->set_velocity({ 0, 0 });
	timer.restart();
}

void EnemyAimState::on_update(float delta)
{
	timer.on_update(delta);

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
}

// 空中冲刺状态开始时,先计算得到敌人到玩家运动方向的向量,并根据其方向设置其冲刺速度
void EnemyDashInAirState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("dash_in_air");

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	const std::shared_ptr<Character> player = CharacterMgr::GetInstance()->get_player();

	MyVector pos_target = { player->get_position().x, player->get_floor_y() };                      // 玩家正下方地面位置
	enemy->set_velocity((pos_target - enemy->get_position()).normalize() * ENEMY_SPEED_DASH_IN_AIR);// 获取从敌人到目标点的方向,并把方向变成单位向量(只保留方向,长度为1)
	enemy->set_dashing_in_air(true);
	enemy->set_gravity_enabled(false);																// 关闭重力,实现空中直线冲刺,不会往下掉
	enemy->on_dash();

	play_audio(_T("enemy_dash"), false);
}

void EnemyDashInAirState::on_update(float delta)
{
	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());

	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
	else if (enemy->is_on_floor())                                                                  // 空中冲刺时状态的结束条件由是否落地进行判断
		enemy->switch_state("idle");
}

void EnemyDashInAirState::on_exit()
{
	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());

	enemy->set_gravity_enabled(true);																// 退出空中冲刺状态时启用重力
	enemy->set_dashing_in_air(false);
}

EnemyDashOnFloorState::EnemyDashOnFloorState()
{
	timer.set_wait_time(0.5f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&](){																		// 地面冲刺时状态的结束条件由定时器进行控制
			auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
			enemy->set_dashing_on_floor(false);
		}
	);
}

void EnemyDashOnFloorState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("dash_on_floor");

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_velocity({ enemy->get_facing_left() ? -ENEMY_SPEED_DASH_ON_FLOOR : ENEMY_SPEED_DASH_ON_FLOOR, 0 });
	enemy->set_dashing_on_floor(true);
	enemy->on_dash();
	timer.restart();

	play_audio(_T("enemy_dash"), false);
}

void EnemyDashOnFloorState::on_update(float delta)
{
	timer.on_update(delta);

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
	else if (!enemy->get_dashing_on_floor())														// 地面冲刺时状态的结束条件由定时器进行控制
		enemy->switch_state("idle");
}

void EnemyDeadState::on_enter()
{
	MessageBox(GetHWnd(), _T("很好，这样能行！"), _T("挑战成功！"), MB_OK);
	exit(0);																						// 0 = 正常退出;点击确定后游戏直接关闭，结束运行
}

void EnemyFallState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("fall");
}

void EnemyFallState::on_update(float delta)
{
	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
	else if (enemy->is_on_floor())																	// 检查角色是否落到地板上,如果是则切换到闲置状态
		enemy->switch_state("idle");
}

EnemyIdleState::EnemyIdleState()
{
	timer.set_one_shot(true);
	timer.set_on_timeout([&](){
			auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());

			// 闲置状态倒计时结束后,根据敌人当前的剩余生命值做出不同概率的随机动作
			int rand_num = range_random(1, 100);
			if (enemy->get_hp() > ENEMY_HP_THRESHOLD)
			{
				if (rand_num <= 25)
				{
					if (!enemy->is_on_floor())
						enemy->switch_state("fall");
					else
						enemy->switch_state("jump");	// 25%
				}
				else if (rand_num <= 50)
				{
					if (!enemy->is_on_floor())
						enemy->switch_state("fall");
					else
						enemy->switch_state("run");		// 25%
				}
				else if (rand_num <= 80)
				{
					enemy->switch_state("squat");		// 30%
				}
				else if (rand_num <= 90)
				{
					enemy->switch_state("throw_silk");	// 10%
				}
				else
				{
					enemy->switch_state("throw_sword");	// 10%
				}
			}
			else
			{
				if (rand_num <= 25)
				{
					if (!enemy->is_on_floor())
						enemy->switch_state("fall");
					else
						enemy->switch_state("jump");	// 25%
				}
				else if (rand_num <= 60)
				{
					enemy->switch_state("throw_sword");	// 35%
				}
				else if (rand_num <= 70)
				{
					enemy->switch_state("throw_silk");	// 10%
				}
				else if (rand_num <= 90)
				{
					enemy->switch_state("throw_barb");	// 20%
				}
				else
				{
					enemy->switch_state("squat");		// 10%
				}
			}
		}
	);
}

void EnemyIdleState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("idle");

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_velocity({ 0, 0 });

	float wait_time = 0.0f;
	if (enemy->get_hp() > ENEMY_HP_THRESHOLD)
		wait_time = range_random(0, 2) * 0.25f; // 0.0 ~ 0.50s,血量多时,待机更久
	else
		wait_time = range_random(0, 1) * 0.25f; // 0.0 ~ 0.25s,血量少时,待机时间短,攻击更频繁

	timer.set_wait_time(wait_time);
	timer.restart();
}

void EnemyIdleState::on_update(float delta)
{
	timer.on_update(delta);

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
	else if (enemy->get_velocity().y > 0)
		enemy->switch_state("fall");
}

void EnemyIdleState::on_exit()
{
	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_facing_left(enemy->get_position().x > CharacterMgr::GetInstance()->get_player()->get_position().x); // 退出闲置状态时会更根据玩家当前的位置调整敌人的面朝方向
}

void EnemyJumpState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("jump");

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_velocity({ 0, -ENEMY_SPEED_JUMP });							// 进入跳跃状态时设置敌人的竖直方向速度为跳跃速度
}

void EnemyJumpState::on_update(float delta)
{
	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());

	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
	else if (enemy->get_velocity().y > 0)
	{
		// 根据当前敌人的生命值,根据随机概率切换到不同状态
		int rand_num = range_random(1, 100);
		if (enemy->get_hp() > ENEMY_HP_THRESHOLD)
		{
			if (rand_num <= 50)
				enemy->switch_state("aim");				// 50%
			else if (rand_num <= 80)
				enemy->switch_state("fall");			// 30%
			else
				enemy->switch_state("throw_silk");		// 20%
		}
		else
		{
			if (rand_num <= 50)
				enemy->switch_state("throw_silk");		// 50%
			else if (rand_num <= 80)
				enemy->switch_state("fall");			// 30%
			else
				enemy->switch_state("aim");				// 20%
		}
	}
}

void EnemyRunState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("run");
	play_audio(_T("enemy_run"), true);
}

void EnemyRunState::on_update(float delta)
{
	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());

	const MyVector& pos_enemy = enemy->get_position();
	const MyVector& pos_player = CharacterMgr::GetInstance()->get_player()->get_position();
	enemy->set_velocity({ pos_enemy.x < pos_player.x ? ENEMY_SPEED_RUN : -ENEMY_SPEED_RUN, 0 });

	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
	else if (abs(pos_enemy.x - pos_player.x) <= ENEMY_MIN_DIS)  // 奔跑状态的退出条件不是定时器,而是需要敌人与玩家间的水平距离小于一定阈值时在跳转状态进入下一步动作
	{
		int rand_num = range_random(1, 100);
		if (enemy->get_hp() > ENEMY_HP_THRESHOLD)
		{
			if (rand_num <= 75)
				enemy->switch_state("squat");			// 75%
			else
				enemy->switch_state("throw_silk");		// 25%
		}
		else
		{
			if (rand_num <= 75)
				enemy->switch_state("throw_silk");		// 75%
			else
				enemy->switch_state("squat");			// 25%
		}
		stop_audio(_T("enemy_run"));
	}
}

void EnemyRunState::on_exit()
{
	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_velocity({ 0, 0 });
}

// 下蹲状态是地面冲刺的前置状态,需要使用定时器控制退出时机
EnemySquatState::EnemySquatState()
{
	timer.set_wait_time(0.5f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&](){
		auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
		enemy->switch_state("dash_on_floor");
	});
}

void EnemySquatState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("squat");

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_facing_left(enemy->get_position().x > CharacterMgr::GetInstance()->get_player()->get_position().x);
	timer.restart();
}

void EnemySquatState::on_update(float delta)
{
	timer.on_update(delta);

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
}

EnemyThrowBarbState::EnemyThrowBarbState()
{
	timer.set_wait_time(0.8f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&](){													// 进入状态一定时间(0.8s)后,调用封装好的扔刺球方法即可,然后将状态切换到闲置状态
			auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
			enemy->throw_barbs();
			enemy->switch_state("idle");
		}
	);
}

void EnemyThrowBarbState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("throw_barb");
	timer.restart();
	play_audio(_T("enemy_throw_barbs"), false);
}

void EnemyThrowBarbState::on_update(float delta)
{
	timer.on_update(delta);

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
}

EnemyThrowSilkState::EnemyThrowSilkState()
{
	timer.set_wait_time(0.9f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&](){
			auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
			enemy->set_gravity_enabled(true);													// 退出时开启重力 
			enemy->set_throwing_silk(false);
			if (!enemy->is_on_floor() && enemy->get_hp() > ENEMY_HP_THRESHOLD && range_random(1, 100) <= 25)
				enemy->switch_state("aim");
			else if (!enemy->is_on_floor())
				enemy->switch_state("fall");
			else
				enemy->switch_state("idle");
		}
	);
}

void EnemyThrowSilkState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("throw_silk");

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	enemy->set_gravity_enabled(false);															// 进入时禁用重力
	enemy->set_throwing_silk(true);
	enemy->set_velocity({ 0, 0 });
	enemy->on_throw_silk();

	timer.restart();

	play_audio(_T("enemy_throw_silk"), false);
}

void EnemyThrowSilkState::on_update(float delta)
{
	timer.on_update(delta);

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
}

EnemyThrowSwordState::EnemyThrowSwordState()
{
	// "扔"是一个持续性的动作,我们需要让飞剑道具生成的时机恰到好处,也就是说飞剑应该出现在动画播放的过程中,而不是动画播放开始时或动画播放结束后,因此使用一个额外的定时器单独控制飞剑道具的生成
	timer_throw.set_wait_time(0.65f);
	timer_throw.set_one_shot(true);
	timer_throw.set_on_timeout([&](){
			auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
			enemy->throw_sword();
			play_audio(_T("enemy_throw_sword"), false);
		}
	);

	// 在状态切换的定时器中,依然根据敌人当前的生命值根据不同概率进入不同状态
	timer_switch.set_wait_time(1.0f);
	timer_switch.set_one_shot(true);
	timer_switch.set_on_timeout([&](){
			auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());

			int rand_num = range_random(1, 100);
			if (enemy->get_hp() > ENEMY_HP_THRESHOLD)
			{
				if (rand_num <= 50)
					enemy->switch_state("squat");			// 50%
				else if (rand_num <= 80)
					enemy->switch_state("jump");			// 30%
				else
					enemy->switch_state("idle");			// 20%
			}
			else
			{
				if (rand_num <= 50)
					enemy->switch_state("jump");			// 50%
				else if (rand_num <= 80)
					enemy->switch_state("throw_silk");		// 30%
				else
					enemy->switch_state("idle");			// 20%
			}
		}
	);
}

void EnemyThrowSwordState::on_enter()
{
	CharacterMgr::GetInstance()->get_enemy()->set_animation("throw_sword");
	timer_throw.restart();
	timer_switch.restart();
}

void EnemyThrowSwordState::on_update(float delta)
{
	timer_throw.on_update(delta);
	timer_switch.on_update(delta);

	auto enemy = std::dynamic_pointer_cast<Enemy>(CharacterMgr::GetInstance()->get_enemy());
	if (enemy->get_hp() <= 0)
		enemy->switch_state("dead");
}
