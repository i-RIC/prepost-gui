#ifndef BACKGROUNDCOLOREDITVTKINTERFACE_H
#define BACKGROUNDCOLOREDITVTKINTERFACE_H

#include "../guicore_global.h"

#include "backgroundcoloreditinterface.h"

class VTKGraphicsView;

class GUICOREDLL_EXPORT BackgroundColorEditVtkInterface : public BackgroundColorEditInterface
{
public:
	virtual ~BackgroundColorEditVtkInterface();

	QColor backgroundColor() const override;
	void setBackgroundColor(const QColor& color) override;

private:
	virtual VTKGraphicsView* viewForBackgroundColor() const = 0;
};

#endif // BACKGROUNDCOLOREDITVTKINTERFACE_H
