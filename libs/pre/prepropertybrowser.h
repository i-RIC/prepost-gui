#ifndef PREPROPERTYBROWSER_H
#define PREPROPERTYBROWSER_H

#include <guicore/base/propertybrowser.h>

class PrePropertyBrowser : public PropertyBrowser
{
	Q_OBJECT

public:
	PrePropertyBrowser(QWidget* parent);
	~PrePropertyBrowser();
};

#endif // PREPROPERTYBROWSER_H
