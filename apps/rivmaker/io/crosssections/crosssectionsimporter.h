#ifndef CROSSSECTIONSIMPORTER_H
#define CROSSSECTIONSIMPORTER_H

#include <QObject>

class CrossSection;

class CrossSectionsImporter : public QObject
{
	Q_OBJECT

public:
	static bool importData(std::vector<CrossSection*>* sections, QPointF* offset, QWidget* w);

private:
	CrossSectionsImporter();
};

#endif // CROSSSECTIONSIMPORTER_H
