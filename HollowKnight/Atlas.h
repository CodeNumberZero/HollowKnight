#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <graphics.h>

// 图集类，负责加载和管理一系列相关的图片资源
class Atlas
{
private:
	//std::vector<IMAGE> img_list;                       // 存储图集中的图片
	std::vector<std::shared_ptr<IMAGE>> img_list;

public:
	Atlas() = default;
	~Atlas() = default;

	void load(LPCTSTR path_template, int num);
	void clear();
	int get_size() const;                               // 获取图集中图片的数量
	std::shared_ptr<IMAGE> get_image(int idx);	        // 获取指定索引的图片，返回指向IMAGE对象的指针
	//void add_image(const IMAGE& img);                 // 向图集中添加一张图片
	void add_image(const std::shared_ptr<IMAGE>& img);
};