#ifndef BACKGROUNDCOLOREDITVTKI_H
#define BACKGROUNDCOLOREDITVTKI_H

#include "../guicore_global.h"

#include "backgroundcolorediti.h"

class VTKGraphicsView;

class GUICOREDLL_EXPORT BackgroundColorEditVtkI : public BackgroundColorEditI
{
public:
	virtual ~BackgroundColorEditVtkI();

	QColor backgroundColor() const override;
	void setBackgroundColor(const QColor& color) override;

private:
	virtual VTKGraphicsView* viewForBackgroundColor() const = 0;
};

#endif // BACKGROUNDCOLOREDITVTKI_H
