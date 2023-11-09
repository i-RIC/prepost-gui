#ifndef GRIDIMPORTERFACTORY_H
#define GRIDIMPORTERFACTORY_H

#include "../pre_global.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QObject>
#include <QList>

class GridImporterI;

class PREDLL_EXPORT GridImporterFactory : public QObject
{

public:
	virtual ~GridImporterFactory();
	const QList<GridImporterI*> list(const SolverDefinitionGridType& gt) const;
	static void init();
	static GridImporterFactory& instance();

private:
	// Constructor is private
	GridImporterFactory();
	QList<GridImporterI*> m_importerList;

	static GridImporterFactory* m_instance;
};

#endif // GRIDIMPORTERFACTORY_H
