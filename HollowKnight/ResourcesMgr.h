#pragma once
#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "Atlas.h"
#include "config.h"
#include "util.h"

// 资源管理器类,使用单例模式
class ResourcesMgr
{
private:
	// 默认构造设为保护，禁用拷贝构造和拷贝复制
	ResourcesMgr() = default;
	ResourcesMgr(const ResourcesMgr&) = delete;
	ResourcesMgr& operator=(const ResourcesMgr&) = delete;

	static std::shared_ptr<ResourcesMgr> _instance; 
	std::unordered_map<std::string, std::shared_ptr<Atlas>> atlas_pool; // 使用字符串当作资源的ID来映射到对应的资源对象;该资源池对应散装的图片素材,即一副图片只有一个动作这种素材,要形成连续动作要加载多张图片
	std::unordered_map<std::string, std::shared_ptr<IMAGE>> image_pool; // 该资源池对应连续的图像素材,即一幅图片有多个动作这种素材

	// 由于对动画序列帧的左右翻转是一个资源加载阶段的处理行为,所以相关逻辑封装为私有方法
	void flip_image(std::shared_ptr<IMAGE> src_image, std::shared_ptr<IMAGE> dst_image, int num_h = 1);     // 水平翻转图像,如果num_h>1则表示src_image是一个水平排列的图集,需要对图集中的每一帧进行翻转
	void flip_image(const std::string& src_id, const std::string& dst_id, int num_h = 1);
	void flip_atlas(const std::string& src_id, const std::string& dst_id);

public:
	~ResourcesMgr() = default;                                         // 析构也可以设置为私有。但是如果设为私有，子类无法调用，就需要使用辅助类作为删除器来进行析构
	static std::shared_ptr<ResourcesMgr> GetInstance();
	void load();
	std::shared_ptr<Atlas> find_atlas(const std::string& id) const;    // 不同的图片素材都提供了查找接口;根据id查找对用的素材
	std::shared_ptr<IMAGE> find_image(const std::string& id) const;
};


