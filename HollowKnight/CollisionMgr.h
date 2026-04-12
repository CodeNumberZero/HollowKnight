#pragma once
#include <memory>
#include <mutex>
#include <vector>

#include "CollisionBox.h"

class CollisionMgr
{
private:
	CollisionMgr() = default;

	static std::shared_ptr<CollisionMgr> _instance;
	std::vector<std::shared_ptr<CollisionBox>> CollisionBoxList;   // 游戏中所有的碰撞箱对象都要放到这个列表中进行更新检测(如果场景非常大或者游戏对象非常多可以扩展使用其他数据结构存储碰撞箱,例如四叉树等,以减少碰撞的相交性运算)

public:
	~CollisionMgr() = default;                                     // 析构也可以设置为私有。但是如果设为私有，子类无法调用，就需要使用辅助类作为删除器来进行析构
	static std::shared_ptr<CollisionMgr> GetInstance();

	// 两个工厂方法,一个负责创建,一个负责回收(因为构造和析构都设为了私有,所有要提供公有接口进行创建和销毁)
	std::shared_ptr<CollisionBox> CreateCollisionBox();            // 工厂方法可以扩展为通过传递参数创建任意形状的碰撞箱
	void DestroyCollisionBox(std::shared_ptr<CollisionBox>& collision_box);

	void ProcessCollide();                                         // 碰撞检测处理逻辑(如果需要让一个碰撞箱可以对多个层级的目标产生碰撞,可以将检测逻辑扩展为按位运算)
	void OnDebugRender();                                          // 调试渲染碰撞箱,在屏幕上用矩形框可视化显示每个碰撞箱的位置和大小
};

