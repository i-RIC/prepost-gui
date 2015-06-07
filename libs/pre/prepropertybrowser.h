#ifndef PREPROPERTYBROWSER_H
#define PREPROPERTYBROWSER_H

#include <guicore/base/propertybrowser.h>

class PrePropertyBrowser : public PropertyBrowser
{
	Q_OBJECT

public:
	/// Constructor
	PrePropertyBrowser(QWidget* parent) : PropertyBrowser(parent) {

	}
	/// Destructor
	~PrePropertyBrowser() {}
};

#endif // PREPROPERTYBROWSER_H
