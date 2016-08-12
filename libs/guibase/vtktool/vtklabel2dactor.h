#ifndef VTKLABEL2DACTOR_H
#define VTKLABEL2DACTOR_H

#include "../guibase_global.h"

#include <string>

class QPointF;

class vtkActor2D;
class vtkTextProperty;

class GUIBASEDLL_EXPORT vtkLabel2DActor
{
public:
	enum LabelPosition {
		lpMiddleLeft,
		lpMiddleRight,
		lpTopCenter,
		lpBottomCenter
	};

	vtkLabel2DActor();
	~vtkLabel2DActor();

	vtkActor2D* actor() const;
	vtkTextProperty* labelTextProperty() const;

	QPointF position() const;
	void setPosition(const QPointF& position);

	LabelPosition labelPosition() const;
	void setLabelPosition(LabelPosition position);

	std::string label() const;
	void setLabel(const std::string& label);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/vtklabel2dactor_impl.h"
#endif // _DEBUG

#endif // VTKLABEL2DACTOR_H
