#ifndef POST2DWINDOWGRAPHSETTINGDIALOG_IMPL_H
#define POST2DWINDOWGRAPHSETTINGDIALOG_IMPL_H

#include "../post2dwindowgraphsettingdialog.h"

#include "../post2dwindowgraphsetting.h"

class Post2dWindowGraphSettingDialog::Impl
{
public:
	int m_dimI;
	int m_dimJ;

	Post2dWindowGraphSetting m_setting;
};

#endif // POST2DWINDOWGRAPHSETTINGDIALOG_IMPL_H
