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
	CompositeContainer({&visible, &orientation, &quadrant, &numberOfLabels, &width, &height, &positionX, &positionY, &labelFormat, &titleTextSetting, &labelTextSetting}),
	visible {"visible", true},
	orientation {"orientation", Orientation::Vertical},
	quadrant {"quadrant", Quadrant::None},
	numberOfLabels {"numberOfLabels", DEFAULT_NUMOFLABELS},
	width {"width", 0.1},
	height {"height", 0.3},
	positionX {"positionX", 0.8},
	positionY {"positionY", 0.1},
	labelFormat {"labelFormat", DEFAULT_LABELFORMAT}
{
	titleTextSetting.addPrefix("titleText");
	labelTextSetting.addPrefix("labelText");
}

ScalarBarSetting::ScalarBarSetting(const ScalarBarSetting& setting) :
	ScalarBarSetting {}
{
	CompositeContainer::copyValue(setting);
}

ScalarBarSetting::~ScalarBarSetting()
{}

ScalarBarSetting& ScalarBarSetting::operator=(const ScalarBarSetting& setting)
{
	CompositeContainer::copyValue(dynamic_cast<const ScalarBarSetting&> (setting));
	return *this;
}

XmlAttributeContainer& ScalarBarSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ScalarBarSetting&>(c));
}

void ScalarBarSetting::initForLegendBox()
{
	width = 0.3;
	height = 0.25;
	positionX = 0.65;
	positionY = 0.1;
}

std::set<ScalarBarSetting::Quadrant> ScalarBarSetting::getQuadrantSet()
{
	std::set<ScalarBarSetting::Quadrant> quads = {
		ScalarBarSetting::Quadrant::RightLower,
		ScalarBarSetting::Quadrant::LeftLower,
		ScalarBarSetting::Quadrant::LeftUpper,
		ScalarBarSetting::Quadrant::RightUpper
	};
	return quads;
}

void ScalarBarSetting::setDefaultPosition(Quadrant quad)
{
	switch (quad) {
	case ScalarBarSetting::Quadrant::RightLower:
		positionX = 0.8;
		positionY = 0.1;
		break;
	case ScalarBarSetting::Quadrant::LeftLower:
		positionX = 0.1;
		positionY = 0.1;
		break;
	case ScalarBarSetting::Quadrant::LeftUpper:
		positionX = 0.1;
		positionY = 0.6;
		break;
	case ScalarBarSetting::Quadrant::RightUpper:
		positionX = 0.8;
		positionY = 0.6;
		break;
	}
	quadrant = quad;
}

double ScalarBarSetting::distanceFromDefault(Quadrant quadrant)
{
	ScalarBarSetting def;
	def.setDefaultPosition(quadrant);
	return ::sqrt(::pow((positionX - def.positionX), 2) + ::pow((positionY - def.positionY), 2));
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

void ScalarBarSetting::loadFromRepresentation(vtkLegendBoxRepresentation* rep)
{
	double* pos  = rep->GetPosition();
	double* pos2 = rep->GetPosition2();

	positionX = *(pos);
	positionY = *(pos + 1);

	width = *(pos2);
	height = *(pos2 + 1);
}

void ScalarBarSetting::saveToRepresentation(vtkLegendBoxRepresentation* rep)
{
	rep->SetPosition(positionX, positionY);
	rep->SetPosition2(width, height);
}
