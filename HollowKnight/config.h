#pragma once
#include<graphics.h>
#include<string>

const float FLOOR_Y = 620;                          // 地板的竖直方向(游戏窗口是竖屏坐标系,Y越大越靠下,屏幕上Y=620这条水平线,就是角色能站立的地面)
const float GRAVITY = 980 * 2;                      // 重力大小

const float CD_ROLL = 0.75f;						// 翻滚冷却
const float CD_ATTACK = 0.5f;						// 攻击冷却
const float SPEED_RUN = 300.0f;						// 奔跑速度
const float SPEED_JUMP = 780.0f;					// 跳跃速度
const float SPEED_ROLL = 800.0f;					// 翻滚速度

const float SPEED_PROGRESS = 2.0f;                  // 表示进入和退出的速度
const float DST_DELTA_FACTOR = 0.35f;               // 完全进入子弹时间后帧更新时间的缩放
const float DST_COLOR_FACTOR = 0.35f;               // 完全进入子弹时间后画面色彩改变比例

const int ENEMY_HP = 15;							// 敌人血量
const float SPEED_DASH = 1500.0f;					// 敌人冲刺速度
const float SPEED_MOVE = 1250.0f;                   // 敌人释放的飞剑飞行速度

struct ImageResInfo									// 图片资源信息
{
	std::string id;
	LPCTCH path;                                    // LPCTSTR是指向"通用常量字符串"的长指针,它是Long Pointer to Constant TCHAR String的缩写。简单说它就是const TCHAR*的别名
};

struct AtlasResInfo									// 图集资源信息
{
	std::string id;
	LPCTCH path;
	int num_frame = 0;                              // 相较于图片资源的信息,图集资源的信息需要额外提供图集中图片的数量,方便后续自动加载
};


enum class AnchorMode								// 锚点模式,定义动画帧的对齐方式
{
	Centered,										// 中心对齐
	BottomCentered									// 底部中心对齐	
};


// 在游戏开发中，通常需要将不同的游戏对象分类到不同的碰撞层，以便于控制哪些对象之间可以发生碰撞(如果要添加不同的碰撞层级直接在此添加即可)
enum class CollisionLayer {							// 碰撞层:用来描述某个碰撞箱自身所处的碰撞层级或可以发生碰撞的目标层级
	None,											// 无碰撞层（不参与碰撞检测）
	Player,											// 玩家层
	Enemy,											// 敌人层
};


enum class AttackDirection {						// 玩家四个攻击方向:会影响到不同的攻击特效播放以及碰撞箱位置
	Up, Down, Left, Right
};


enum class Status {									// 子弹时间状态类,用来标注当前是正在进入子弹时间还是正在退出子弹时间
	Entering,
	Exiting
};


enum class Stage {									// 刺球的四个状态
	Idle,											// 默认的上下浮动状态
	Aim,											// 向着玩家冲刺前的准备状态
	Dash,											// 冲刺中的状态
	Break											// 破碎的状态
};