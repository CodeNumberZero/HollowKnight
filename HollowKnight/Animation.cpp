#include "Animation.h"

Animation::Animation()
{
	timer.set_one_shot(false);                                 // 定时器设为循环模式,动画需要不断推进到下一帧,所以不应该只触发一次
	timer.set_on_timeout([&]() {                               // 为定时器设置触发回调
		idx_frame++;                                           // 定时器触发后的逻辑就是累加帧索引使动画推进到下一帧
		if (idx_frame >= frame_list.size()) {
			idx_frame = is_loop ? 0 : frame_list.size() - 1;   // 如果需要循环播放则重置为第一帧，否则保持在最后一帧
			if (!is_loop && on_finished)
				on_finished();                                 // 如果不循环播放且动画结束时有回调函数，则调用它
		}
	});
}

Animation::~Animation()
{
}

void Animation::reset() {
	timer.restart();
	idx_frame = 0;
}

void Animation::set_anchor_mode(AnchorMode mode)
{
	anchor_mode = mode;
}

void Animation::set_position(const MyVector& position)
{
	this->position = position;
}

void Animation::set_loop(bool is_loop)
{
	this->is_loop = is_loop;
}

void Animation::set_interval(float interval)
{
	timer.set_wait_time(interval);
}

void Animation::set_on_finished(std::function<void()> on_finished)
{
	this->on_finished = on_finished;
}

void Animation::add_frame(std::shared_ptr<IMAGE> image, int num_h)
{
	int width = image->getwidth();
	int height = image->getheight();
	int width_frame = width / num_h;                     // 每帧的宽度

	for (int i = 0; i < num_h; ++i) {
		Rect rect_src;
		rect_src.x = i * width_frame, rect_src.y = 0;
		rect_src.w = width_frame, rect_src.h = height;

		frame_list.emplace_back(image, rect_src);        // 将新帧添加到动画的帧列表中
	}
}

void Animation::add_frame(std::shared_ptr<Atlas> atlas)
{
	for (int i = 0; i < atlas->get_size(); i++) {
		std::shared_ptr<IMAGE> image = atlas->get_image(i);
		int width = image->getwidth();
		int height = image->getheight();

		Rect rect_src;
		rect_src.x = 0, rect_src.y = 0;
		rect_src.w = width, rect_src.h = height;
		frame_list.emplace_back(image, rect_src);
	}
}

void Animation::on_update(float delta) {
	timer.on_update(delta);         // 每帧更新时调用定时器的更新方法
}

void Animation::on_render()
{
	const Frame& frame = frame_list[idx_frame];

	Rect rect_dst;
	// 锚点(position.x)对齐到帧的横向中心
	rect_dst.x = (int)position.x - frame.rect_src.w / 2;
	// 根据动画的锚点位置计算得到动画渲染时目标矩形的位置
	rect_dst.y = (anchor_mode == AnchorMode::Centered) 
		? (int)position.y - frame.rect_src.h / 2                     // 中心对齐：锚点对齐到帧的纵向中心
		: (int)position.y - frame.rect_src.h;                        // 底部对齐：锚点对齐到帧的底部边缘
	// 设置目标矩形的宽高(保持原始帧尺寸)
	rect_dst.w = frame.rect_src.w, rect_dst.h = frame.rect_src.h;

	putimage_ex(frame.image.get(), &rect_dst, &frame.rect_src);
}
