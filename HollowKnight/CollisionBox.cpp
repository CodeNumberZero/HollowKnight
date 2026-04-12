#include "CollisionBox.h"

void CollisionBox::set_enabled(bool flag) {
	enabled = flag;
}

void CollisionBox::set_layer_src(CollisionLayer layer)
{
	layer_src = layer;
}

void CollisionBox::set_layer_dst(CollisionLayer layer)
{
	layer_dst = layer;
}

void CollisionBox::set_on_collide(std::function<void()> on_collide)
{
	this->on_collide = on_collide;
}

void CollisionBox::set_size(const MyVector& size)
{
	this->size = size;
}

const MyVector& CollisionBox::get_size() const {
	return size;
}

void CollisionBox::set_position(const MyVector& position) {
	this->position = position;
}