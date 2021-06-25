#ifndef GRIDCREATINGCONDITIONEXTERNALPROGRAM_H
#define GRIDCREATINGCONDITIONEXTERNALPROGRAM_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>

#include <QLocale>
#include <QPoint>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkStructuredGrid.h>

class SolverDefinition;
class QMenu;

class GridCreatingConditionExternalProgram : public GridCreatingCondition
{
	Q_OBJECT

public:
	GridCreatingConditionExternalProgram(const QString& folderName, const QLocale& locale, ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionExternalProgram();

	bool create(QWidget* parent) override;
	bool ready() const override;
	void clear() override;
	void showInitialDialog() override;

private slots:
	void cancel() {m_canceled = true;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override {}
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;

	SolverDefinition* m_definition;
	std::string m_name;
	QLocale m_locale;
	bool m_canceled;

	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;
};

#endif // GRIDCREATINGCONDITIONEXTERNALPROGRAM_H
