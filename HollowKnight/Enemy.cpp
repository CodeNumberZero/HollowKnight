#include "Enemy.h"
#include "CollisionMgr.h"
#include "ResourcesMgr.h"
#include "EnemyStateNode.h"

Enemy::Enemy()
{
	hp = ENEMY_HP;
	is_facing_left = true;
	position = { 1050, 200 };
	logic_height = 150;

	hit_box->set_size({ 50, 80 });
	hurt_box->set_size({ 100, 180 });

	hit_box->set_layer_src(CollisionLayer::None);
	hit_box->set_layer_dst(CollisionLayer::Player);

	hurt_box->set_layer_src(CollisionLayer::Enemy);
	hurt_box->set_layer_dst(CollisionLayer::None);
	hurt_box->set_on_collide([&]() { decrease_hp(); });

	collision_box_silk = CollisionMgr::GetInstance()->CreateCollisionBox();
	collision_box_silk->set_layer_src(CollisionLayer::None);
	collision_box_silk->set_layer_dst(CollisionLayer::Player);
	collision_box_silk->set_size({ 225, 225 });
	collision_box_silk->set_enabled(false);

	// 瞄准动画
	AnimationGroup& animation_aim = animation_pool["aim"];
	Animation& animation_aim_left = animation_aim.left;
	animation_aim_left.set_interval(0.05f);
	animation_aim_left.set_loop(false);
	animation_aim_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_aim_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_aim_left"));

	Animation& animation_aim_right = animation_aim.right;
	animation_aim_right.set_interval(0.05f);
	animation_aim_right.set_loop(false);
	animation_aim_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_aim_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_aim_right"));

	// 空中冲刺动画
	AnimationGroup& animation_dash_in_air = animation_pool["dash_in_air"];
	Animation& animation_dash_in_air_left = animation_dash_in_air.left;
	animation_dash_in_air_left.set_interval(0.05f);
	animation_dash_in_air_left.set_loop(true);
	animation_dash_in_air_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_dash_in_air_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_dash_in_air_left"));

	Animation& animation_dash_in_air_right = animation_dash_in_air.right;
	animation_dash_in_air_right.set_interval(0.05f);
	animation_dash_in_air_right.set_loop(true);
	animation_dash_in_air_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_dash_in_air_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_dash_in_air_right"));

	// 地面冲刺动画
	AnimationGroup& animation_dash_on_floor = animation_pool["dash_on_floor"];
	Animation& animation_dash_on_floor_left = animation_dash_on_floor.left;
	animation_dash_on_floor_left.set_interval(0.05f);
	animation_dash_on_floor_left.set_loop(true);
	animation_dash_on_floor_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_dash_on_floor_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_dash_on_floor_left"));

	Animation& animation_dash_on_floor_right = animation_dash_on_floor.right;
	animation_dash_on_floor_right.set_interval(0.05f);
	animation_dash_on_floor_right.set_loop(true);
	animation_dash_on_floor_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_dash_on_floor_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_dash_on_floor_right"));

	// 跌落动画
	AnimationGroup& animation_fall = animation_pool["fall"];
	Animation& animation_fall_left = animation_fall.left;
	animation_fall_left.set_interval(0.1f);
	animation_fall_left.set_loop(true);
	animation_fall_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_fall_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_fall_left"));

	Animation& animation_fall_right = animation_fall.right;
	animation_fall_right.set_interval(0.1f);
	animation_fall_right.set_loop(true);
	animation_fall_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_fall_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_fall_right"));

	// 闲置动画
	AnimationGroup& animation_idle = animation_pool["idle"];
	Animation& animation_idle_left = animation_idle.left;
	animation_idle_left.set_interval(0.1f);
	animation_idle_left.set_loop(true);
	animation_idle_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_idle_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_idle_left"));

	Animation& animation_idle_right = animation_idle.right;
	animation_idle_right.set_interval(0.1f);
	animation_idle_right.set_loop(true);
	animation_idle_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_idle_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_idle_right"));

	// 跳跃动画
	AnimationGroup& animation_jump = animation_pool["jump"];
	Animation& animation_jump_left = animation_jump.left;
	animation_jump_left.set_interval(0.1f);
	animation_jump_left.set_loop(true);
	animation_jump_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_jump_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_jump_left"));

	Animation& animation_jump_right = animation_jump.right;
	animation_jump_right.set_interval(0.1f);
	animation_jump_right.set_loop(true);
	animation_jump_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_jump_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_jump_right"));

	// 奔跑动画
	AnimationGroup& animation_run = animation_pool["run"];
	Animation& animation_run_left = animation_run.left;
	animation_run_left.set_interval(0.05f);
	animation_run_left.set_loop(true);
	animation_run_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_run_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_run_left"));

	Animation& animation_run_right = animation_run.right;
	animation_run_right.set_interval(0.05f);
	animation_run_right.set_loop(true);
	animation_run_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_run_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_run_right"));

	// 下蹲动画
	AnimationGroup& animation_squat = animation_pool["squat"];
	Animation& animation_squat_left = animation_squat.left;
	animation_squat_left.set_interval(0.05f);
	animation_squat_left.set_loop(false);
	animation_squat_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_squat_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_squat_left"));

	Animation& animation_squat_right = animation_squat.right;
	animation_squat_right.set_interval(0.05f);
	animation_squat_right.set_loop(false);
	animation_squat_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_squat_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_squat_right"));

	// 释放刺球动画
	AnimationGroup& animation_throw_barb = animation_pool["throw_barb"];
	Animation& animation_throw_barb_left = animation_throw_barb.left;
	animation_throw_barb_left.set_interval(0.1f);
	animation_throw_barb_left.set_loop(false);
	animation_throw_barb_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_throw_barb_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_throw_barb_left"));

	Animation& animation_throw_barb_right = animation_throw_barb.right;
	animation_throw_barb_right.set_interval(0.1f);
	animation_throw_barb_right.set_loop(false);
	animation_throw_barb_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_throw_barb_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_throw_barb_right"));

	// 释放丝绸动画
	AnimationGroup& animation_throw_silk = animation_pool["throw_silk"];
	Animation& animation_throw_silk_left = animation_throw_silk.left;
	animation_throw_silk_left.set_interval(0.1f);
	animation_throw_silk_left.set_loop(true);
	animation_throw_silk_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_throw_silk_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_throw_silk_left"));

	Animation& animation_throw_silk_right = animation_throw_silk.right;
	animation_throw_silk_right.set_interval(0.1f);
	animation_throw_silk_right.set_loop(true);
	animation_throw_silk_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_throw_silk_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_throw_silk_right"));

	// 释放飞剑动画
	AnimationGroup& animation_throw_sword = animation_pool["throw_sword"];
	Animation& animation_throw_sword_left = animation_throw_sword.left;
	animation_throw_sword_left.set_interval(0.05f);
	animation_throw_sword_left.set_loop(false);
	animation_throw_sword_left.set_anchor_mode(AnchorMode::BottomCentered);
	animation_throw_sword_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_throw_sword_left"));

	Animation& animation_throw_sword_right = animation_throw_sword.right;
	animation_throw_sword_right.set_interval(0.05f);
	animation_throw_sword_right.set_loop(false);
	animation_throw_sword_right.set_anchor_mode(AnchorMode::BottomCentered);
	animation_throw_sword_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_throw_sword_right"));

	// 初始化敌人特有的特效动画对象
	animation_silk.set_interval(0.1f);
	animation_silk.set_loop(false);
	animation_silk.set_anchor_mode(AnchorMode::Centered);
	animation_silk.add_frame(ResourcesMgr::GetInstance()->find_atlas("silk"));

	Animation& animation_dash_in_air_vfx_left = animation_dash_in_air_vfx.left;
	animation_dash_in_air_vfx_left.set_interval(0.1f);
	animation_dash_in_air_vfx_left.set_loop(false);
	animation_dash_in_air_vfx_left.set_anchor_mode(AnchorMode::Centered);
	animation_dash_in_air_vfx_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_vfx_dash_in_air_left"));

	Animation& animation_dash_in_air_vfx_right = animation_dash_in_air_vfx.right;
	animation_dash_in_air_vfx_right.set_interval(0.1f);
	animation_dash_in_air_vfx_right.set_loop(false);
	animation_dash_in_air_vfx_right.set_anchor_mode(AnchorMode::Centered);
	animation_dash_in_air_vfx_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_vfx_dash_in_air_right"));

	Animation& animation_dash_on_floor_vfx_left = animation_dash_on_floor_vfx.left;
	animation_dash_on_floor_vfx_left.set_interval(0.1f);
	animation_dash_on_floor_vfx_left.set_loop(false);
	animation_dash_on_floor_vfx_left.set_anchor_mode(AnchorMode::Centered);
	animation_dash_on_floor_vfx_left.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_vfx_dash_on_floor_left"));

	Animation& animation_dash_on_floor_vfx_right = animation_dash_on_floor_vfx.right;
	animation_dash_on_floor_vfx_right.set_interval(0.1f);
	animation_dash_on_floor_vfx_right.set_loop(false);
	animation_dash_on_floor_vfx_right.set_anchor_mode(AnchorMode::Centered);
	animation_dash_on_floor_vfx_right.add_frame(ResourcesMgr::GetInstance()->find_atlas("enemy_vfx_dash_on_floor_right"));

	state_machine.register_state("aim", std::make_shared<EnemyAimState>());
	state_machine.register_state("dash_in_air", std::make_shared<EnemyDashInAirState>());
	state_machine.register_state("dash_on_floor", std::make_shared<EnemyDashOnFloorState>());
	state_machine.register_state("dead", std::make_shared<EnemyDeadState>());
	state_machine.register_state("fall", std::make_shared<EnemyFallState>());
	state_machine.register_state("idle", std::make_shared<EnemyIdleState>());
	state_machine.register_state("jump", std::make_shared<EnemyJumpState>());
	state_machine.register_state("run", std::make_shared<EnemyRunState>());
	state_machine.register_state("squat", std::make_shared<EnemySquatState>());
	state_machine.register_state("throw_barb", std::make_shared<EnemyThrowBarbState>());
	state_machine.register_state("throw_silk", std::make_shared<EnemyThrowSilkState>());
	state_machine.register_state("throw_sword", std::make_shared<EnemyThrowSwordState>());

	state_machine.set_entry("idle");
}

Enemy::~Enemy()
{
	CollisionMgr::GetInstance()->DestroyCollisionBox(collision_box_silk);
}

void Enemy::on_update(float delta)
{
	if (velocity.x >= 0.0001f)                                                 // 根据当前移动速度更新角色朝向
		is_facing_left = (velocity.x < 0.0f);

	Character::on_update(delta);

	hit_box->set_position(get_logic_center());

	if (is_throwing_silk)
	{
		collision_box_silk->set_position(get_logic_center());
		animation_silk.set_position(get_logic_center());
		animation_silk.on_update(delta);
	}

	if (is_dashing_in_air || is_dashing_on_floor)
	{
		current_dash_animation->set_position(is_dashing_in_air ? get_logic_center() : position);
		current_dash_animation->on_update(delta);
	}

	for (auto& barb : barb_list)
		barb->on_update(delta);
	for (auto& sword : sword_list)
		sword->on_update(delta);

	//barb_list.erase(std::remove_if(barb_list.begin(), barb_list.end(),[](Barb* barb){
	//			bool can_remove = !barb->check_valid();
	//			if (can_remove)
	//				delete barb;
	//			return can_remove;
	//		}
	//	),
	//	barb_list.end()
	//);

	//sword_list.erase(std::remove_if(sword_list.begin(), sword_list.end(), [](Sword* sword){
	//			bool can_remove = !sword->check_valid();
	//			if (can_remove)
	//				delete sword;
	//			return can_remove;
	//		}
	//	),
	//	sword_list.end()
	//);

	// 清理刺球(erase_if是C++20的函数)
	std::erase_if(barb_list, [](const std::shared_ptr<Barb>& barb) {
		return !barb->check_valid();
	});

	// 清理剑
	std::erase_if(sword_list, [](const std::shared_ptr<Sword>& sword) {
		return !sword->check_valid();
	});
}

void Enemy::on_render()
{
	for (auto& barb : barb_list)
		barb->on_render();
	for (auto& sword : sword_list)
		sword->on_render();

	Character::on_render();

	if (is_throwing_silk)
		animation_silk.on_render();
	if (is_dashing_in_air || is_dashing_on_floor)
		current_dash_animation->on_render();
}

void Enemy::on_hurt()
{
	switch (range_random(1, 3))
	{
	case 1:
		play_audio(_T("enemy_hurt_1"), false);
		break;
	case 2:
		play_audio(_T("enemy_hurt_2"), false);
		break;
	case 3:
		play_audio(_T("enemy_hurt_3"), false);
		break;
	}
}

void Enemy::set_facing_left(bool flag)
{
	is_facing_left = flag;
}

bool Enemy::get_facing_left() const
{
	return is_facing_left;
}

void Enemy::set_dashing_in_air(bool flag)
{
	is_dashing_in_air = flag;
}

bool Enemy::get_dashing_in_air() const
{
	return is_dashing_in_air;
}

void Enemy::set_dashing_on_floor(bool flag)
{
	is_dashing_on_floor = flag;
}

bool Enemy::get_dashing_on_floor() const
{
	return is_dashing_on_floor;
}

void Enemy::set_throwing_silk(bool flag)
{
	is_throwing_silk = flag;
	collision_box_silk->set_enabled(flag);
}

bool Enemy::get_throwing_silk() const
{
	return is_throwing_silk;
}

void Enemy::throw_barbs()
{
	int num_new_barb = range_random(3, 6);                                             // 随机召唤3到6个刺球
	if (barb_list.size() >= 10)
		num_new_barb = 1;
	int width_grid = getwidth() / num_new_barb;                                        // 让刺球位置尽可能均匀分布

	for (int i = 0; i < num_new_barb; i++)
	{
		auto new_barb = std::make_shared<Barb>();
		int rand_x = range_random(i * width_grid, (i + 1) * width_grid);
		int rand_y = range_random(250, 500);
		new_barb->set_position({ static_cast<float>(rand_x), static_cast<float>(rand_y) }); // 对刺球的具体位置进行小幅度的随机偏移
		barb_list.push_back(new_barb);
	}
}

void Enemy::throw_sword()
{
	auto new_sword = std::make_shared<Sword>(get_logic_center(), is_facing_left);      // 根究敌人角色的当前朝向将新飞剑生成在自身中心处即可
	sword_list.push_back(new_sword);
}

void Enemy::on_dash()
{
	// 根据标志判断是空中冲刺还是地面冲刺,并更新对应的特效动画
	if (is_dashing_in_air)
		current_dash_animation = velocity.x < 0 ? &animation_dash_in_air_vfx.left : &animation_dash_in_air_vfx.right;
	else
		current_dash_animation = velocity.x < 0 ? &animation_dash_on_floor_vfx.left : &animation_dash_on_floor_vfx.right;

	current_dash_animation->reset();
}

void Enemy::on_throw_silk()
{
	animation_silk.reset();
}
