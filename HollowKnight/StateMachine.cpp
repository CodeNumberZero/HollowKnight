#include "StateMachine.h"

void StateMachine::on_update(float delta) {
	if (!current_state)                                    // 先检查当前状态指针是否为空
		return;
	if (need_init) {                                       // 不为空则检查状态机是否需要初始化
		current_state->on_enter();
		need_init = false;
	}
	current_state->on_update(delta);                       // 调用当前节点的更新逻辑
}

void StateMachine::set_entry(const std::string& id)
{
	current_state = state_pool[id];
}

void StateMachine::switch_to(const std::string& id)
{
	if (current_state)
		current_state->on_exit();                          // 需要切换新状态时,需要先退出当前状态节点
	current_state = state_pool[id];
	if (current_state)
		current_state->on_enter();                         // 然后进入新的状态节点
}

void StateMachine::register_state(const std::string& id, std::shared_ptr<StateNode> state_node)
{
	state_pool[id] = state_node;
}
