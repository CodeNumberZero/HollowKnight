#include "Barb.h"
#include "ResourcesMgr.h"
#include "CollisionMgr.h"
#include "CharacterMgr.h"

Barb::Barb()
{
	diff_period = range_random(0, 6);

	animation_loose.set_interval(0.15f);
	animation_loose.set_loop(true);
	animation_loose.set_anchor_mode(AnchorMode::Centered);
	animation_loose.add_frame(ResourcesMgr::GetInstance()->find_atlas("barb_loose"));

	animation_break.set_interval(0.1f);
	animation_break.set_loop(false);
	animation_break.set_anchor_mode(AnchorMode::Centered);
	animation_break.add_frame(ResourcesMgr::GetInstance()->find_atlas("barb_break"));
	animation_break.set_on_finished([&]() { is_valid = false; });

	collision_box = CollisionMgr::GetInstance()->CreateCollisionBox();
	collision_box->set_layer_src(CollisionLayer::Enemy);                              // 刺球既是一个可以被攻击从而破碎的受击对象
	collision_box->set_layer_dst(CollisionLayer::Player);                             // 也是一个可以击中玩家造成伤害的攻击对象
	collision_box->set_size({ 20, 20 });
	collision_box->set_on_collide([&]() { on_break(); });

	timer_idle.set_wait_time(static_cast<float>(range_random(3, 10)));
	timer_idle.set_one_shot(true);
	timer_idle.set_on_timeout([&](){
		if (stage == Stage::Idle)
		{
			stage = Stage::Aim;
			base_position = current_position;
		}
	});

	timer_aim.set_wait_time(0.75f);
	timer_aim.set_one_shot(true);
	timer_aim.set_on_timeout([&](){
		if (stage == Stage::Aim)
		{
			stage = Stage::Dash;
			const MyVector& player_position = CharacterMgr::GetInstance()->get_player()->get_position();
			velocity = (player_position - current_position).normalize() * SPEED_DASH;
		}
	});
}

Barb::~Barb()
{
	CollisionMgr::GetInstance()->DestroyCollisionBox(collision_box);
}

void Barb::on_update(float delta) {
	// 更新定时器逻辑
	if (stage == Stage::Idle)
		timer_idle.on_update(delta);
	if (stage == Stage::Aim)
		timer_aim.on_update(delta);

	// 更新移动逻辑
	total_delta_time += delta;
	switch (stage)
	{
	case Stage::Idle:
		current_position.y = base_position.y + sin(total_delta_time * 2.0f + diff_period) * 30.0f;
		break;
	case Stage::Aim:
		current_position.x = base_position.x + range_random(-10, 10);
		break;
	case Stage::Dash:
		current_position += velocity * delta;
		if (current_position.y >= CharacterMgr::GetInstance()->get_player()->get_floor_y())         // 碰到地面则销毁
			on_break();
		if (current_position.y <= 0 || current_position.x <= 0 
			|| current_position.x >= getwidth())                                                    // 没碰到地面但飞出屏幕则令其失效
			is_valid = false;
		break;
	}
	collision_box->set_position(current_position);

	// 更新动画逻辑
	current_animation = (stage == Stage::Break ? &animation_break : &animation_loose);
	current_animation->set_position(current_position);
	current_animation->on_update(delta);
}

void Barb::on_render() {
	current_animation->on_render();
}

bool Barb::check_valid() const {
	return is_valid;
}

void Barb::set_position(const MyVector& position) {
	this->base_position = position;
	this->current_position = position;
}

void Barb::on_break()
{
	if (stage == Stage::Break)                                                                     // 如果已经是破碎状态，直接返回
		return;
	stage = Stage::Break;                                                                          // 切换到破碎状态
	collision_box->set_enabled(false);                                                             // 关闭碰撞(不再能伤害玩家)
	play_audio(_T("barb_break"), false);
}
