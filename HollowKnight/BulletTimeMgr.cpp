#include "BulletTimeMgr.h"

std::shared_ptr<BulletTimeMgr> BulletTimeMgr::_instance = nullptr;

std::shared_ptr<BulletTimeMgr> BulletTimeMgr::GetInstance() {
	static std::once_flag flag;
	std::call_once(flag, []() {
		_instance = std::shared_ptr<BulletTimeMgr>(new BulletTimeMgr);
		});
	return _instance;
}

float BulletTimeMgr::lerp(float start, float end, float t) {
	return (1 - t) * start + t * end;
}

// 让整个屏幕平滑变暗
void BulletTimeMgr::post_process()
{
	DWORD* buffer = GetImageBuffer();                                   // 获取当前窗口的像素缓冲区
	int w = getwidth(), h = getheight();
	for (int y = 0; y < h; y++)                                         // 遍历屏幕上的每一个像素，根据子弹时间进度 progress 把画面调暗
	{
		for (int x = 0; x < w; x++)
		{
			int index = y * w + x;
			DWORD color = buffer[index];                               // 当前像素的原始颜色
			BYTE r = static_cast<BYTE>(GetBValue(color) * lerp(1.0f, DST_COLOR_FACTOR, progress));
			BYTE g = static_cast<BYTE>(GetGValue(color) * lerp(1.0f, DST_COLOR_FACTOR, progress));
			BYTE b = static_cast<BYTE>(GetRValue(color) * lerp(1.0f, DST_COLOR_FACTOR, progress));
			buffer[index] = BGR(RGB(r, g, b) | ((DWORD)(BYTE)(255)) << 24);
		}
	}
}

void BulletTimeMgr::set_status(Status status)
{
	this->status = status;
}

// 计算并返回被放慢后的时间;输入参数是真实世界的帧时间,返回值是被子弹时间放慢后的时间
// 返回值是根据当前子弹时间状态进度计算得到的缩放后帧更新时间
float BulletTimeMgr::on_update(float delta) {
	float delta_progress = SPEED_PROGRESS * delta;
	progress += delta_progress * (status == Status::Entering ? 1 : -1);

	if (progress < 0) progress = 0;
	if (progress > 1) progress = 1;

	return delta * lerp(1.0f, DST_DELTA_FACTOR, progress);
}
