#ifndef GRIDCREATINGCONDITIONCREATOREXTERNALPROGRAM_H
#define GRIDCREATINGCONDITIONCREATOREXTERNALPROGRAM_H

#include "gcc_externalprogram_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

#include <QString>
#include <QList>

class QWidget;

class GCC_EXTERNALPROGRAM_EXPORT GridCreatingConditionCreatorExternalProgram : public GridCreatingConditionCreator
{
	Q_OBJECT
public:
	GridCreatingConditionCreatorExternalProgram(const QString& folderName, const QLocale& locale);
	~GridCreatingConditionCreatorExternalProgram() {}
	/// Returns Grid Type.
	SolverDefinitionGridType::GridType gridType() const {return m_gridType;}
	/// Create grid creating condition instance.
	GridCreatingCondition* create(ProjectDataItem* parent);
	/// Folder Name
	const QString& folderName() {return m_folderName;}
	// get the list of creators.
	static QList<GridCreatingConditionCreator*> getList(const QLocale& locale, QWidget* mainWindow);

private:
	QString m_folderName;
	SolverDefinitionGridType::GridType m_gridType;
};

#endif // GRIDCREATINGCONDITIONCREATOREXTERNALPROGRAM_H
