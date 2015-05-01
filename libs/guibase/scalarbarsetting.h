#ifndef SCALARBARSETTING_H
#define SCALARBARSETTING_H

#include "guibase_global.h"
#include <misc/xmlattributecontainer.h>

class vtkScalarBarRepresentation;
class vtkLegendBoxRepresentation;
class QDomNode;
class QXmlStreamWriter;

#include <QString>

class GUIBASEDLL_EXPORT ScalarBarSetting : public XmlAttributeContainer
{
public:
	static const int DEFAULT_NUMOFLABELS = 8;
	static const QString DEFAULT_LABELFORMAT;
	enum Orientation {oVertical, oHorizontal};

	/// Constructor
	ScalarBarSetting();
	void initForLegendBox();
	void loadFromRepresentation(vtkScalarBarRepresentation* rep);
	void loadFromRepresentation(vtkLegendBoxRepresentation* rep);
	void saveToRepresentation(vtkScalarBarRepresentation* rep);
	void saveToRepresentation(vtkLegendBoxRepresentation* rep);

	void load(const QDomNode& node);
	void save(QXmlStreamWriter& writer) const;

	bool visible;
	Orientation orientation;
	int numberOfLabels;
	/// relative width (0 < width < 1)
	double width;
	/// relative width (0 < height < 1)
	double height;
	/// relative position (0 < positionX < 1)
	double positionX;
	/// relative position (0 < positionY < 1)
	double positionY;
	/// labelFormat
	QString labelFormat;
};

#endif // SCALARBARSETTING_H
