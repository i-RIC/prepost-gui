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
	bool create(QWidget* parent);
	bool ready() const;
	void clear();
	void showInitialDialog();
private slots:
	void cancel(){m_canceled = true;}
protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/){}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/){}
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
private:
	static int readErrorCode(int fn);
	void deleteGrid(const QString& fname);
	QString m_name;
	QLocale m_locale;
	SolverDefinition* m_definition;
	bool m_canceled;

	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;
};

#endif // GRIDCREATINGCONDITIONEXTERNALPROGRAM_H
