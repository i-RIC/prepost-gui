#ifndef INPUTCONDITIONWIDGETFUNCTIONALDIALOG_H
#define INPUTCONDITIONWIDGETFUNCTIONALDIALOG_H

#include <QString>
#include <QWidget>
#include <QDialog>
#include <QDomNode>
#include <QList>
#include "../inputconditioncontainerfunctional.h"
#include <qwt_plot_curve.h>

#include "ui_inputconditionwidgetfunctionaldialog.h"

class QStandardItemModel;
class QItemSelectionModel;
class SolverDefinitionTranslator;
class QStyledItemDelegate;
class QwtArrayData;
class QwtPlotCurve;

class InputConditionWidgetFunctionalDialog : public QDialog
{
	Q_OBJECT

private:
	enum AxisSetting {asLeft, asRight, asNone};

public:
	InputConditionWidgetFunctionalDialog(QDomNode node, const SolverDefinitionTranslator& t, QWidget* parent = 0);
	~InputConditionWidgetFunctionalDialog();
	const InputConditionContainerFunctional& container() const;
	void setData(const InputConditionContainerFunctional& c);

public slots:
	void accept() override;

private slots:
	void clear();
	void importFromCsv();
	void exportToCsv();
	void selectionChange(const QItemSelection& selected, const QItemSelection& deselected);
	void removeSelected();
	void add();
	void sort();
	void updateGraph();

private:
	void setupData();
	void setupModel(QDomNode node, const SolverDefinitionTranslator& t);
	void setupViews();
	void setupConnections();
	void saveModel();
	void clearGraphData();

	static void setInt(const QVariant& v, QVariant& target);
	static void setDouble(const QVariant& v, QVariant& target);

private:
	static const int defaultRowHeight = 26;

	void (*m_paramfunc)(const QVariant&, QVariant&);
	QList<void (*)(const QVariant&, QVariant&)> m_valuefuncs;
	QStyledItemDelegate* tableViewDelegate;
	QList<QwtPlotCurve*> m_graphCurves;
	QList<QwtArrayData*> m_graphDatas;
	QList<AxisSetting> m_axisSettings;
	QList<QString> m_valueCaptions;
	QList<bool> m_valueIsSteps;
	QList<bool> m_axisReverses;

	InputConditionContainerFunctional m_container;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;
	Ui::InputConditionWidgetFunctionalDialog ui;

	bool m_preventGraph;
	bool m_preventSort;
};

#endif // INPUTCONDITIONWIDGETFUNCTIONALDIALOG_H
