#include "backgroundcoloreditvtki.h"
#include "../datamodel/vtkgraphicsview.h"

#include <guibase/colortool.h>

#include <QColor>

#include <vtkRenderer.h>

BackgroundColorEditVtkI::~BackgroundColorEditVtkI()
{}

QColor BackgroundColorEditVtkI::backgroundColor() const
{
	double vtkColor[3];
	QColor qColor;
	viewForBackgroundColor()->mainRenderer()->GetBackground(vtkColor);
	iRIC::VTKColorToQColor(vtkColor, qColor);
	return qColor;
}

void BackgroundColorEditVtkI::setBackgroundColor(const QColor& color)
{
	double vtkColor[3];
	iRIC::QColorToVTKColor(color, vtkColor);
	auto v = viewForBackgroundColor();
	v->mainRenderer()->SetBackground(vtkColor);
	v->update();
}
