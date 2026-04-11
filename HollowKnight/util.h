#pragma once
#include <graphics.h>

#pragma comment(lib, "winmm.lib")    // 预处理指令,作用是告诉链接器在链接时自动将winmm.lib这个库文件链接到程序中;链接Windows多媒体库，提供时间相关函数
#pragma comment(lib, "msimg32.lib")  // 链接msimg32库;MSIMG32.lib是Windows图形设备接口(GDI)的扩展库,提供了高级图像处理函数,主要用于图像的透明混合和Alpha通道合成

struct Rect
{
	int x, y;                       // 记录矩形左上角的坐标位置;为了与绘图接口参数一致，所以选择了整型而不是浮点型
	int w, h;                       // 记录矩形的宽和高
};

/*
	1、增强版的图像绘制函数，核心功能是将源图片中任意矩形区域，经过Alpha透明混合后，绘制到目标位置的任意矩形区域,实现灵活的透明图片渲染
	2、接受的三个参数分别是：需要绘制的图像对象、目标矩形和源矩形
	 (1)源图像指针
	 (2)目标矩形是执行绘图时裁剪下来的这部分图片贴附在窗口的哪一部分区域
	 (3)源矩形决定着我们需要在原始图片素材上裁剪的区域位置和大小;当不需要裁剪时可以将源矩形默认为空指针
	3、inline的作用是使放在头文件中的函数避免多重定义的问题;如果只在单个cpp文件中使用,可以去掉inline,使用static
*/
inline void putimage_ex(IMAGE* img, const Rect* rect_dst, const Rect* rect_src = nullptr) {
	static BLENDFUNCTION blend_func = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };  // 定义一个静态的BLENDFUNCTION结构体变量,用于指定Alpha混合的参数;AC_SRC_OVER表示源图像覆盖目标图像;0表示没有特殊标志;255表示完全不透明;AC_SRC_ALPHA表示使用源图像的Alpha通道进行混合
	AlphaBlend(                                                 // Windows GDI绘图函数(如AlphaBlend)需要HDC参数
		GetImageHDC(GetWorkingImage()),                         // 目标HDC:当前工作图像的设备上下文;GetWorkingImage()获取当前正在使用的IMAGE对象指针;GetImageHDC()获取IMAGE对象关联的HDC(设备上下文句柄)
		rect_dst->x, rect_dst->y, rect_dst->w, rect_dst->h,     // 目标矩形
		GetImageHDC(img),                                       // 源HDC:源图像的设备上下文
		rect_src ? rect_src->x : 0, rect_src ? rect_src->y : 0, 
		rect_src ? rect_src->w : img->getwidth(), rect_src ? rect_src->h : img->getheight(), 
		blend_func                                              // Alpha混合参数
	);
}

// 这三个函数是对Windows MCI(Media Control Interface)的简单封装，用于播放音频文件
// 打开音频文件,并给它起一个别名,后续操作使用这个别名
inline void load_audio(LPCTSTR path, LPCTSTR id)
{
	// static的必要性？
	static TCHAR str_cmd[512];
	_stprintf_s(str_cmd, _T("open %s alias %s"), path, id);     // 命令格式：open <文件路径> alias <别名>
	mciSendString(str_cmd, NULL, 0, NULL);                      // 向 MCI 设备发送命令字符串
} 

// 播放已加载的音频，可选择是否循环
inline void play_audio(LPCTSTR id, bool is_loop = false)
{
	static TCHAR str_cmd[512];
	//_stprintf_s(str_cmd, _T("play %s %s from θ"), id, is_loop ? _T("repeat") : _T("")); // 单次播放：play <别名>;循环播放：play <别名> repeat
	_stprintf_s(str_cmd, _T("play %s %s"), id, is_loop ? _T("repeat") : _T("")); // 单次播放：play <别名>;循环播放：play <别名> repeat
	mciSendString(str_cmd, NULL, 0, NULL);
}

// 停止正在播放的音频
inline void stop_audio(LPCTSTR id)
{
	static TCHAR str_cmd[512]; 
	_stprintf_s(str_cmd, _T("stop %s"), id);                    // stop <别名>
	mciSendString(str_cmd, NULL, 0, NULL);
}

