#ifndef POLYLINEIMPORTER_H
#define POLYLINEIMPORTER_H

#include <QObject>

#include <vector>

class QPointF;
class QWidget;

class PolyLineImporter : public QObject
{
	Q_OBJECT

public:
	static bool importData(std::vector<QPointF>* line, QPointF* offset, QWidget* w);

private:
	PolyLineImporter();
};

#endif // POLYLINEIMPORTER_H
