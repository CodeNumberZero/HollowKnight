#pragma once
#include "Timer.h"
#include "StateNode.h"

/* 玩家角色的各个状态节点继承基类并重写相应的进入退出以及更新的逻辑即可 */
/* 对于攻击、翻滚等退出条件受时间影响的节点都添加了Timer计时器类来控制这些状态的跳转逻辑 */

class PlayerAttackState : public StateNode
{
private:
	Timer timer;
	void update_hit_box_position();

public:
	PlayerAttackState();
	~PlayerAttackState() = default;

	void on_enter() override;
	void on_update(float delta) override;
	void on_exit() override;
};

class PlayerDeadState : public StateNode
{
private:
	Timer timer;

public:
	PlayerDeadState();
	~PlayerDeadState() = default;

	void on_enter() override;                               
	void on_update(float delta) override;
};

class PlayerFallState : public StateNode
{
public:
	PlayerFallState() = default;
	~PlayerFallState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class PlayerIdleState : public StateNode
{
public:
	PlayerIdleState() = default;
	~PlayerIdleState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class PlayerJumpState : public StateNode
{
public:
	PlayerJumpState() = default;
	~PlayerJumpState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class PlayerRollState : public StateNode
{
private:
	Timer timer;

public:
	PlayerRollState();
	~PlayerRollState() = default;

	void on_enter() override;
	void on_update(float delta) override;
	void on_exit() override;
};

class PlayerRunState : public StateNode
{
private:
	Timer timer;

public:
	PlayerRunState() = default;
	~PlayerRunState() = default;

	void on_enter() override;
	void on_update(float delta) override;
	void on_exit() override;
};

