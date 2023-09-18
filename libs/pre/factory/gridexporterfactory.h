#ifndef GRIDEXPORTERFACTORY_H
#define GRIDEXPORTERFACTORY_H

#include "../pre_global.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QObject>

#include <vector>

class GridExporterI;

class PREDLL_EXPORT GridExporterFactory : public QObject
{

public:
	~GridExporterFactory();

	const std::vector<GridExporterI*> listForGridType(SolverDefinitionGridType::GridType gt) const;

	static void init();
	static GridExporterFactory& instance();

private:
	GridExporterFactory();

	std::vector<GridExporterI*> m_exporterList;
	static GridExporterFactory* m_instance;
};

#endif // GRIDEXPORTERFACTORY_H
