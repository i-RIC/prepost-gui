#ifndef POST2DWINDOWGRIDTYPEDATAITEMI_H
#define POST2DWINDOWGRIDTYPEDATAITEMI_H

#include <string>

class ModifyCommandDialog;

class Post2dWindowGridTypeDataItemI
{
public:
	virtual ~Post2dWindowGridTypeDataItemI() {};
	virtual ModifyCommandDialog* createApplyColorMapSettingDialog(const std::string& name, QWidget* parent) = 0;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEMI_H
