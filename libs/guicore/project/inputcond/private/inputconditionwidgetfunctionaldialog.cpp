#include "../../../solverdef/solverdefinitiontranslator.h"
#include "inputconditionwidgetfunctional.h"
#include "inputconditionwidgetfunctionaldelegate.h"
#include "inputconditionwidgetfunctionaldialog.h"
#include "inputconditionwidgetfunctionaldownloaddialog.h"
#include "ui_inputconditionwidgetfunctionaldialog.h"

#include <guibase/overridecursorchanger.h>
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
#include <QTableView>

#include <qwt_scale_engine.h>
#include <qwt_legend.h>

namespace {

const int ERROR_MAX = 10;

void checkMinMax(QStringList* errors, int* errorCount, double value, bool minIsSet, double min, bool maxIsSet, double max, int row, const QString& caption) {
	if (minIsSet && value < min) {
		QString message = InputConditionWidgetFunctionalDialog::tr("%1 at row %2: The value %3 is smaller than minimum value %4").arg(caption).arg(row).arg(value).arg(min);
		if (errors->size() < ERROR_MAX) {
			errors->push_back(message);
		}
		++ (*errorCount);
	}
	if (maxIsSet && value > max) {
		QString message = InputConditionWidgetFunctionalDialog::tr("%1 at row %2: The value %3 is larger than maximum value %4").arg(caption).arg(row).arg(value).arg(max);
		if (errors->size() < ERROR_MAX) {
			errors->push_back(message);
		}
		++ (*errorCount);
	}
}

} // namespace

InputConditionWidgetFunctionalDialog::InputConditionWidgetFunctionalDialog(QDomNode node, const SolverDefinitionTranslator& t, QWidget* parent) :
	QDialog {parent},
	m_preventGraph {false},
	m_preventSort {false},
	m_colorSource {nullptr},
	ui {new Ui::InputConditionWidgetFunctionalDialog}
{
	ui->setupUi(this);
	setupModel(node, t);
	setupViews();
	setupConnections();
}

InputConditionWidgetFunctionalDialog::~InputConditionWidgetFunctionalDialog()
{
	clearGraphData();

	delete ui;
}

void InputConditionWidgetFunctionalDialog::clearGraphData()
{
	for (auto c : m_graphCurves) {
		delete c;
	}
	m_graphCurves.clear();
}

bool InputConditionWidgetFunctionalDialog::importFromCsv(const QString& fileName)
{
	QFile file(fileName);
	if (! file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::critical(this, tr("Error"), tr("Error occured while opening the file."));
		return false;
	}
	QTextStream stream(&file);
	QString line;
	clear();
	int row = 0;
	m_preventGraph = true;
	if (! m_preventSort) {
		m_model->blockSignals(true);
	}
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
			for (int i = 0; i < m_valuefuncs.size(); ++i) {
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
		ui->tableView->setRowHeight(i, InputConditionWidgetFunctionalDialog::defaultRowHeight);
	}
	sort();

	QFileInfo finfo(fileName);
	m_importedCsvFileName = fileName;
	m_importedCsvLastModified = finfo.lastModified();

	return true;
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
	connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(
		m_selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(selectionChange(const QItemSelection&, const QItemSelection&)));
	connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importFromCsv()));
	connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportToCsv()));
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeSelected()));

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	connect(m_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateGraph()));

	connect(ui->importWebButton, SIGNAL(clicked()), this, SLOT(importFromWeb()));
}

void InputConditionWidgetFunctionalDialog::setupModel(QDomNode node, const SolverDefinitionTranslator& t)
{
	m_preventSort = (node.toElement().attribute("noSort") == "true");

	// waterML url
	if (! node.toElement().hasAttribute("wml2:url")) {
		ui->importWebButton->hide();
	}

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
	m_paramCaption = paramCaption;

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
		ui->graphView->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine());
	}

	// param min
	m_paramMinIsSet = false;
	m_paramMin = 0;
	if (paramElem.hasAttribute("min")) {
		m_paramMinIsSet = true;
		m_paramMin = paramElem.attribute("min").toDouble();
	}

	// param max
	m_paramMaxIsSet = false;
	m_paramMax = 0;
	if (paramElem.hasAttribute("max")) {
		m_paramMaxIsSet = true;
		m_paramMax = paramElem.attribute("max").toDouble();
	}

	QStringList spanVals;

	valueNode = node.firstChild();
	int valueIndex = 0;
	while (! valueNode.isNull()) {
		if (valueNode.nodeName() == "Value") {
			QDomElement valueElem = valueNode.toElement();
			QString valueCaption = t.translate(valueElem.attribute("caption"));
			m_valueCaptions.push_back(valueCaption);
			m_model->setHeaderData(1 + valueIndex, Qt::Horizontal, valueCaption);
			QString valueType = valueElem.attribute("valueType");
			if (valueType == "real") {
				m_valuefuncs.push_back(InputConditionWidgetFunctionalDialog::setDouble);
			} else {
				m_valuefuncs.push_back(InputConditionWidgetFunctionalDialog::setInt);
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
			m_axisSettings.push_back(axisSetting);
			if (axisSetting == asLeft) {
				leftValueCaptions.append(valueCaption);
			} else if (axisSetting == asRight) {
				rightValueCaptions.push_back(valueCaption);
			}

			// step
			bool isStep = false;
			if (valueElem.hasAttribute("step")) {
				isStep = (valueElem.attribute("step") == "true");
			}
			m_valueIsSteps.push_back(isStep);

			// span
			bool isSpan = false;
			if (valueElem.hasAttribute("span")){
				isSpan = (valueElem.attribute("span") == "true");
				spanVals.push_back(valueCaption);
			}
			m_valueIsSpans.push_back(isSpan);

			// axislog
			bool isAxisLog = false;
			if (valueElem.hasAttribute("axislog")) {
				isAxisLog = (valueElem.attribute("axislog") == "true");
			}
			if (isAxisLog) {
				if (axisSetting == asLeft) {
					ui->graphView->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
				} else if (axisSetting == asRight) {
					ui->graphView->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine());
				}
			}

			// axisreverse
			bool isAxisReverse = false;
			if (valueElem.hasAttribute("axisreverse")) {
				isAxisReverse = (valueElem.attribute("axisreverse") == "true");
			}
			m_axisReverses.push_back(isAxisReverse);
			if (isAxisReverse) {
				if (axisSetting == asLeft) {
					ui->graphView->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, true);
				} else if (axisSetting == asRight) {
					ui->graphView->axisScaleEngine(QwtPlot::yRight)->setAttribute(QwtScaleEngine::Inverted, true);
				}
			}

			// default
			double valueDefault = 0;
			if (valueElem.hasAttribute("default")) {
				valueDefault = valueElem.attribute("default").toDouble();
			}
			m_valueDefault.push_back(valueDefault);

			// min
			bool valueMinIsSet = false;
			double valueMin = 0;
			if (valueElem.hasAttribute("min")) {
				valueMinIsSet = true;
				valueMin = valueElem.attribute("min").toDouble();
			}
			m_valueMinIsSet.push_back(valueMinIsSet);
			m_valueMin.push_back(valueMin);

			// max
			bool valueMaxIsSet = false;
			double valueMax = 0;
			if (valueElem.hasAttribute("max")) {
				valueMaxIsSet = true;
				valueMax = valueElem.attribute("max").toDouble();
			}
			m_valueMaxIsSet.push_back(valueMaxIsSet);
			m_valueMax.push_back(valueMax);

			++ valueIndex;
		}
		valueNode = valueNode.nextSibling();
	}

	if (spanVals.size() == 0) {
		ui->lineCommentLabel->hide();
	} else {
		QString text = tr("%1 are values for spans. For example value input at 2nd row is for span between 1st row and 2nd row.");
		ui->lineCommentLabel->setText(text.arg(spanVals.join(", ")));
	}

	m_selectionModel = new QItemSelectionModel(m_model);

	// if data is changed, sort() is always called.
	connect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(sort()));

	ui->graphView->setAxisTitle(QwtPlot::xBottom, paramCaption);
	if (leftValueCaptions.count() > 0) {
		ui->graphView->setAxisTitle(QwtPlot::yLeft, leftValueCaptions.join(", "));
		ui->graphView->enableAxis(QwtPlot::yLeft, true);
	}
	if (rightValueCaptions.count() > 0) {
		ui->graphView->setAxisTitle(QwtPlot::yRight, rightValueCaptions.join(", "));
		ui->graphView->enableAxis(QwtPlot::yRight, true);
	}

	QFont axisFont("Helvetica", 10);
	ui->graphView->setAxisFont(QwtPlot::xBottom, axisFont);
	ui->graphView->setAxisFont(QwtPlot::yLeft, axisFont);
	ui->graphView->setAxisFont(QwtPlot::yRight, axisFont);
}

void InputConditionWidgetFunctionalDialog::setupData()
{
	OverrideCursorChanger cursorChanger(Qt::WaitCursor);
	QTime time;
	time.start();
	clear();
	QSignalBlocker signalBlocker(m_model);    // speed up large datasets
	ui->tableView->setModel(nullptr);
	for (int i = 0; i < m_container.param().size(); ++i) {
		QVariant doubletmpx;
		QVariant tmpx;
		m_model->insertRow(i);
		doubletmpx = m_container.param().at(i);
		(*m_paramfunc)(doubletmpx, tmpx);
		m_model->setData(m_model->index(i, 0, QModelIndex()), tmpx);
	}
	for (int j = 0; j < m_valuefuncs.size(); ++j) {
		auto& values = m_container.value(j);
		for (int i = 0; i < values.size(); ++i) {
			QVariant doubletmpy;
			QVariant tmpy;
			doubletmpy = values.at(i);
			(*m_valuefuncs[j])(doubletmpy, tmpy);
			m_model->setData(m_model->index(i, j + 1, QModelIndex()), tmpy);
		}
	}
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);
	sort();
	int rows = m_model->rowCount(QModelIndex());
	for (int i = 0; i < rows; ++i) {
		ui->tableView->setRowHeight(i, InputConditionWidgetFunctionalDialog::defaultRowHeight);
	}
	ui->removeButton->setDisabled(true);
	updateGraph();
	qDebug("InputConditionWidgetFunctionalDialog::setupData():%d", time.elapsed());
}

void InputConditionWidgetFunctionalDialog::setupViews()
{
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);
	auto delegate = new InputConditionWidgetFunctionalDelegate(this);

	ui->tableView->setItemDelegate(delegate);
}

void InputConditionWidgetFunctionalDialog::saveModel()
{
	sort();
	int rows = m_model->rowCount(QModelIndex());
	m_container.removeAllValues();
	for (int i = 0; i < rows; ++i) {
		m_container.param().push_back(m_model->data(m_model->index(i, 0, QModelIndex())).toDouble());
		for (int j = 0; j < m_valuefuncs.size(); ++j) {
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
	ui->removeButton->setDisabled(! deletable);
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

void InputConditionWidgetFunctionalDialog::importFromCsv()
{
	QString fileName;
	QString dir = LastIODirectory::get();
	fileName = QFileDialog::getOpenFileName(this, tr("Choose a text file"), dir, tr("Text files (*.csv *.txt);;All files (*.*)"));
	if (fileName.isEmpty()) {
		return;
	}
	dir = QFileInfo(fileName).absolutePath();
	LastIODirectory::set(dir);

	importFromCsv(fileName);
}

void InputConditionWidgetFunctionalDialog::exportToCsv()
{
	QString fileName;
	QString dir = LastIODirectory::get();
	fileName = QFileDialog::getSaveFileName(this, tr("Specify file name to save"), dir, tr("CSV files (*.csv)"));
	if (fileName.isEmpty()){
		return;
	}
	dir = QFileInfo(fileName).absolutePath();
	LastIODirectory::set(dir);

	QFile file(fileName);
	if (! file.open(QFile::WriteOnly | QFile::Text)){
		QMessageBox::critical(this, tr("Error"), tr("Error occured while opening the file."));
		return;
	}
	QTextStream stream(&file);
	for (int i = 0; i < m_model->rowCount(); ++i){
		for (int j = 0; j < m_model->columnCount(); ++j) {
			if (j != 0) {stream << ",";}
			stream << m_model->data(m_model->index(i, j)).toDouble();
		}
		stream << "\n";
	}
	file.close();
}


void InputConditionWidgetFunctionalDialog::importFromWeb()
{
	InputConditionWidgetFunctionalDownloadDialog dlg(this);
	dlg.setData(container());
	int ret = dlg.exec();
	if (ret == QDialog::Accepted) {
		setData(dlg.container());
	}
}

void InputConditionWidgetFunctionalDialog::add()
{
	int lastrow = m_model->rowCount();
	m_model->insertRow(lastrow);
	ui->tableView->setRowHeight(lastrow, InputConditionWidgetFunctionalDialog::defaultRowHeight);
	QVariant param, val;
	QVariant tmpparam, tmpval;
	if (lastrow == 0) {
		param = 0;
	} else {
		param = m_model->data(m_model->index(lastrow - 1, 0)).toDouble() + 1;
	}
	(*m_paramfunc)(param, tmpparam);
	m_model->setData(m_model->index(lastrow, 0, QModelIndex()), tmpparam);
	for (int i = 0; i < m_valuefuncs.size(); ++i) {
		val = m_valueDefault[i];
		(*m_valuefuncs[i])(val, tmpval);
		m_model->setData(m_model->index(lastrow, i + 1, QModelIndex()), tmpval);
	}
}

void InputConditionWidgetFunctionalDialog::sort()
{
	if (! m_preventSort) {
		m_model->sort(0);
	}
	updateSpanColumns();
	updateGraph();
}

const InputConditionContainerFunctional& InputConditionWidgetFunctionalDialog::container() const
{
	return m_container;
}

void InputConditionWidgetFunctionalDialog::setData(const InputConditionContainerFunctional& c)
{
	m_container = c;
	setupData();
}

bool InputConditionWidgetFunctionalDialog::checkImportSourceUpdate()
{
	if (m_importedCsvFileName.isNull()) {return false;}
	QFileInfo finfo(m_importedCsvFileName);
	if (finfo.lastModified() == m_importedCsvLastModified) {return false;}

	QString msg = tr("File %1 is updated after imported to calculation condition %2. Do you want to import the updated file?").arg(QDir::toNativeSeparators(m_importedCsvFileName)).arg(windowTitle());
	int ret = QMessageBox::information(this, tr("Information"), msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return false;}

	bool ok = importFromCsv(m_importedCsvFileName);
	if (ok) {
		saveModel();
	}
	return ok;
}

void InputConditionWidgetFunctionalDialog::toggleReadOnly(bool readonly)
{
	std::vector<QWidget*> widgets;
	widgets.push_back(ui->addButton);
	widgets.push_back(ui->importButton);
	widgets.push_back(ui->clearButton);
	widgets.push_back(ui->buttonBox->button(QDialogButtonBox::Ok));

	for (QWidget* w : widgets) {
		w->setDisabled(readonly);
	}

	if (readonly) {
		ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
	} else {
		ui->tableView->setEditTriggers(QTableView::DoubleClicked | QTableView::AnyKeyPressed | QTableView::EditKeyPressed);
	}
}

void InputConditionWidgetFunctionalDialog::accept()
{
	bool ok = checkValues();
	if (! ok) {return;}

	saveModel();

	QDialog::accept();
}

void InputConditionWidgetFunctionalDialog::updateSpanColumns()
{
	if (m_model->rowCount() == 0) {return;}

	Qt::ItemFlags normalFlag = Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
	Qt::ItemFlags firstFlag = Qt::NoItemFlags;

	for (int i = 0; i < m_valueIsSpans.size(); ++i) {
		if (! m_valueIsSpans.at(i)) {
			continue;
		}
		QVariant zeroVal;
		(*m_valuefuncs[i])(0, zeroVal);
		m_model->setData(m_model->index(0, i + 1), zeroVal);
		m_model->setData(m_model->index(0, i + 1), QColor(Qt::gray), Qt::BackgroundColorRole);
		QStandardItem* item = m_model->item(0, i + 1);
		if (item != 0){item->setFlags(firstFlag);}
		for (int j = 1; j < m_model->rowCount(); ++j) {
			item = m_model->item(j, i + 1);
			if (item != 0){item->setFlags(normalFlag);}
			m_model->setData(m_model->index(j, i + 1), QColor(Qt::white), Qt::BackgroundColorRole);
		}
	}
}

bool InputConditionWidgetFunctionalDialog::checkValues()
{
	QStringList errors;
	int errorCount = 0;

	int rows = m_model->rowCount();
	for (int i = 0; i < rows; ++i) {
		double paramVal = m_model->data(m_model->index(i, 0)).toDouble();
		checkMinMax(&errors, &errorCount, paramVal, m_paramMinIsSet, m_paramMin, m_paramMaxIsSet, m_paramMax, i + 1, m_paramCaption);

		for (int j = 0; j < m_valuefuncs.size(); ++j) {
			double valueVal = m_model->data(m_model->index(i, j + 1)).toDouble();
			checkMinMax(&errors, &errorCount, valueVal, m_valueMinIsSet.at(j), m_valueMin.at(j), m_valueMaxIsSet.at(j), m_valueMax.at(j), i + 1, m_valueCaptions.at(j));
		}
	}
	if (errorCount == 0) {return true;}

	QString msg = tr("The following problems found: \n").append(errors.join('\n'));

	if (errorCount != errors.size()) {
		msg.append(tr("\n ... and other %1 errors").arg(errorCount - errors.size()));
	}
	QMessageBox::warning(this, tr("Warning"), msg);

	return false;
}

void InputConditionWidgetFunctionalDialog::updateGraph()
{
	if (m_preventGraph) {return;}

	clearGraphData();
	std::vector<double> x, y;

	for (int j = 0; j < m_axisSettings.size(); ++j) {
		QwtPlotCurve* pc = new QwtPlotCustomCurve();
		pc->setTitle(m_valueCaptions.at(j));
		QColor color = m_colorSource.getColor(j);
		pc->setPen(color, 1);
		if (m_valueIsSteps.at(j)) {
			setupXYStep(j, &x, &y);
		} else if (m_valueIsSpans.at(j)) {
			setupXYSpan(j, &x, &y);
		} else {
			setupXYStandard(j, &x, &y);
		}
		pc->setSamples(x.data(), y.data(), static_cast<int>(x.size()));

		if (m_axisSettings[j] == asLeft) {
			pc->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
			pc->attach(ui->graphView);
		} else if (m_axisSettings[j] == asRight) {
			pc->setAxes(QwtPlot::xBottom, QwtPlot::yRight);
			pc->attach(ui->graphView);
		} else {
			// do not shown on graphs.
		}
		m_graphCurves.push_back(pc);

		x.clear();
		y.clear();
	}
	if (m_axisSettings.size() > 1) {
		QwtLegend* legend = new QwtLegend();
		ui->graphView->insertLegend(legend, QwtPlot::RightLegend);
	}

	ui->graphView->replot();
}

void InputConditionWidgetFunctionalDialog::setupXYStandard(int row, std::vector<double>* x, std::vector<double>* y)
{
	int dataCount = m_model->rowCount();
	int len = dataCount;

	x->assign(len, 0);
	y->assign(len, 0);

	for (int i = 0; i < dataCount; ++i){
		(*x)[i] = m_model->data(m_model->index(i, 0)).toDouble();
		(*y)[i] = m_model->data(m_model->index(i, row + 1)).toDouble();
	}
}

void InputConditionWidgetFunctionalDialog::setupXYStep(int row, std::vector<double>* x, std::vector<double>* y)
{
	int dataCount = m_model->rowCount();
	int len = dataCount * 3 + 1;

	x->assign(len, 0);
	y->assign(len, 0);

	double firstx = 0;
	double x0 = m_model->data(m_model->index(0, 0)).toDouble();
	if (dataCount == 1){
		firstx = x0 - 1;
	} else if (dataCount > 0){
		double x1 = m_model->data(m_model->index(1, 0)).toDouble();
		double firstwidth = x1 - x0;
		firstx = x0 - firstwidth;
	}
	double xstart = firstx;
	(*x)[0] = xstart;
	(*y)[0] = 0;
	for (int i = 0; i < dataCount; ++i){
		double xend = m_model->data(m_model->index(i, 0)).toDouble();
		double yval =  m_model->data(m_model->index(i, row + 1)).toDouble();

		(*x)[i * 3 + 1] = xstart;
		(*y)[i * 3 + 1] = yval;
		(*x)[i * 3 + 2] = xend;
		(*y)[i * 3 + 2] = yval;
		(*x)[i * 3 + 3] = xend;
		(*y)[i * 3 + 3] = 0;

		xstart = xend;
	}
}

void InputConditionWidgetFunctionalDialog::setupXYSpan(int row, std::vector<double>* x, std::vector<double>* y)
{
	int dataCount = m_model->rowCount();
	int len = dataCount * 2 - 2;
	if (len <= 0) {len = 1;}

	x->assign(len, 0);
	y->assign(len, 0);

	if (dataCount == 0) {
		(*x)[0] = 0;
		(*y)[0] = 0;
		return;
	} else if (dataCount == 1) {
		(*x)[0] = m_model->data(m_model->index(0, 0)).toDouble();
		(*y)[0] = m_model->data(m_model->index(0, row * 1)).toDouble();
		return;
	}

	double xstart = m_model->data(m_model->index(0, 0)).toDouble();
	for (int i = 1; i < dataCount; ++i){
		double xend = m_model->data(m_model->index(i, 0)).toDouble();
		double yval =  m_model->data(m_model->index(i, row + 1)).toDouble();

		(*x)[i * 2 - 2] = xstart;
		(*y)[i * 2 - 2] = yval;
		(*x)[i * 2 - 1] = xend;
		(*y)[i * 2 - 1] = yval;

		xstart = xend;
	}
}
