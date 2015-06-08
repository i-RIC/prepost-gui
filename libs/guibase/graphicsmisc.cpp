#include "graphicsmisc.h"

#include <QImage>
#include <QPixmap>

#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkProperty2D.h>
#include <vtkScalarBarActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkUnsignedCharArray.h>

namespace iRIC
{

	QPixmap pixmapFromVTKImage(QSize size, vtkImageData* image)
	{
		vtkUnsignedCharArray* arr = vtkUnsignedCharArray::SafeDownCast(image->GetPointData()->GetScalars());

		int pixelcount = (size.width() + 1) * size.height();
		int rgbsize = pixelcount * 3;
		unsigned char* buffer = new unsigned char[rgbsize];
		int linebits = (size.width() * 3 + 3) / 4 * 4;
		for (int j = 0; j < size.height(); ++j) {
			for (int i = 0; i < size.width(); ++i) {
				int myOffset  = i * 3 + (size.height() - j - 1) * linebits;
				int vtkOffset = (i + j * size.width()) * 3;
				*(buffer + myOffset) = arr->GetValue(vtkOffset);             // R
				*(buffer + myOffset + 1) = arr->GetValue(vtkOffset + 1); // G
				*(buffer + myOffset + 2) = arr->GetValue(vtkOffset + 2); // B
			}
		}
		QImage img(buffer, size.width(), size.height(), QImage::Format_RGB888);
		QPixmap pixmap = QPixmap::fromImage(img);
		delete[] buffer;
		return pixmap;
	}
	void fattenBounds(double bounds[6], double rate)
	{
		double width;
		width = bounds[1] - bounds[0];
		bounds[0] -= width * rate;
		bounds[1] += width * rate;

		width = bounds[3] - bounds[2];
		bounds[2] -= width * rate;
		bounds[3] += width * rate;

		width = bounds[5] - bounds[4];
		bounds[4] -= width * rate;
		bounds[5] += width * rate;
	}
	void setupCubeActor2D(vtkCubeAxesActor2D* actor)
	{
		actor->SetFlyModeToClosestTriad();
		actor->GetProperty()->SetColor(0, 0, 0);
		vtkTextProperty* prop = actor->GetAxisLabelTextProperty();
		prop->SetFontFamilyToArial();
		prop->ItalicOff();
		prop->BoldOff();
		prop->SetFontSize(20);
		prop->ShadowOff();
		prop->SetColor(0, 0, 0);

		prop = actor->GetAxisTitleTextProperty();
		prop->SetFontFamilyToArial();
		prop->ItalicOff();
		prop->BoldOff();
		prop->SetFontSize(20);
		prop->ShadowOff();
		prop->SetColor(0, 0, 0);

		actor->SetNumberOfLabels(5);
	}

	void setupAxesActor(vtkAxesActor* actor)
	{
		actor->SetShaftTypeToLine();
		actor->SetConeResolution(4);
		actor->SetTotalLength(1.5, 1.5, 1.5);
		actor->SetNormalizedShaftLength(0.75, 0.75, 0.75);
		actor->SetNormalizedTipLength(0.25, 0.25, 0.25);

		vtkTextProperty* prop;
		prop = actor->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty();
		prop->SetColor(0, 0, 0);
		prop->BoldOff();
		prop->ItalicOff();
		prop->ShadowOff();
		prop->SetFontFamilyToArial();

		prop = actor->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty();
		prop->SetColor(0, 0, 0);
		prop->BoldOff();
		prop->ItalicOff();
		prop->ShadowOff();
		prop->SetFontFamilyToArial();

		prop = actor->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty();
		prop->SetColor(0, 0, 0);
		prop->BoldOff();
		prop->ItalicOff();
		prop->ShadowOff();
		prop->SetFontFamilyToArial();
	}

	void setupScalarBarProperty(vtkScalarBarActor* actor)
	{
		vtkTextProperty* prop = actor->GetTitleTextProperty();
		prop->SetFontFamilyToArial();
		prop->ItalicOff();
		prop->BoldOff();
		prop->ShadowOff();
		prop->SetColor(0, 0, 0);

		prop = actor->GetLabelTextProperty();
		prop->SetFontFamilyToArial();
		prop->ItalicOff();
		prop->BoldOff();
		prop->ShadowOff();
		prop->SetColor(0, 0, 0);
	}
}
