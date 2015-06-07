#ifndef GRIDEXPORTERFACTORY_H
#define GRIDEXPORTERFACTORY_H

#include "../pre_global.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QObject>
#include <QList>

class GridExporterInterface;

class PREDLL_EXPORT GridExporterFactory : public QObject
{

public:
	~GridExporterFactory();
	const QList<GridExporterInterface*> list(SolverDefinitionGridType::GridType gt) const;
	static void init();
	static GridExporterFactory& instance() {return *m_instance;}

private:
	// Constructor is private
	GridExporterFactory();
	QList<GridExporterInterface*> m_exporterList;

	static GridExporterFactory* m_instance;
};

#endif // GRIDEXPORTERFACTORY_H
