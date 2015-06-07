#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditionwidgetfunctional.h"
#include "inputconditionwidgetfunctionaldelegate.h"
#include "inputconditionwidgetfunctionaldialog.h"

#include <guibase/qwtplotcustomcurve.h>
#include <misc/errormessage.h>
#include <misc/lastiodirectory.h>
#include <misc/xmlsupport.h>

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QItemSelection>
#include <QItemSelectionRange>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QTextStream>

#include <qwt_scale_engine.h>

InputConditionWidgetFunctionalDialog::InputConditionWidgetFunctionalDialog(QDomNode node, const SolverDefinitionTranslator& t, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_preventSort = false;
	m_preventGraph = false;
	setupModel(node, t);
	setupViews();
	setupConnections();
}

InputConditionWidgetFunctionalDialog::~InputConditionWidgetFunctionalDialog()
{
	clearGraphData();
	delete tableViewDelegate;
}

void InputConditionWidgetFunctionalDialog::clearGraphData()
{
	for (auto c : m_graphCurves) {
		delete c;
	}
	m_graphCurves.clear();
}

void InputConditionWidgetFunctionalDialog::setInt(const QVariant& v, QVariant& target)
{
	target = v.toInt();
}
void InputConditionWidgetFunctionalDialog::setDouble(const QVariant& v, QVariant& target)
{
	target = v.toDouble();
}

void InputConditionWidgetFunctionalDialog::setupConnections()
{
	connect(ui.clearButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(
		m_selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(selectionChange(const QItemSelection&, const QItemSelection&)));
	connect(ui.importButton, SIGNAL(clicked()), this, SLOT(import()));
	connect(ui.addButton, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui.removeButton, SIGNAL(clicked()), this, SLOT(removeSelected()));

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(m_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateGraph()));
}
void InputConditionWidgetFunctionalDialog::setupModel(QDomNode node, const SolverDefinitionTranslator& t)
{
	int valueCount = 0;
	QDomNode valueNode = node.firstChild();
	while (! valueNode.isNull()) {
		if (valueNode.nodeName() == "Value") {++ valueCount;}
		valueNode = valueNode.nextSibling();
	}
	m_model = new QStandardItemModel(0, 1 + valueCount, this);
	QDomNode paramNode = iRIC::getChildNode(node, "Parameter");
	if (paramNode.isNull()) {
		// this item doesn't contain "Parameter" Node!.
		throw(ErrorMessage("Parameter node is not stored!"));
	}
	QDomElement paramElem = paramNode.toElement();
	QString paramCaption = t.translate(paramElem.attribute("caption"));
	QStringList leftValueCaptions, rightValueCaptions;
	m_model->setHeaderData(0, Qt::Horizontal, paramCaption);
	QString paramType = paramElem.attribute("valueType");
	if (paramType == "real") {
		m_paramfunc = InputConditionWidgetFunctionalDialog::setDouble;
	} else {
		m_paramfunc = InputConditionWidgetFunctionalDialog::setInt;
	}
	bool isAxisLog = false;
	if (paramElem.hasAttribute("axislog")) {
		isAxisLog = (paramElem.attribute("axislog") == "true");
	}
	if (isAxisLog) {
		ui.graphView->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine());
	}

	valueNode = node.firstChild();
	int valueIndex = 0;
	while (! valueNode.isNull()) {
		if (valueNode.nodeName() == "Value") {
			QDomElement valueElem = valueNode.toElement();
			QString valueCaption = t.translate(valueElem.attribute("caption"));
			m_valueCaptions.append(valueCaption);
			m_model->setHeaderData(1 + valueIndex, Qt::Horizontal, valueCaption);
			QString valueType = valueElem.attribute("valueType");
			if (valueType == "real") {
				m_valuefuncs.append(InputConditionWidgetFunctionalDialog::setDouble);
			} else {
				m_valuefuncs.append(InputConditionWidgetFunctionalDialog::setInt);
			}
			AxisSetting axisSetting;
			if (valueElem.hasAttribute("axis")) {
				QString axis = valueElem.attribute("axis");
				if (axis == "left") {axisSetting = asLeft;}
				else if (axis == "right") {axisSetting = asRight;}
			} else if (valueElem.attribute("hide") == "true") {
				axisSetting = asNone;
			} else {
				if (valueIndex == 0) {
					axisSetting = asLeft;
				} else {
					axisSetting = asRight;
				}
			}
			m_axisSettings.append(axisSetting);
			if (axisSetting == asLeft) {
				leftValueCaptions.append(valueCaption);
			} else if (axisSetting == asRight) {
				rightValueCaptions.append(valueCaption);
			}
			bool isStep = false;
			if (valueElem.hasAttribute("step")) {
				isStep = (valueElem.attribute("step") == "true");
			}
			bool isAxisLog = false;
			if (valueElem.hasAttribute("axislog")) {
				isAxisLog = (valueElem.attribute("axislog") == "true");
			}
			if (isAxisLog) {
				if (axisSetting == asLeft) {
					ui.graphView->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
				} else if (axisSetting == asRight) {
					ui.graphView->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine());
				}
			}
			m_valueIsSteps.append(isStep);
			bool isAxisReverse = false;
			if (valueElem.hasAttribute("axisreverse")) {
				isAxisReverse = (valueElem.attribute("axisreverse") == "true");
			}
			m_axisReverses.append(isAxisReverse);
			if (isAxisReverse) {
				if (axisSetting == asLeft) {
					ui.graphView->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, true);
				} else if (axisSetting == asRight) {
					ui.graphView->axisScaleEngine(QwtPlot::yRight)->setAttribute(QwtScaleEngine::Inverted, true);
				}
			}
			++ valueIndex;
		}
		valueNode = valueNode.nextSibling();
	}

	m_selectionModel = new QItemSelectionModel(m_model);

	// if data is changed, sort() is always called.
	connect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(sort()));

	ui.graphView->setAxisTitle(QwtPlot::xBottom, paramCaption);
	if (leftValueCaptions.count() > 0) {
		ui.graphView->setAxisTitle(QwtPlot::yLeft, leftValueCaptions.join(", "));
		ui.graphView->enableAxis(QwtPlot::yLeft, true);
	}
	if (rightValueCaptions.count() > 0) {
		ui.graphView->setAxisTitle(QwtPlot::yRight, rightValueCaptions.join(", "));
		ui.graphView->enableAxis(QwtPlot::yRight, true);
	}

	QFont axisFont("Helvetica", 10);
	ui.graphView->setAxisFont(QwtPlot::xBottom, axisFont);
	ui.graphView->setAxisFont(QwtPlot::yLeft, axisFont);
	ui.graphView->setAxisFont(QwtPlot::yRight, axisFont);
}

void InputConditionWidgetFunctionalDialog::setupData()
{
	clear();
	m_model->blockSignals(true);
	for (int i = 0; i < m_container.param().size(); ++i) {
		QVariant doubletmpx;
		QVariant tmpx;
		m_model->insertRow(i);
		doubletmpx = m_container.param().at(i);
		(*m_paramfunc)(doubletmpx, tmpx);
		m_model->setData(m_model->index(i, 0, QModelIndex()), tmpx);
	}
	for (int j = 0; j < m_valuefuncs.size(); ++j) {
		QVector<double>& values = m_container.value(j);
		for (int i = 0; i < values.size(); ++i) {
			QVariant doubletmpy;
			QVariant tmpy;
			doubletmpy = values.at(i);
			(*m_valuefuncs[j])(doubletmpy, tmpy);
			m_model->setData(m_model->index(i, j + 1, QModelIndex()), tmpy);
		}
	}
	m_model->blockSignals(false);
	sort();
	int rows = m_model->rowCount(QModelIndex());
	for (int i = 0; i < rows; ++i) {
		ui.tableView->setRowHeight(i, InputConditionWidgetFunctionalDialog::defaultRowHeight);
	}
	ui.removeButton->setDisabled(true);
	updateGraph();
}

void InputConditionWidgetFunctionalDialog::setupViews()
{
	ui.tableView->setModel(m_model);
	ui.tableView->setSelectionModel(m_selectionModel);
	tableViewDelegate = new InputConditionWidgetFunctionalDelegate(this);
	ui.tableView->setItemDelegate(tableViewDelegate);
}

void InputConditionWidgetFunctionalDialog::saveModel()
{
	sort();
	int rows = m_model->rowCount(QModelIndex());
	m_container.clear();
	for (int i = 0; i < rows; ++i) {
		m_container.param().push_back(m_model->data(m_model->index(i, 0, QModelIndex())).toDouble());
		for (int j = 0; j < m_valuefuncs.count(); ++j) {
			m_container.value(j).push_back(m_model->data(m_model->index(i, 1 + j, QModelIndex())).toDouble());
		}
	}
}
void InputConditionWidgetFunctionalDialog::clear()
{
	int rows = m_model->rowCount(QModelIndex());
	m_model->removeRows(0, rows, QModelIndex());
	updateGraph();
}

void InputConditionWidgetFunctionalDialog::selectionChange(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	QItemSelection selection = m_selectionModel->selection();
	bool deletable = false;
	for (auto it = selection.begin(); it != selection.end(); ++it) {
		deletable = deletable || (it->left() == 0 && it->right() == m_model->columnCount() - 1);
	}
	ui.removeButton->setDisabled(! deletable);
}

void InputConditionWidgetFunctionalDialog::removeSelected()
{
	QItemSelection selection = m_selectionModel->selection();
	for (auto it = selection.begin(); it != selection.end(); ++it) {
		if (it->left() == 0 && it->right() == m_model->columnCount() - 1) {
			// delete this.
			m_model->removeRows(it->top(), (it->bottom() - it->top() + 1));
		}
	}
	updateGraph();
}

void InputConditionWidgetFunctionalDialog::import()
{
	QString fileName;
	QString dir = LastIODirectory::get();
	fileName = QFileDialog::getOpenFileName(this, tr("Choose a text file"), dir, tr("Text files (*.csv *.txt);;All files (*.*)"));
	if (fileName.isEmpty()) {
		return;
	}
	dir = QFileInfo(fileName).absolutePath();
	LastIODirectory::set(dir);

	QFile file(fileName);
	if (! file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::critical(this, tr("Error"), tr("Error occured while opening the file."));
		return;
	}
	QTextStream stream(&file);
	QString line;
	clear();
	int row = 0;
	m_preventGraph = true;
	m_model->blockSignals(true);
	do {
		line = stream.readLine();
		if (! line.isEmpty()) {
			QVariant tmp;
			QVariant tmp2;
			m_model->insertRow(row);
			QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
			tmp = pieces.value(0);
			(*m_paramfunc)(tmp, tmp2);
			m_model->setData(m_model->index(row, 0), tmp2);
			for (int i = 0; i < m_valuefuncs.count(); ++i) {
				tmp = pieces.value(i + 1);
				(*m_valuefuncs[i])(tmp, tmp2);
				m_model->setData(m_model->index(row, i + 1), tmp2);
			}
			row++;
		}
	} while (! line.isEmpty());
	file.close();
	m_model->blockSignals(false);
	m_preventGraph = false;

	int rows = m_model->rowCount();
	for (int i = 0; i < rows; ++i) {
		ui.tableView->setRowHeight(i, InputConditionWidgetFunctionalDialog::defaultRowHeight);
	}
	sort();
}

void InputConditionWidgetFunctionalDialog::add()
{
	int lastrow = m_model->rowCount();
	m_model->insertRow(lastrow);
	ui.tableView->setRowHeight(lastrow, InputConditionWidgetFunctionalDialog::defaultRowHeight);
	QVariant param, val;
	QVariant tmpparam, tmpval;
	if (lastrow == 0) {
		param = 0;
	} else {
		param = m_model->data(m_model->index(lastrow - 1, 0)).toDouble() + 1;
	}
	val = 0;
	(*m_paramfunc)(param, tmpparam);
	m_model->setData(m_model->index(lastrow, 0, QModelIndex()), tmpparam);
	for (int i = 0; i < m_valuefuncs.count(); ++i) {
		(*m_valuefuncs[i])(val, tmpval);
		m_model->setData(m_model->index(lastrow, i + 1, QModelIndex()), tmpval);
	}
}

void InputConditionWidgetFunctionalDialog::sort()
{
	if (m_preventSort) {return;}
	m_model->sort(0);
	updateGraph();
}

void InputConditionWidgetFunctionalDialog::setData(const InputConditionContainerFunctional& c)
{
	m_container = c;
	setupData();
}

void InputConditionWidgetFunctionalDialog::updateGraph()
{
	if (m_preventGraph) {return;}

	clearGraphData();
	int dataCount = m_model->rowCount();
	double* x, *y;

	for (int j = 0; j < m_axisSettings.count(); ++j) {
		QwtPlotCurve* pc = new QwtPlotCustomCurve();
		if (m_valueIsSteps.at(j)) {
			x = new double[dataCount * 3 + 1];
			y = new double[dataCount * 3 + 1];
			double firstx = 0;
			double x0 = m_model->data(m_model->index(0, 0)).toDouble();
			if (dataCount == 1) {
				firstx = x0 - 1;
			} else if (dataCount > 0) {
				double x1 = m_model->data(m_model->index(1, 0)).toDouble();
				double firstwidth = x1 - x0;
				firstx = x0 - firstwidth;
			}
			double xstart = firstx;
			* (x) = xstart;
			* (y) = 0;
			for (int i = 0; i < dataCount; ++i) {
				double xend = m_model->data(m_model->index(i, 0)).toDouble();
				double yval =  m_model->data(m_model->index(i, j + 1)).toDouble();
				double xdelta = 0;

				* (x + i * 3 + 1) = xstart + xdelta;
				* (y + i * 3 + 1) = yval;
				* (x + i * 3 + 2) = xend - xdelta;
				* (y + i * 3 + 2) = yval;
				* (x + i * 3 + 3) = xend - xdelta;
				* (y + i * 3 + 3) = 0;
				xstart = xend;
			}
			pc->setSamples(x, y, dataCount * 3 + 1);
		} else {
			x = new double[dataCount];
			y = new double[dataCount];

			for (int i = 0; i < dataCount; ++i) {
				*(x + i) = m_model->data(m_model->index(i, 0)).toDouble();
			}
			for (int i = 0; i < dataCount; ++i) {
				*(y + i) = m_model->data(m_model->index(i, j + 1)).toDouble();
			}
			pc->setSamples(x, y, dataCount);
		}

		if (m_axisSettings[j] == asLeft) {
			pc->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
			pc->attach(ui.graphView);
		} else if (m_axisSettings[j] == asRight) {
			pc->setAxes(QwtPlot::xBottom, QwtPlot::yRight);
			pc->attach(ui.graphView);
		} else {
			// do not shown on graphs.
		}

		delete x;
		delete y;
		m_graphCurves.append(pc);
	}
	ui.graphView->replot();

}
