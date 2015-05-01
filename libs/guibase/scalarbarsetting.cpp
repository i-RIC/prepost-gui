#include "scalarbarsetting.h"
#include "vtklegendboxrepresentation.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkScalarBarActor.h>
#include <vtkScalarBarRepresentation.h>

const QString ScalarBarSetting::DEFAULT_LABELFORMAT = "%-#6.3g";

ScalarBarSetting::ScalarBarSetting()
{
	visible = true;
	orientation = oVertical;
	numberOfLabels = DEFAULT_NUMOFLABELS;
	width = 0.1;
	height = 0.3;
	positionX = 0.8;
	positionY = 0.1;
	labelFormat = DEFAULT_LABELFORMAT;

	m_prefix = "scalarBar";
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
		orientation = oVertical;
	} else {
		orientation = oHorizontal;
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

void ScalarBarSetting::saveToRepresentation(vtkScalarBarRepresentation* rep)
{
	if (orientation == oVertical) {
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

void ScalarBarSetting::load(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	visible = iRIC::getBooleanAttribute(node, attName("visible"));
	orientation = static_cast<Orientation>(iRIC::getIntAttribute(node, attName("orientation")));
	numberOfLabels = iRIC::getIntAttribute(node, attName("numberOfLabels"));
	width = iRIC::getDoubleAttribute(node, attName("width"));
	height = iRIC::getDoubleAttribute(node, attName("height"));
	positionX = iRIC::getDoubleAttribute(node, attName("positionX"));
	positionY = iRIC::getDoubleAttribute(node, attName("positionY"));
	labelFormat = elem.attribute(attName("labelFormat"), DEFAULT_LABELFORMAT);
}

void ScalarBarSetting::save(QXmlStreamWriter& writer) const
{
	iRIC::setBooleanAttribute(writer, attName("visible"), visible);
	iRIC::setIntAttribute(writer, attName("orientation"), static_cast<int>(orientation));
	iRIC::setIntAttribute(writer, attName("numberOfLabels"), numberOfLabels);
	iRIC::setDoubleAttribute(writer, attName("width"), width);
	iRIC::setDoubleAttribute(writer, attName("height"), height);
	iRIC::setDoubleAttribute(writer, attName("positionX"), positionX);
	iRIC::setDoubleAttribute(writer, attName("positionY"), positionY);
	writer.writeAttribute(attName("labelFormat"), labelFormat);
}
