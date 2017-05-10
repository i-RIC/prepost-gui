#ifndef POLYLINEEXPORTER_H
#define POLYLINEEXPORTER_H

#include <QObject>

#include <vector>

class PolyLineExporter : public QObject
{
	Q_OBJECT

public:
	static bool exportData(const std::vector<QPointF>& line, const QPointF& offset, QWidget* w);

private:
	PolyLineExporter();
};

#endif // POLYLINEEXPORTER_H
