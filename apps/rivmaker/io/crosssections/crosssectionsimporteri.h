#ifndef CROSSSECTIONSIMPORTERI_H
#define CROSSSECTIONSIMPORTERI_H

#include <vector>

class CrossSection;

class QPointF;
class QString;
class QStringList;
class QWidget;

class CrossSectionsImporterI
{
public:
	virtual ~CrossSectionsImporterI() {}

	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;
	virtual bool importData(std::vector<CrossSection*>* crossSections, QPointF* offset, const QString& filename, QWidget* w) = 0;
};

#endif // CROSSSECTIONSIMPORTERI_H
