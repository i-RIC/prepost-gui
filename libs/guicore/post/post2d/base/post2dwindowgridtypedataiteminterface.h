#ifndef POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H
#define POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H

class ModifyCommandDialog;

class Post2dWindowGridTypeDataItemInterface
{
public:
	virtual ~Post2dWindowGridTypeDataItemInterface() {};
	virtual ModifyCommandDialog* createApplyColorMapSettingDialog(const std::string& name, QWidget* parent) = 0;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEMINTERFACE_H
