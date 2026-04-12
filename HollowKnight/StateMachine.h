#pragma once
#include "StateNode.h"

#include <graphics.h>
#include <memory>
#include <string>
#include <unordered_map>

// 状态机类
class StateMachine
{
private:
	bool need_init = true;                                                             // 标记当前状态机是否已被初始化过
	std::shared_ptr<StateNode> current_state = nullptr;                                // 当前激活的状态节点
	std::unordered_map<std::string, std::shared_ptr<StateNode>> state_pool;            // 使用状态池,可以更灵活的扩展状态机节点并方便编写状态跳转的代码

public:
	StateMachine() = default;
	~StateMachine() = default;
	void on_update(float delta);                                                       
	void set_entry(const std::string& id);                                             // 设置状态机的初始状态
	void switch_to(const std::string& id);                                             // 切换状态机的激活状态
	void register_state(const std::string& id, std::shared_ptr<StateNode> state_node); // 注册新的状态
};

