#include "Atlas.h"

void Atlas::load(LPCTSTR path_template, int num)                // LPCTSTR是指向"通用常量字符串"的长指针,它是Long Pointer to Constant TCHAR String的缩写。简单说它就是const TCHAR*的别名
{
	img_list.clear();											// 清空现有数据
	//img_list.resize(num);                                   
	img_list.reserve(num);                                      // 只预留空间,不构造对象
	
	TCHAR path_file[256];                                       // TCHAR是一个"马甲",如果定义了_UNICODE(表示用Unicode),TCHAR就变成wchar_t(一个占2字节的宽字符);如果没有定义_UNICODE(表示用ANSI),TCHAR就变成普通的char(一个占1字节的字符)
	for (int i = 0; i < num; i++)
	{
		// 在Unicode环境下,_stprintf_s变成swprintf_s(处理宽字符);在ANSI环境下,它变成sprintf_s(处理窄字符)
		_stprintf_s(path_file, path_template, i + 1);           // 按照path_template(路径模板)的格式,将数字i+1填充进去,生成一个完整的文件路径字符串,并安全地存入path_file缓冲区中

		auto img = std::make_shared<IMAGE>();
		//loadimage(img_list[i].get(), path_file);                // 第一个参数为保存图像的IMAGE对象指针,第二个参数为图片文件名
		loadimage(img.get(), path_file);                        // 第一个参数为保存图像的IMAGE对象指针,第二个参数为图片文件名
		img_list.push_back(img);
	}
}

void Atlas::clear() {
	img_list.clear();
}

int Atlas::get_size() const
{
	return img_list.size();
}

std::shared_ptr<IMAGE> Atlas::get_image(int idx)
{
	if (idx < 0 || idx >= img_list.size())
		return nullptr;
	return img_list[idx];
}

void Atlas::add_image(const std::shared_ptr<IMAGE>& img)
{
	img_list.push_back(img);
}

//void Atlas::add_image(const IMAGE& img)
//{
//	img_list.push_back(img);
//}
