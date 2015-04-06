#ifndef GRIDCREATINGCONDITIONGRIDCOMBINE_H
#define GRIDCREATINGCONDITIONGRIDCOMBINE_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include "gridcreatingconditiongridcombinesettingdialog.h"

class QMenu;

class GridCreatingConditionGridCombine : public GridCreatingCondition
{
	Q_OBJECT
public:
	GridCreatingConditionGridCombine(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionGridCombine();
	bool create(QWidget *parent);
	bool ready() const;
	void setupMenu();
	void showDialog(QWidget *parent);
	void handleDialogAccepted(QDialog *d);
	void setupParameters();
	void clear();
protected:
	// @todo not implemented yet.
	void doLoadFromProjectMainFile(const QDomNode& node);
	// @todo not implemented yet.
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void loadGridCombineFromProjectMainFile(const QDomNode& node);
	void saveGridCombineToProjectMainFile(QXmlStreamWriter& writer);

	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	// parameters
	GridCreatingConditionGridCombineSettingDialog::Type j_conf;
	int j_m1;
	int j_m2;
	int j_t1;
	int j_t2;
	int i_t1;
	int i_t2;
	double p1_x;
	double p1_y;
	double p2_x;
	double p2_y;
	double alpha;
	double beta;

	double** x8;
	double** y8;
	double** z8;
	int** obst;

	Grid* mainstreamGrid;
	Grid* tributaryGrid;
	QString mainstreamGridName;
	QString tributaryGridName;
	int m_iMax;
	int m_jMax;

	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;
	bool m_isAccepted;
};

#endif // GRIDCREATINGCONDITIONGRIDCOMBINE_H
