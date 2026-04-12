#pragma once

// 状态节点基类
class StateNode
{
public:
	StateNode() = default;
	virtual ~StateNode() = default;             // 析构要声明为虚函数,以便在基类指针或引用指向派生类对象时能正确调用派生类的析构

	virtual void on_enter() {}                  // 当前状态进入时的初始化逻辑
	virtual void on_update(float delta) {}      // 当前节点的行动逻辑
	virtual void on_exit() {}                   // 节点退出时的逻辑
};

