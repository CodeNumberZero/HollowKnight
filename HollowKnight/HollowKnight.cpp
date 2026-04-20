#include <chrono>
#include <graphics.h>
#include <iostream>
#include <thread>

#include "BulletTimeMgr.h"
#include "util.h"
#include "ResourcesMgr.h"
#include "CollisionMgr.h"
#include "CharacterMgr.h"

// 绘制背景
static void draw_backgroud() {
    static auto img_backgroud = ResourcesMgr::GetInstance()->find_image("background");
    static Rect rect_dst = {
        (getwidth() - img_backgroud->getwidth()) / 2,
        (getheight() - img_backgroud->getheight()) / 2,
        img_backgroud->getwidth(),
        img_backgroud->getheight()
    };
    putimage_ex(img_backgroud.get(), &rect_dst);
}

static void draw_remain_hp()
{
    static auto img_ui_heart = ResourcesMgr::GetInstance()->find_image("ui_heart");
    Rect rect_dst_player = { 0, 10, img_ui_heart->getwidth(), img_ui_heart->getheight() };
    Rect rect_dst_enemy = { 0, 10, img_ui_heart->getwidth(), img_ui_heart->getheight() };
    for (int i = 0; i < CharacterMgr::GetInstance()->get_player()->get_hp(); i++)
    {
        rect_dst_player.x = 10 + i * 40;                    // 玩家生命值绘制在左上角
        putimage_ex(img_ui_heart.get(), &rect_dst_player);
    }
    for (int i = 0; i < CharacterMgr::GetInstance()->get_enemy()->get_hp(); i++)
    {
        rect_dst_enemy.x = getwidth() - 10 - (i + 1) * 40;  // 敌人生命值绘制在右上角
        putimage_ex(img_ui_heart.get(), &rect_dst_enemy);
    }
}

int main()
{
    bool is_render_collision_box = false;                       // 是否显示碰撞箱

    //HWND hwnd = initgraph(1280, 720, EW_SHOWCONSOLE);        // EX_SHOWCONSOLE标志位表示显示控制台窗口;返回值是窗口句柄(Windows 窗口的唯一标识符)
    HWND hwnd = initgraph(1280, 720);                           // 不显示控制台
    SetWindowText(hwnd, _T("Hollow Knight"));                   // 设置窗口标题

    // 游戏开始前弹出操作提示
    MessageBox(hwnd,
        _T("操作说明：\n")
        _T("跳跃：W, 空格, 上方向键\n")
        _T("左移：A, 左方向键\n")
        _T("翻滚：S, 下方向键\n")
        _T("右移：D, 右方向键\n")
        _T("攻击：鼠标左键\n")
        _T("子弹时间：鼠标右键\n"),
        _T("游戏操作说明"),
        MB_OK | MB_ICONINFORMATION
    );

    // 加载资源
    try {
        ResourcesMgr::GetInstance()->load();
    }
    catch (const LPCTSTR id) {
        TCHAR err_msg[512];
        _stprintf_s(err_msg, _T("无法加载: %s"), id);
        MessageBox(hwnd, err_msg, _T("资源加载失败"), MB_OK | MB_ICONERROR);                // MessageBox是WindowsAPI函数,原生弹窗函数,显示对话框;设置显示的消息内容、对话框标题以及标志位MB_OK | MB_ICONERROR表示显示"确定"按钮+错误图标
        return -1;
    }

    play_audio(_T("bgm"), true);

    /*
        固定帧率(144 FPS)的游戏主循环,通过精确的时间控制让每一帧的时间间隔保持一致
        原因:循环速度很快,如果不根据帧间隔进行动态延时会导致帧率不稳以及性能浪费
    */
    const std::chrono::nanoseconds frame_duration(1000000000 / 144);                      // 1秒÷144帧=每帧约6.94毫秒=6,944,444纳秒;定义每帧应该持续的目标时长(约6.94ms/帧),达到144FPS
    std::chrono::steady_clock::time_point last_tick = std::chrono::steady_clock::now();   // 存储上一帧开始的时间点

    ExMessage msg;                                           // ExMessage是EasyX 的消息结构体,存储鼠标、键盘等输入事件信息
    bool is_quit = false;                                    // 游戏循环的控制标志，true 时退出游戏

    BeginBatchDraw();                                        // 开启批量绘图模式(双缓冲):避免屏幕闪烁，提高绘图效率;正常情况下,每次绘图操作都会立即显示到屏幕,开启批量绘图后,所有绘图操作都绘制到后台缓冲区,调用FlushBatchDraw()时才一次性显示到屏幕

    while (!is_quit) {
        while (peekmessage(&msg)) {                          // 非阻塞地获取一条消息,有消息返回true,无消息返回false;getmessage函数会阻塞:没有消息时会等待
            // 处理消息
            CharacterMgr::GetInstance()->on_input(msg);
        }

        std::chrono::steady_clock::time_point frame_start = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = std::chrono::duration<float>(frame_start - last_tick);     // 计算从上一帧到这一帧经过的时间

        // 处理更新
        float scaled_delta = BulletTimeMgr::GetInstance()->on_update(delta.count());                    // 将子弹时间管理器缩放后的时间作为角色管理器更新所需的时间
        CharacterMgr::GetInstance()->on_update(scaled_delta);// count()把chrono时间对象提取成普通的float数字
        CollisionMgr::GetInstance()->ProcessCollide();

        setbkcolor(RGB(0, 0, 0));                            // 设置绘图背景色为黑色
        cleardevice();                                       // 用当前背景颜色清空绘图设备;效果:整个窗口变成黑色,清除上一帧的所有内容

        // 处理绘图
        draw_backgroud();
        CharacterMgr::GetInstance()->on_render();
        if(is_render_collision_box)
            CollisionMgr::GetInstance()->OnDebugRender();
        draw_remain_hp();

        FlushBatchDraw();                                    // 将后台缓冲区的内容一次性显示到屏幕
    
        // 帧率控制逻辑
        last_tick = frame_start;
        std::chrono::nanoseconds sleep_duration = frame_duration - (std::chrono::steady_clock::now() - frame_start); // 先计算当前帧已经执行了多长时间,再计算还需要休眠多少时间才能达到目标帧间隔
        if (sleep_duration > std::chrono::nanoseconds(0)) {
            std::this_thread::sleep_for(sleep_duration);
        }
    }

    EndBatchDraw();                                          // 结束批量绘图模式的函数,它会将缓冲区中所有未完成的绘图操作一次性输出到屏幕上
    closegraph();

    return 0;
}

/*
    1、项目的生成后事件:xcopy /y /e /i /d "$(ProjectDir)resources" "$(OutDir)resources\"
    (1)/e - 复制所有子目录（包括空目录）
    (1)/i - 如果目标不存在，假定目标是目录并创建
    (1)/y - 自动覆盖已有文件
    (1)/d - 只复制源文件比目标文件新的文件;新增文件会被复制;修改过的文件会被复制;未修改的文件跳过
*/