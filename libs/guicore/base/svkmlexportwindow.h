#ifndef SVKMLEXPORTWINDOW_H
#define SVKMLEXPORTWINDOW_H

#include <QList>
#include <QString>
#include <QXmlStreamWriter>

class QString;

/// Window that support KML for street view.
class SVKmlExportWindow
{
public:
	SVKmlExportWindow() {}
	virtual bool exportKMLHeader(QXmlStreamWriter& writer, const QString& zonename) = 0;
	virtual bool exportKMLFooter(QXmlStreamWriter& writer, const QString& zonename) = 0;
	virtual bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const QString& zonename) = 0;
	virtual QList<QString> contourDrawingZones() = 0;
};

#endif // SVKMLEXPORTWINDOW_H
