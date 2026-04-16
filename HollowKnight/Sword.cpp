#include "Sword.h"
#include "ResourcesMgr.h"
#include "CollisionMgr.h"

Sword::Sword(const MyVector& position, bool move_left)
{
	animation.set_interval(0.1f);
	animation.set_loop(true);
	animation.set_anchor_mode(AnchorMode::Centered);
	animation.add_frame(ResourcesMgr::GetInstance()->find_atlas(move_left ? "sword_left" : "sword_right"));

	collision_box = CollisionMgr::GetInstance()->CreateCollisionBox();
	collision_box->set_layer_src(CollisionLayer::None);
	collision_box->set_layer_dst(CollisionLayer::Player);
	collision_box->set_size({ 195, 10 });

	this->position = position;
	this->velocity = { move_left ? -ENEMY_SWORD_SPEED_MOVE : ENEMY_SWORD_SPEED_MOVE, 0 };
}

Sword::~Sword()
{
	CollisionMgr::GetInstance()->DestroyCollisionBox(collision_box);
}

void Sword::on_update(float delta) {
	position += velocity * delta;                             // 根究移动速度修改其位置实现飞行效果
	animation.set_position(position);                         // 修改位置的同时还要同步设置对应的动画和碰撞箱位置
	collision_box->set_position(position);

	animation.on_update(delta);
	if (position.x <= -200 || position.x >= getwidth() + 200) // 当剑飞出屏幕一定距离后,另其失效
		is_valid = false;
}

void Sword::on_render() {
	animation.on_render();
}

bool Sword::check_valid() const
{
	return is_valid;
}
