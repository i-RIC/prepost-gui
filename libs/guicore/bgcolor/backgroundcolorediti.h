#ifndef BACKGROUNDCOLOREDITI_H
#define BACKGROUNDCOLOREDITI_H

#include "../guicore_global.h"

class QColor;
class QWidget;

class GUICOREDLL_EXPORT BackgroundColorEditI
{
public:
	virtual ~BackgroundColorEditI();

protected:
	void editBackgroundColor(QWidget* w);

private:
	virtual QColor backgroundColor() const = 0;
	virtual void setBackgroundColor(const QColor& color) = 0;

	class SetBackgroundColorCommand;
};

#endif // BACKGROUNDCOLOREDITI_H
