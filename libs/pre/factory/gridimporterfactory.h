#ifndef GRIDIMPORTERFACTORY_H
#define GRIDIMPORTERFACTORY_H

#include "../pre_global.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QObject>
#include <QList>

class GridImporterInterface;

class PREDLL_EXPORT GridImporterFactory : public QObject
{
public:
	virtual ~GridImporterFactory();
	const QList<GridImporterInterface*> list(SolverDefinitionGridType::GridType gt) const;
	static void init();
	static GridImporterFactory& instance(){return *m_instance;}

private:
	// Constructor is private
	GridImporterFactory();
	QList<GridImporterInterface*> m_importerList;

	static GridImporterFactory* m_instance;
};

#endif // GRIDIMPORTERFACTORY_H
