#ifndef POST2DPROPERTYBROWSER_H
#define POST2DPROPERTYBROWSER_H

#include  <guicore/base/propertybrowser.h>

class Post2dPropertyBrowser : public PropertyBrowser
{
	Q_OBJECT

public:
	/// Constructor
	Post2dPropertyBrowser(QWidget* parent) : PropertyBrowser(parent) {

	}
	/// Destructor
	~Post2dPropertyBrowser() {}
};

#endif // POST2DPROPERTYBROWSER_H
