#pragma once
#include <memory>
#include <mutex>
#include <graphics.h>

// 枚举类,用来标注当前是正在进入子弹时间还是正在退出子弹时间
enum class Status {
	Entering,
	Exiting
};

class BulletTimeMgr
{
private:
	BulletTimeMgr() = default;
	BulletTimeMgr(const BulletTimeMgr&) = delete;
	BulletTimeMgr& operator=(const BulletTimeMgr&) = delete;

	static std::shared_ptr<BulletTimeMgr> _instance;
	float progress = 0;                                       // 0到1之间的浮点数,记录进入子弹时间效果的进度;0表示当前没有子弹时间效果,1表示当前已完全进入子弹时间状态
	Status status = Status::Exiting;
	const float SPEED_PROGRESS = 2.0f;                        // 表示进入和退出的速度
	const float DST_DELTA_FACTOR = 0.35f;                     // 完全进入子弹时间后帧更新时间的缩放
	const float DST_COLOR_FACTOR = 0.35f;                     // 完全进入子弹时间后画面色彩改变比例

	float lerp(float start, float end, float t);              // 该函数计算得到从start到end过渡的进度为t时的数值
public:
	~BulletTimeMgr() = default;
	static std::shared_ptr<BulletTimeMgr> GetInstance();

	void post_process();                                      // 在子弹时间状态下,让除玩家在内的整个场景暗下来,该全屏后处理效果由post_process函数实现该功能
	void set_status(Status status);
	float on_update(float delta);
};

