#ifndef BACKGROUNDCOLOREDITINTERFACE_H
#define BACKGROUNDCOLOREDITINTERFACE_H

#include "../guicore_global.h"

class QColor;
class QWidget;

class GUICOREDLL_EXPORT BackgroundColorEditInterface
{
public:
	virtual ~BackgroundColorEditInterface();

protected:
	void editBackgroundColor(QWidget* w);

private:
	virtual QColor backgroundColor() const = 0;
	virtual void setBackgroundColor(const QColor& color) = 0;

	class SetBackgroundColorCommand;
};

#endif // BACKGROUNDCOLOREDITINTERFACE_H
