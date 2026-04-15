#include "ResourcesMgr.h"

// 通过列表初始化的方法定义图片资源信息列表;static变量,作用域限制在当前源文件,如果需要被其他文件访问，要提供非静态的获取函数接口
static const std::vector<ImageResInfo> image_info_list =
{
	{"background", _T(R"(resources\background.png)")},                   // R"()"标记用于表示原始字符串,让字符串中的转义字符失效,这样便不需要对字符串内部的反斜杠进行转义处理了
	{"ui_heart", _T(R"(resources\ui_heart.png)")},                       // _T是字符集适配宏,它根据项目字符集设置,将字符串转换为正确的类型

	{"player_attack_right", _T(R"(resources\player\attack.png)")},
	{"player_dead_right",	_T(R"(resources\player\dead.png)")},
	{"player_fall_right",	_T(R"(resources\player\fall.png)")},
	{"player_idle_right",	_T(R"(resources\player\idle.png)")},
	{"player_jump_right",	_T(R"(resources\player\jump.png)")},
	{"player_run_right",	_T(R"(resources\player\run.png)")},
	{"player_roll_right",	_T(R"(resources\player\roll.png)")},

	{"player_vfx_attack_down",	_T(R"(resources\player\vfx_attack_down.png)")},
	{"player_vfx_attack_left",	_T(R"(resources\player\vfx_attack_left.png)")},
	{"player_vfx_attack_right", _T(R"(resources\player\vfx_attack_right.png)")},
	{"player_vfx_attack_up",	_T(R"(resources\player\vfx_attack_up.png)")},
	{"player_vfx_jump",			_T(R"(resources\player\vfx_jump.png)")},
	{"player_vfx_land",			_T(R"(resources\player\vfx_land.png)")},
};


// 通过列表初始化的方法定义图集资源信息列表
static const std::vector<AtlasResInfo> atlas_info_list =
{
	{"barb_break",	_T(R"(resources\enemy\barb_break\%d.png)"), 3},
	{"barb_loose",	_T(R"(resources\enemy\barb_loose\%d.png)"), 5},
	{"silk",		_T(R"(resources\enemy\silk\%d.png)"),		9},
	{"sword_left",	_T(R"(resources\enemy\sword\%d.png)"),		3},

	{"enemy_aim_left",				_T(R"(resources\enemy\aim\%d.png)"),			9},
	{"enemy_dash_in_air_left",		_T(R"(resources\enemy\dash_in_air\%d.png)"),	2},
	{"enemy_dash_on_floor_left",	_T(R"(resources\enemy\dash_on_floor\%d.png)"),	2},
	{"enemy_fall_left",				_T(R"(resources\enemy\fall\%d.png)"),			4},
	{"enemy_idle_left",				_T(R"(resources\enemy\idle\%d.png)"),			6},
	{"enemy_jump_left",				_T(R"(resources\enemy\jump\%d.png)"),			8},
	{"enemy_run_left",				_T(R"(resources\enemy\run\%d.png)"),			8},
	{"enemy_squat_left",			_T(R"(resources\enemy\squat\%d.png)"),			10},
	{"enemy_throw_barb_left",		_T(R"(resources\enemy\throw_barb\%d.png)"),		8},
	{"enemy_throw_silk_left",		_T(R"(resources\enemy\throw_silk\%d.png)"),		17},
	{"enemy_throw_sword_left",		_T(R"(resources\enemy\throw_sword\%d.png)"),	16},

	{"enemy_vfx_dash_in_air_left",	_T(R"(resources\enemy\vfx_dash_in_air\%d.png)"),	5},
	{"enemy_vfx_dash_on_floor_left",_T(R"(resources\enemy\vfx_dash_on_floor\%d.png)"),	6},
};

// 用来检查图片对象是否加载成功
static inline bool check_image_valid(std::shared_ptr<IMAGE> image) {
	/*
		1、GetImageBuffer函数用于获取绘图设备的显示缓冲区指针, 获取到的显示缓冲区指针可以直接读写; 在显示缓冲区中, 每个点占用4个字节, 因此：显示缓冲区的大小 = 宽度×高度×4(字节)。像素点在显示缓冲区中按照从左到右、从上向下的顺序依次排列
		2、注意:GetImageBuffer返回IMAGE对象的像素缓冲区指针,有效的IMAGE对返回非空指针(DWORD*),未初始化的IMAGE对象	返回非空指针(指向有效的缓冲区),空指针会返回NULL。
		   关键的地方在于,即使loadimage失败,IMAGE对象仍然是一个有效的对象(只是没有图片数据),GetImageBuffer仍然会返回一个非空指针！
		   所以如果使用下面的语句作为check_image_valid的判断逻辑,几乎会永远返回true,也就会导致在主函数中永远不会抛出异常
			例：return GetImageBuffer(image.get());                 
	*/

	// 通过检查图片尺寸是否有效来判断是否加载完成
	if (!image) 
		return false;
	return (image->getwidth() > 0 && image->getheight() > 0);
}

// static成员变量必须在类外进行初始化，而不能在构造函数内进行初始化
std::shared_ptr<ResourcesMgr> ResourcesMgr::_instance = nullptr;

// 将一张图片水平分割成多个帧,然后对每一帧进行水平翻转(镜像),并保存到目标图片中
void ResourcesMgr::flip_image(std::shared_ptr<IMAGE> src_image, std::shared_ptr<IMAGE> dst_image, int num_h)
{
	int w = src_image->getwidth();
	int h = src_image->getheight();
	int w_frame = w / num_h;                                     // 每帧的宽度
	Resize(dst_image.get(), w, h);
	DWORD* src_buffer = GetImageBuffer(src_image.get());         // 源图片像素数组
	DWORD* dst_buffer = GetImageBuffer(dst_image.get());         // 目标图片像素数组
	for (int i = 0; i < num_h; i++)                              // 遍历每一帧
	{
		int x_left = i * w_frame;                                // 当前帧左边界
		int x_right = (i + 1) * w_frame;                         // 当前帧右边界
		for (int y = 0; y < h; y++)
		{
			for (int x = x_left; x < x_right; x++)
			{
				int idx_src = y * w + x;                         // 源像素索引(原始位置)
				int idx_dst = y * w + x_right - (x - x_left);    // 目标像素索引(镜像位置)
				dst_buffer[idx_dst] = src_buffer[idx_src];
			}
		}
	}
}

// 第一个参数为原始图片在资源池中的id,第二个参数为翻转处理后的图片在资源池中的id,第三个参数表示这一张动画图片素材包含多少个子序列帧
void ResourcesMgr::flip_image(const std::string& src_id, const std::string& dst_id, int num_h)
{
	auto src_image = image_pool[src_id];
	auto dst_image = std::make_shared<IMAGE>();

	flip_image(src_image, dst_image, num_h);

	image_pool[dst_id] = dst_image;
}

// 第一个参数为原始素材的id,第二个参数为处理后的资源存储使用的id
void ResourcesMgr::flip_atlas(const std::string& src_id, const std::string& dst_id)
{
	auto src_atlas = atlas_pool[src_id];
	auto dst_atlas = std::make_shared<Atlas>();

	for (int i = 0; i < src_atlas->get_size(); ++i) {   //  按顺序提取出图集中的每一张图像
		auto img_flipped = std::make_shared<IMAGE>();
		flip_image(src_atlas->get_image(i), img_flipped);
		dst_atlas->add_image(img_flipped);              // 反转后添加到新的图集中
	}
	atlas_pool[dst_id] = dst_atlas;
}

std::shared_ptr<ResourcesMgr> ResourcesMgr::GetInstance() {
	static std::once_flag flag;                         // 标志位，用于标记std::call_once调用的目标函数是否已执行  
	std::call_once(flag, []() {                         // std::call_once 是C++11引入的线程安全工具，核心作用是保证某个函数/操作在多线程环境下"仅被执行一次"(即使多个线程同时调用)
		//_instance = std::make_shared<ResourcesMgr>();            // 不能使用这种方式构造。原因:make_shared需要调用构造函数,而这里的托管对象是单例,构造设置为私有了,make_shared无权限调用
		_instance = std::shared_ptr<ResourcesMgr>(new ResourcesMgr);          // 能用new进行构造是因为：new是在类的成员函数内使用的，而类的成员函数本身就有权限访问私有构造函数
		});
	return _instance;
}

void ResourcesMgr::load()
{
	// 遍历图片和图集资源信息列表
	for (const auto& info : image_info_list) {
		//std::cout << info.id << std::endl;
		std::shared_ptr<IMAGE> image = std::make_shared<IMAGE>();
		loadimage(image.get(), info.path);                                   // 第一个参数为保存图像的IMAGE对象指针,第二个参数为图片文件名
		if (!check_image_valid(image))                                       // 如果图片加载失败则抛出异常
			throw info.path;
		image_pool[info.id] = image;
	}

	for (const auto& info : atlas_info_list) {
		//std::cout << info.id << std::endl;
		std::shared_ptr<Atlas> atlas = std::make_shared<Atlas>();
		atlas->load(info.path, info.num_frame);
		//std::cout << atlas->get_size() << std::endl;
		for (int i = 0; i < atlas->get_size(); ++i) {
			auto image = atlas->get_image(i);
			if (!check_image_valid(image))
				throw info.path;
		}
		atlas_pool[info.id] = atlas;
	}

	// 加载了资源信息后,调用对应的flip函数对相应资源进行翻转处理(因为素材只有单个朝向,要做出角色向左向右移动的动画,还是需要在素材加载后通过操作像素缓冲区进行水平翻转处理)
	flip_image("player_attack_right", "player_attack_left", 5);
	flip_image("player_dead_right", "player_dead_left", 6);
	flip_image("player_fall_right", "player_fall_left", 5);
	flip_image("player_idle_right", "player_idle_left", 5);
	flip_image("player_jump_right", "player_jump_left", 5);
	flip_image("player_run_right", "player_run_left", 10);
	flip_image("player_roll_right", "player_roll_left", 7);

	flip_atlas("sword_left", "sword_right");
	flip_atlas("enemy_aim_left", "enemy_aim_right");
	flip_atlas("enemy_dash_in_air_left", "enemy_dash_in_air_right");
	flip_atlas("enemy_dash_on_floor_left", "enemy_dash_on_floor_right");
	flip_atlas("enemy_fall_left", "enemy_fall_right");
	flip_atlas("enemy_idle_left", "enemy_idle_right");
	flip_atlas("enemy_jump_left", "enemy_jump_right");
	flip_atlas("enemy_run_left", "enemy_run_right");
	flip_atlas("enemy_squat_left", "enemy_squat_right");
	flip_atlas("enemy_throw_barb_left", "enemy_throw_barb_right");
	flip_atlas("enemy_throw_silk_left", "enemy_throw_silk_right");
	flip_atlas("enemy_throw_sword_left", "enemy_throw_sword_right");

	flip_atlas("enemy_vfx_dash_in_air_left", "enemy_vfx_dash_in_air_right");
	flip_atlas("enemy_vfx_dash_on_floor_left", "enemy_vfx_dash_on_floor_right");

	// 加载音频文件
	load_audio(_T(R"(resources\audio\bgm.mp3)"), _T("bgm"));
	load_audio(_T(R"(resources\audio\barb_break.mp3)"), _T("barb_break"));
	load_audio(_T(R"(resources\audio\bullet_time.mp3)"), _T("bullet_time"));

	load_audio(_T(R"(resources\audio\enemy_dash.mp3)"), _T("enemy_dash"));
	load_audio(_T(R"(resources\audio\enemy_run.mp3)"), _T("enemy_run"));
	load_audio(_T(R"(resources\audio\enemy_hurt_1.mp3)"), _T("enemy_hurt_1"));
	load_audio(_T(R"(resources\audio\enemy_hurt_2.mp3)"), _T("enemy_hurt_2"));
	load_audio(_T(R"(resources\audio\enemy_hurt_3.mp3)"), _T("enemy_hurt_3"));
	load_audio(_T(R"(resources\audio\enemy_throw_barbs.mp3)"), _T("enemy_throw_barbs"));
	load_audio(_T(R"(resources\audio\enemy_throw_silk.mp3)"), _T("enemy_throw_silk"));
	load_audio(_T(R"(resources\audio\enemy_throw_sword.mp3)"), _T("enemy_throw_sword"));

	load_audio(_T(R"(resources\audio\player_attack_1.mp3)"), _T("player_attack_1"));
	load_audio(_T(R"(resources\audio\player_attack 2.mp3)"), _T("player_attack_2"));
	load_audio(_T(R"(resources\audio\player_attack_3.mp3)"), _T("player_attack_3"));
	load_audio(_T(R"(resources\audio\player_dead.mp3)"), _T("player_dead"));
	load_audio(_T(R"(resources\audio\player_hurt.mp3)"), _T("player_hurt"));
	load_audio(_T(R"(resources\audio\player_jump.mp3)"), _T("player_jump"));
	load_audio(_T(R"(resources\audio\player_land.mp3)"), _T("player_land"));
	load_audio(_T(R"(resources\audio\player_roll.mp3)"), _T("player_roll"));
	load_audio(_T(R"(resources\audio\player_run.mp3)"), _T("player_run"));

}

std::shared_ptr<Atlas> ResourcesMgr::find_atlas(const std::string& id) const
{
	const auto& iter = atlas_pool.find(id);
	if (iter == atlas_pool.end()) {
		return nullptr;
	}
	return iter->second;
}

std::shared_ptr<IMAGE> ResourcesMgr::find_image(const std::string& id) const
{
	const auto& iter = image_pool.find(id);
	if (iter == image_pool.end()) {
		return nullptr;
	}
	return iter->second;
}
