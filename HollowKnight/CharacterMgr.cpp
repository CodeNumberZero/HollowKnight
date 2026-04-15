#include "BulletTimeMgr.h"
#include "CharacterMgr.h"
#include "Player.h"

std::shared_ptr<CharacterMgr> CharacterMgr::_instance = nullptr;

CharacterMgr::CharacterMgr()
{
	player = std::make_shared<Player>();
}

std::shared_ptr<CharacterMgr> CharacterMgr::GetInstance(){
	static std::once_flag flag;
	std::call_once(flag, []() {
		_instance = std::shared_ptr<CharacterMgr>(new CharacterMgr);
	});
	return _instance;
}

std::shared_ptr<Character> CharacterMgr::get_enemy()
{
	return enemy;
}

std::shared_ptr<Character> CharacterMgr::get_player()
{
	return player;
}

void CharacterMgr::on_input(const ExMessage& msg) {
	player->on_input(msg);
}

void CharacterMgr::on_update(float delta) {
	player->on_update(delta);
}

void CharacterMgr::on_render() {
	BulletTimeMgr::GetInstance()->post_process();                        // 在玩家渲染前,调用子弹时间管理器的后处理方法,这样就可以让除了玩家之外的所有内容都受到变暗效果的影响
	player->on_render();
}