#ifndef INPUTCONDITIONWIDGETFUNCTIONALDIALOG_H
#define INPUTCONDITIONWIDGETFUNCTIONALDIALOG_H

#include "../inputconditioncontainerfunctional.h"
#include "../../colorsource.h"

#include <QDateTime>
#include <QDialog>
#include <QDomNode>
#include <QString>
#include <QWidget>

#include <qwt_plot_curve.h>

#include <vector>

namespace Ui
{
	class InputConditionWidgetFunctionalDialog;
}

class SolverDefinitionTranslator;

class QStandardItemModel;
class QItemSelection;
class QItemSelectionModel;
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

	bool checkImportSourceUpdate();
	void toggleReadOnly(bool readonly);

public slots:
	void accept() override;

private slots:
	void clear();
	void importFromCsv();
	void importFromWeb();
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

	bool importFromCsv(const QString& fileName);

	void setupXYStandard(int row, std::vector<double>* x, std::vector<double>* y);
	void setupXYStep(int row, std::vector<double>* x, std::vector<double>* y);
	void setupXYSpan(int row, std::vector<double>* x, std::vector<double>* y);

	void updateSpanColumns();
	bool checkValues();

	static void setInt(const QVariant& v, QVariant& target);
	static void setDouble(const QVariant& v, QVariant& target);

private:
	static const int defaultRowHeight = 26;

	void (*m_paramfunc)(const QVariant&, QVariant&);
	std::vector<void (*)(const QVariant&, QVariant&)> m_valuefuncs;

	QString m_paramCaption;
	bool m_paramMinIsSet;
	double m_paramMin;
	bool m_paramMaxIsSet;
	double m_paramMax;

	std::vector<QwtPlotCurve*> m_graphCurves;
	std::vector<QwtArrayData*> m_graphDatas;
	std::vector<AxisSetting> m_axisSettings;

	std::vector<QString> m_valueCaptions;
	std::vector<bool> m_valueIsSteps;
	std::vector<bool> m_valueIsSpans;
	std::vector<bool> m_axisReverses;

	std::vector<double> m_valueDefault;
	std::vector<bool> m_valueMinIsSet;
	std::vector<double> m_valueMin;
	std::vector<bool> m_valueMaxIsSet;
	std::vector<double> m_valueMax;

	InputConditionContainerFunctional m_container;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;

	bool m_preventGraph;
	bool m_preventSort;

	ColorSource m_colorSource;

	QString m_importedCsvFileName;
	QDateTime m_importedCsvLastModified;

	Ui::InputConditionWidgetFunctionalDialog* ui;

public:
	friend class InputConditionWidgetFunctionalDelegate;
};

#endif // INPUTCONDITIONWIDGETFUNCTIONALDIALOG_H
