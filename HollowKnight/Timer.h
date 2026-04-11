#pragma once
#include <functional>

// 定时器类，提供基本的定时功能
class Timer
{
private:
	float pass_time = 0;                     // 已经经过的时间(累计)
	float wait_time = 0;                     // 需要等待的时间(每隔多久触发一次)
	bool paused = false;                     // 是否暂停
	bool shotted = false;                    // 是否已经触发过定时回调
	bool one_shot = false;                   // 是否只触发一次
	std::function<void()> on_timeout;        // 触发定时器时执行的回调函数

public :
	Timer() = default;
	~Timer() = default;

	void restart();                          // 重置定时器
	void set_wait_time(float time);          // 设置等待时长 
	void set_one_shot(bool flag);            // 设置是否为单次触发模式
	void set_on_timeout(std::function<void()> on_timeout);  // 设置回调函数
	void pause();                            // 暂停计时
	void resume();                           // 恢复计时
	void on_update(float delta);             // 每帧更新，传入时间增量
};

