#pragma once
#include <functional>

#include "MyVector.h"
#include "CollisionLayer.h"

class CollisionMgr;

// 碰撞箱类
class CollisionBox
{
	friend class CollisionMgr;                                 // 设为友元,允许管理器访问私有构造和析构
private:
	MyVector size;                                             // 碰撞箱的尺寸
	MyVector position;                                         // 碰撞箱的中心
	bool enabled = true;                                       // 表示当前碰撞箱是否启用碰撞检测
	std::function<void()> on_collide;                          // 碰撞发生后的回调逻辑
	CollisionLayer layer_src = CollisionLayer::None;           // 碰撞箱自身所处的碰撞层
	CollisionLayer layer_dst = CollisionLayer::None;           // 与当前碰撞箱碰撞的目标所处的碰撞层

	CollisionBox() = default;                                  // 将构造设为私有,除了碰撞管理器外不允许随意创建碰撞箱对象

public:
	~CollisionBox() = default;
	void set_enabled(bool flag);
	void set_layer_src(CollisionLayer layer);
	void set_layer_dst(CollisionLayer layer);
	void set_on_collide(std::function<void()> on_collide);
	void set_size(const MyVector& size);
	const MyVector& get_size() const;
	void set_position(const MyVector& position);
};

