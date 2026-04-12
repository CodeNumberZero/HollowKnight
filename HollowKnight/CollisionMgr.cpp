#include "CollisionMgr.h"
#include <graphics.h>

std::shared_ptr<CollisionMgr> CollisionMgr::_instance = nullptr;

// 原理：static变量生命周期随同程序，而在C++11之后，static局部变量的初始化是线程安全的(静态局部变量的初始化只会在控制首次进入包含它的作用域时发生，且之后不会重复初始化)
std::shared_ptr<CollisionMgr> CollisionMgr::GetInstance() {
	static std::once_flag flag;                         // 标志位，用于标记std::call_once调用的目标函数是否已执行  
	std::call_once(flag, []() {                         // std::call_once 是C++11引入的线程安全工具，核心作用是保证某个函数/操作在多线程环境下"仅被执行一次"(即使多个线程同时调用)
		//_instance = std::make_shared<ResourcesMgr>();            // 不能使用这种方式构造。原因:make_shared需要调用构造函数,而这里的托管对象是单例,构造设置为私有了,make_shared无权限调用
		_instance = std::shared_ptr<CollisionMgr>(new CollisionMgr);          // 能用new进行构造是因为：new是在类的成员函数内使用的，而类的成员函数本身就有权限访问私有构造函数
		});
	return _instance;
}

std::shared_ptr<CollisionBox> CollisionMgr::CreateCollisionBox()
{
	auto collision_box = std::shared_ptr<CollisionBox>(new CollisionBox());
	CollisionBoxList.push_back(collision_box);
	return collision_box;
}

void CollisionMgr::DestroyCollisionBox(std::shared_ptr<CollisionBox>& collision_box)
{
	CollisionBoxList.erase(std::remove(CollisionBoxList.begin(), CollisionBoxList.end(), collision_box), CollisionBoxList.end());
}

void CollisionMgr::ProcessCollide()
{
	for (std::shared_ptr<CollisionBox> collision_box_src : CollisionBoxList) {                    // 遍历每一个碰撞箱
		if (!collision_box_src->enabled || collision_box_src->layer_dst == CollisionLayer::None)  // 判断是否启用了碰撞检测以及碰撞目标的层级是否为空
			continue;

		for (std::shared_ptr<CollisionBox> collision_box_dst : CollisionBoxList) {
			// 检测列表中除自身之外的其他启用碰撞的同层级对象的碰撞
			if (!collision_box_dst->enabled                                                       // 目标未启用
				|| collision_box_src == collision_box_dst                                         // 自己和自己不碰撞
				|| collision_box_src->layer_dst != collision_box_dst->layer_src)                  // 层级不匹配
				continue;

			/*
				AABB轴对齐矩形碰撞加测算法
				1、X轴碰撞判断：bool is_collide_x = (max(src右边界, dst右边界) - min(src左边界, dst左边界) <= src宽度 + dst宽度);
					逻辑翻译：两个矩形在水平方向重叠的条件：两个矩形的总覆盖宽度 ≤ 两个矩形的宽度之和
					如果水平方向完全分开 -> 总覆盖宽度 ＞ 宽度之和 -> 不碰撞
					如果水平方向有重叠 -> 总覆盖宽度 ≤ 宽度之和 -> 碰撞
				2、Y轴碰撞判断与X轴类似
			*/
			bool is_collide_x = (max(collision_box_src->position.x + collision_box_src->size.x / 2, collision_box_dst->position.x + collision_box_dst->size.x / 2)
				- min(collision_box_src->position.x - collision_box_src->size.x / 2, collision_box_dst->position.x - collision_box_dst->size.x / 2)
				<= collision_box_src->size.x + collision_box_dst->size.x);
			bool is_collide_y = (max(collision_box_src->position.y + collision_box_src->size.y / 2, collision_box_dst->position.y + collision_box_dst->size.y / 2)
				- min(collision_box_src->position.y - collision_box_src->size.y / 2, collision_box_dst->position.y - collision_box_dst->size.y / 2)
				<= collision_box_src->size.y + collision_box_dst->size.y);
			
			// 碰撞矩形发生重合时就调用被碰撞物体的回调函数
			if (is_collide_x && is_collide_y && collision_box_dst->on_collide)
				collision_box_dst->on_collide();
		}
	}
}

void CollisionMgr::OnDebugRender()
{
	for (auto collision_box : CollisionBoxList) {
		setlinecolor(collision_box->enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));     // 设置线条颜色：启用=淡红色，禁用=淡蓝色
		rectangle((int)(collision_box->position.x - collision_box->size.x / 2),             // 画出每个碰撞箱的矩形边框
			(int)(collision_box->position.y - collision_box->size.y / 2),
			(int)(collision_box->position.x + collision_box->size.x / 2),
			(int)(collision_box->position.y + collision_box->size.y / 2));
	}
}
