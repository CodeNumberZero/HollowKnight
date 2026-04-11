#include "Timer.h"

void Timer::restart()
{
	pass_time = 0;
	shotted = false;
}

void Timer::set_wait_time(float time)
{
	wait_time = time;
}

void Timer::set_one_shot(bool flag)
{
	one_shot = flag;
}

void Timer::set_on_timeout(std::function<void()> on_timeout)
{
	this->on_timeout = on_timeout;
}

void Timer::pause()
{
	paused = true;
}

void Timer::resume()
{
	paused = false;
}

void Timer::on_update(float delta)
{
	if (paused) return;

	pass_time += delta;
	if (pass_time >= wait_time) {                                   // 检查是否到达触发时间
		bool can_shot = (!one_shot || (one_shot && !shotted));      // 判断是否可以触发回调;等价于：循环模式或单次模式时未触发过
		shotted = true;                                             // 标记已触发（单次模式会阻止下次触发）
		if (can_shot && on_timeout)                                 // 执行回调(on_timeout的类型是std::function<void()>,它重载了operator bool(),可以隐式转换为bool,表示是否持有有效的可调用对象)
			on_timeout();  
		pass_time -= wait_time;                                     // 减去等待时间，保留剩余时间
	}
}
