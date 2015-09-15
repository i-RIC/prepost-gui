#include "scalarbarsetting.h"
#include "vtklegendboxrepresentation.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkScalarBarActor.h>
#include <vtkScalarBarRepresentation.h>

const int ScalarBarSetting::DEFAULT_NUMOFLABELS = 8;
const QString ScalarBarSetting::DEFAULT_LABELFORMAT = "%-#6.3g";

ScalarBarSetting::ScalarBarSetting() :
	CompositeContainer({&visible, &orientation, &numberOfLabels, &width, &height, &positionX, &positionY, &labelFormat}),
	visible {"visible", true},
	orientation {"orientation", Orientation::Vertical},
	numberOfLabels {"numberOfLabels", DEFAULT_NUMOFLABELS},
	width {"width", 0.1},
	height {"height", 0.3},
	positionX {"positionX", 0.8},
	positionY {"positionY", 0.1},
	labelFormat {"labelFormat", DEFAULT_LABELFORMAT}
{
	setPrefix("scalarBar");
}

ScalarBarSetting::ScalarBarSetting(const ScalarBarSetting& setting) :
	ScalarBarSetting {}
{
	CompositeContainer::copyValue(setting);
}

ScalarBarSetting& ScalarBarSetting::operator=(const ScalarBarSetting& setting)
{
	CompositeContainer::copyValue(setting);
	return *this;
}

void ScalarBarSetting::initForLegendBox()
{
	width = 0.3;
	height = 0.25;
	positionX = 0.65;
	positionY = 0.1;
}

void ScalarBarSetting::loadFromRepresentation(vtkScalarBarRepresentation* rep)
{
	if (rep->GetOrientation() == 1) {
		orientation = Orientation::Vertical;
	} else {
		orientation = Orientation::Horizontal;
	}
	double* pos  = rep->GetPosition();
	double* pos2 = rep->GetPosition2();

	positionX = *(pos);
	positionY = *(pos + 1);

	width = *(pos2);
	height = *(pos2 + 1);

	labelFormat = rep->GetScalarBarActor()->GetLabelFormat();
}

void ScalarBarSetting::loadFromRepresentation(vtkLegendBoxRepresentation* rep)
{
	double* pos  = rep->GetPosition();
	double* pos2 = rep->GetPosition2();

	positionX = *(pos);
	positionY = *(pos + 1);

	width = *(pos2);
	height = *(pos2 + 1);
}

void ScalarBarSetting::saveToRepresentation(vtkScalarBarRepresentation* rep) const
{
	if (orientation == Orientation::Vertical) {
		rep->SetOrientation(1);
	} else {
		rep->SetOrientation(0);
	}
	rep->SetPosition(positionX, positionY);
	rep->SetPosition2(width, height);
	rep->GetScalarBarActor()->SetLabelFormat(iRIC::toStr(labelFormat).c_str());
}

void ScalarBarSetting::saveToRepresentation(vtkLegendBoxRepresentation* rep)
{
	rep->SetPosition(positionX, positionY);
	rep->SetPosition2(width, height);
}
