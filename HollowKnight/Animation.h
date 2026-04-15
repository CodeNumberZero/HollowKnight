#pragma once
#include "Atlas.h"
#include "config.h"
#include "MyVector.h"
#include "util.h"
#include "Timer.h"

#include <functional>
#include <vector>

// 动画类,使用定时器进行驱动
class Animation
{
private:
	// Frame对象用于记录当前帧使用的IMAGE对象以及从这个IMAGE对象上裁剪的区域信息(裁剪区域的生成是根据图片上的帧数量自动计算得到的,无需对外暴露直接操作每一帧信息的接口,所以Frame定义为私有)
	struct Frame
	{
		Rect rect_src;
		std::shared_ptr<IMAGE> image = nullptr;

		Frame() = default;
		Frame(std::shared_ptr<IMAGE> image, const Rect& rect_src) : image(image), rect_src(rect_src) {}
		~Frame() = default;
	};

	Timer timer;						   // 定时器对象,用于控制动画播放
	MyVector position;					   // 标记动画在窗口中渲染的位置
	bool is_loop = true;				   // 动画是否需要循环播放
	std::size_t idx_frame = 0;			   // 记录当前动画播放的帧索引
	std::vector<Frame> frame_list;         // 存储动画所包含的每一帧的帧信息
	std::function<void()> on_finished;	   // 该回调函数用于处理动画播放结束后的处理逻辑(例如子弹破碎动画播放结束后将它们从场景移除)
	AnchorMode anchor_mode = AnchorMode::Centered;

public:
	Animation();
	~Animation();

	void reset();                            // 重置动画
	void set_anchor_mode(AnchorMode mode);
	void set_position(const MyVector& position);
	void set_loop(bool is_loop);
	void set_interval(float interval);
	void set_on_finished(std::function<void()> on_finished);
	void add_frame(std::shared_ptr<IMAGE> image, int num_h); // 为动画添加帧(适用于连续的图像素材,即一幅图片有多个动作,要形成连续动作需要我们进行裁剪;只有一副图片所以用IMAGE管理即可,util.h文件中也封装了对应的裁剪函数)
	void add_frame(std::shared_ptr<Atlas> atlas);            // 为动画添加帧(适用于散装的图片素材,即一副图片只有一个动作,要形成连续动作要加载多张图片,所以要用Atlas图集类进行管理)
	void on_update(float delta);             // 更新动画
	void on_render();                        // 渲染动画

};

