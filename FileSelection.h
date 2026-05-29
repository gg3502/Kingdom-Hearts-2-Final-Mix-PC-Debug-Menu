#pragma once
//#include "YS.hpp"
#include "YW.hpp"

struct FileSelectionWidget : YW::YW_WINDOW_39
{
	std::wstring CurrentFolder;
	std::vector<bool> IsDirectoryList;
	void (*Open)(const std::wstring& folder);
	void (*Callback)(const char* fullPath);
	const char** FileList;
	int FileListNum;
	int Page;
	int PageNum;
	char* Buff;
	const char** DrawFileList;
	YW::YW_LIST_0* List;
	YW::FRAME* Frame;
	YW::BUTTON_3* Button;
	YW::TEXT* Text;
};
