#ifndef SVKMLEXPORTWINDOWI_H
#define SVKMLEXPORTWINDOWI_H

#include <QList>

class QString;
class QXmlStreamWriter;

/// Window that support KML for street view.
class SVKmlExportWindowI
{
public:
	virtual ~SVKmlExportWindowI() {}

	virtual bool checkKmlExportCondition(const QString &zonename) const = 0;
	virtual bool exportKMLHeader(QXmlStreamWriter& writer, const QString& zonename) = 0;
	virtual bool exportKMLFooter(QXmlStreamWriter& writer, const QString& zonename) = 0;
	virtual bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const QString& zonename, bool oneShot) = 0;
	virtual QList<QString> contourDrawingZones() = 0;
};

#endif // SVKMLEXPORTWINDOWI_H
