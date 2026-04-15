#pragma once
#include "Character.h"

// 角色管理器：用来管理玩家实例
class CharacterMgr
{
private:
	CharacterMgr();
	CharacterMgr(const CharacterMgr&) = delete;
	Character& operator=(const Character&) = delete;

	static std::shared_ptr<CharacterMgr> _instance;
	std::shared_ptr<Character> enemy = nullptr;
	std::shared_ptr<Character> player = nullptr;

public:
	~CharacterMgr() = default;
	static std::shared_ptr<CharacterMgr> GetInstance();
	std::shared_ptr<Character> get_enemy();
	std::shared_ptr<Character> get_player();
	void on_input(const ExMessage& msg);
	void on_update(float delta);
	void on_render();
};

