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
	~GridCreatingConditionCreatorExternalProgram();

	SolverDefinitionGridType::GridType gridType() const;
	GridCreatingCondition* create(ProjectDataItem* parent) override;
	const QString& folderName() const;

	static QList<GridCreatingConditionCreator*> getList(const QLocale& locale, QWidget* mainWindow);

private:
	static void addToList(const QString& dirName, const QLocale& locale, QWidget* w, QList<GridCreatingConditionCreator*>* list);

	QString m_folderName;
	SolverDefinitionGridType::GridType m_gridType;
};

#endif // GRIDCREATINGCONDITIONCREATOREXTERNALPROGRAM_H
