#pragma once
#include "StateNode.h"
#include "Timer.h"
#include "config.h"

class EnemyAimState : public StateNode
{
private:
	Timer timer;

public:
	EnemyAimState();
	~EnemyAimState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class EnemyDashInAirState : public StateNode
{
public:
	EnemyDashInAirState() = default;
	~EnemyDashInAirState() = default;

	void on_enter() override;
	void on_update(float delta) override;
	void on_exit() override;
};

class EnemyDashOnFloorState : public StateNode
{
private:
	Timer timer;

public:
	EnemyDashOnFloorState();
	~EnemyDashOnFloorState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class EnemyDeadState : public StateNode
{
public:
	EnemyDeadState() = default;
	~EnemyDeadState() = default;

	void on_enter() override;
};

class EnemyFallState : public StateNode
{
public:
	EnemyFallState() = default;
	~EnemyFallState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class EnemyIdleState : public StateNode
{
private:
	Timer timer;

public:
	EnemyIdleState();
	~EnemyIdleState() = default;

	void on_enter() override;
	void on_update(float delta) override;
	void on_exit() override;
};

class EnemyJumpState : public StateNode
{
public:
	EnemyJumpState() = default;
	~EnemyJumpState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class EnemyRunState : public StateNode
{
public:
	EnemyRunState() = default;
	~EnemyRunState() = default;

	void on_enter() override;
	void on_update(float delta) override;
	void on_exit() override;
};

class EnemySquatState : public StateNode
{
private:
	Timer timer;

public:
	EnemySquatState();
	~EnemySquatState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class EnemyThrowBarbState : public StateNode
{
private:
	Timer timer;

public:
	EnemyThrowBarbState();
	~EnemyThrowBarbState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class EnemyThrowSilkState : public StateNode
{
private:
	Timer timer;

public:
	EnemyThrowSilkState();
	~EnemyThrowSilkState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};

class EnemyThrowSwordState : public StateNode
{
private:
	Timer timer_throw;
	Timer timer_switch;

public:
	EnemyThrowSwordState();
	~EnemyThrowSwordState() = default;

	void on_enter() override;
	void on_update(float delta) override;
};