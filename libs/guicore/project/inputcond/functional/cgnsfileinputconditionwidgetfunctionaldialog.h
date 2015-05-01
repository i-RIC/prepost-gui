#ifndef CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALDIALOG_H
#define CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALDIALOG_H

#include <QString>
#include <QWidget>
#include <QDialog>
#include <QDomNode>
#include <QList>
#include "../cgnsfileinputconditioncontainerfunctional.h"
#include <qwt_plot_curve.h>

#include "ui_cgnsfileinputconditionwidgetfunctionaldialog.h"

class QStandardItemModel;
class QItemSelectionModel;
class SolverDefinitionTranslator;
class QStyledItemDelegate;
class QwtArrayData;
class QwtPlotCurve;

class CgnsFileInputConditionWidgetFunctionalDialog : public QDialog
{
	Q_OBJECT
private:
	enum AxisSetting {asLeft, asRight, asNone};

public:
	CgnsFileInputConditionWidgetFunctionalDialog(QDomNode node, const SolverDefinitionTranslator& t, QWidget* parent = 0);
	~CgnsFileInputConditionWidgetFunctionalDialog();
	const CgnsFileInputConditionContainerFunctional& container() {
		return m_container;
	}
public:
	void setData(const CgnsFileInputConditionContainerFunctional& c);
private:
	void setupData();
	void setupModel(QDomNode node, const SolverDefinitionTranslator& t);
	void setupViews();
	void setupConnections();
	void saveModel();
	void clearGraphData();

	CgnsFileInputConditionContainerFunctional m_container;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;
	Ui::CgnsFileInputConditionWidgetFunctionalDialog ui;
public slots:
	void accept() {
		saveModel();
		emit accepted();
		hide();
	}
private slots:
	void clear();
	void import();
	void selectionChange(const QItemSelection& selected, const QItemSelection& deselected);
	void removeSelected();
	void add();
	void sort();
	void updateGraph();
private:
	static const int defaultRowHeight = 26;
	static void setInt(const QVariant& v, QVariant& target);
	static void setDouble(const QVariant& v, QVariant& target);
	void (*m_paramfunc)(const QVariant&, QVariant&);
	QList<void (*)(const QVariant&, QVariant&)> m_valuefuncs;
	QStyledItemDelegate* tableViewDelegate;
	QList<QwtPlotCurve*> m_graphCurves;
	QList<QwtArrayData*> m_graphDatas;
	QList<AxisSetting> m_axisSettings;
	QList<QString> m_valueCaptions;
	QList<bool> m_valueIsSteps;
	QList<bool> m_axisReverses;

	bool m_preventGraph;
	bool m_preventSort;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALDIALOG_H
