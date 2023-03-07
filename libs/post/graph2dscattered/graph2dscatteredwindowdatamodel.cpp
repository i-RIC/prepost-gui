#define _USE_MATH_DEFINES
#include <cmath>

#include "datamodel/graph2dscatteredwindowresultdataitem.h"
#include "datamodel/graph2dscatteredwindowresultgroupdataitem.h"
#include "datamodel/graph2dscatteredwindowrootdataitem.h"
#include "graph2dscatteredaxissettingdialog.h"
#include "graph2dscatteredwindow.h"
#include "graph2dscatteredwindowdatamodel.h"
#include "graph2dscatteredwindowdatasourcedialog.h"
#include "graph2dscatteredwindowdrawsettingdialog.h"
#include "graph2dscatteredwindowfontsettingdialog.h"
#include "graph2dscatteredwindowview.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guibase/objectbrowserview.h>
#include <misc/errormessage.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QFile>
#include <QFont>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QTextStream>

#include <qwt_abstract_legend.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_engine.h>

#include <stdexcept>

Graph2dScatteredWindowDataModel::Graph2dScatteredWindowDataModel(Graph2dScatteredWindow* w, ProjectDataItem* parent) :
	Graph2dWindowDataModel(w, parent)
{
	init();
}

Graph2dScatteredWindowDataModel::~Graph2dScatteredWindowDataModel()
{
	m_objectBrowserView->blockSignals(true);
	delete m_rootDataItem;
	m_objectBrowserView->blockSignals(false);
}

void Graph2dScatteredWindowDataModel::init()
{
	m_view = new Graph2dScatteredWindowView(dynamic_cast<QWidget*>(parent()));

	// setup the basic itemModel structure.
	Graph2dScatteredWindowRootDataItem* root = new Graph2dScatteredWindowRootDataItem(dynamic_cast<Graph2dScatteredWindow*>(m_mainWindow), this);
	m_rootDataItem = root;
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));

	PostSolutionInfo* post = postSolutionInfo();
	connect(post, SIGNAL(currentStepUpdated()), this, SLOT(updateTime()));
	connect(post, SIGNAL(cgnsStepsUpdated()), this, SLOT(updateData()));
}

Graph2dScatteredWindowView* Graph2dScatteredWindowDataModel::view() const
{
	return dynamic_cast<Graph2dScatteredWindowView*>(m_view);
}

PostSolutionInfo* Graph2dScatteredWindowDataModel::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

void Graph2dScatteredWindowDataModel::axisSetting()
{
	Graph2dScatteredAxisSettingDialog dialog(mainWindow());
	if (m_setting.yAxisLeftAutoRange()) {
		double min, max;
		getYAxisValueRange(Graph2dWindowDataModel::asLeft, &min, &max);
		m_setting.setYAxisLeftMin(min);
		m_setting.setYAxisLeftMax(max);
	}
	if (m_setting.yAxisRightAutoRange()) {
		double min, max;
		getYAxisValueRange(Graph2dWindowDataModel::asRight, &min, &max);
		m_setting.setYAxisRightMin(min);
		m_setting.setYAxisRightMax(max);
	}
	if (m_setting.xAxisAutoRange()) {
		double min, max;
		getXAxisValueRange(&min, &max);
		m_setting.setXAxisValueMin(min);
		m_setting.setXAxisValueMax(max);
	}
	dialog.setSetting(m_setting);
	if (QDialog::Rejected == dialog.exec()) {return;}
	m_setting = dialog.setting();
	applySettings();
	view()->replot();
}

bool Graph2dScatteredWindowDataModel::savePixmap(const QPixmap& pixmap, const QString& filename)
{
	// rename the temporary file to the target file.
	if (QFile::exists(filename)) {
		// remove first.
		if (! QFile::remove(filename)) {
			// unable to remove. fail.
			return false;
		}
	}
	return pixmap.save(filename);
}

void Graph2dScatteredWindowDataModel::showErrorMessage(const QString& filename)
{
	QMessageBox::critical(mainWindow(), tr("Error"), tr("Saving snapshot image to %1 failed.").arg(filename));
}

QString Graph2dScatteredWindowDataModel::formattedNumber(int number, int max)
{
	int limit = 10;
	int len = 1;
	while (max >= limit) {
		limit *= 10;
		++ len;
	}
	QString ret;
	QTextStream out(&ret);
	out.setFieldWidth(len);
	out.setPadChar('0');
	out.setFieldAlignment(QTextStream::AlignRight);
	out << number;

	return ret;
}

void Graph2dScatteredWindowDataModel::drawSetting()
{
	Graph2dScatteredWindowDrawSettingDialog dialog(mainWindow());
	Graph2dScatteredWindowRootDataItem* root = dynamic_cast<Graph2dScatteredWindowRootDataItem*>(m_rootDataItem);
	Graph2dScatteredWindowResultGroupDataItem* rgroup = root->resultGroupItem();
	QList <Graph2dWindowDataItem*> resultList = rgroup->childItems();
	QList<Graph2dScatteredWindowResultSetting::Setting> resultSettings;
	for (int i = 0; i < resultList.count(); ++i) {
		Graph2dScatteredWindowResultDataItem* item = dynamic_cast<Graph2dScatteredWindowResultDataItem*>(resultList.at(i));
		resultSettings.append(item->setting());
	}
	dialog.setResultSettings(resultSettings);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	resultSettings = dialog.resultSettings();
	for (int i = 0; i < resultList.count(); ++i) {
		Graph2dScatteredWindowResultDataItem* item = dynamic_cast<Graph2dScatteredWindowResultDataItem*>(resultList.at(i));
		item->setSetting(resultSettings.at(i));
	}
	m_setting.targetDatas() = resultSettings;

	updateData();
	applyAxisSetting();
	root->renderView();
}

void Graph2dScatteredWindowDataModel::fontSetting()
{
	Graph2dScatteredWindowFontSettingDialog dialog(mainWindow());

	dialog.setChartTitleFont(m_setting.chartTitleFont());
	dialog.setLegendsFont(m_setting.legendFont());
	dialog.setXAxisTitleFont(m_setting.xAxisTitleFont());
	dialog.setXAxisTickFont(m_setting.xAxisTickFont());
	dialog.setYAxisTitleFont(m_setting.yAxisTitleFont());
	dialog.setYAxisTickFont(m_setting.yAxisTickFont());

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.setChartTitleFont(dialog.chartTitleFont());
	m_setting.setLegendFont(dialog.legendsFont());
	m_setting.setXAxisTitleFont(dialog.xAxisTitleFont());
	m_setting.setXAxisTickFont(dialog.xAxisTickFont());
	m_setting.setYAxisTitleFont(dialog.yAxisTitleFont());
	m_setting.setYAxisTickFont(dialog.yAxisTickFont());

	applySettings();
}

void Graph2dScatteredWindowDataModel::updateTitle()
{
	QString title = m_setting.title().trimmed();
	int currentStep = postSolutionInfo()->currentStep();
	const QList<double>& timesteps = postSolutionInfo()->timeSteps()->timesteps();
	double time;
	if (timesteps.count() == 0) {
		time = 0;
	} else {
		if (currentStep < timesteps.count()) {
			time = timesteps.at(currentStep);
		} else {
			time = 0;
		}
	}
	if (m_setting.addTimeToTitle()) {
		QString suffix = tr("Time = %1 sec").arg(time);
		title.append(" : ").append(suffix);
	}

	QwtText titleText(title);
	titleText.setFont(m_setting.chartTitleFont());

	view()->setTitle(titleText);

	view()->legend()->setFont(m_setting.legendFont());
}

bool Graph2dScatteredWindowDataModel::setupInitialSetting()
{
	PostSolutionInfo* sInfo = postSolutionInfo();
	if (! sInfo->isDataAvailable()) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("No calculation result exists."));
		return false;
	}
	// initially, setup physical value settings.
	bool loaded = m_setting.init(postSolutionInfo());
	if (! loaded) {
		QMessageBox::critical(mainWindow(), tr("Error"), tr("Graph window setup fail. Calculation result is not loaded properly."));
		return false;
	}

	Graph2dScatteredWindowRootDataItem* rItem = dynamic_cast<Graph2dScatteredWindowRootDataItem*>(m_rootDataItem);

	Graph2dScatteredWindowDataSourceDialog dialog(mainWindow());
	dialog.setDimension(m_setting.dimension());
	dialog.setZoneDataContainer(sInfo->zoneContainer(m_setting.dimension(), m_setting.zoneName()));
	dialog.setSetting(m_setting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return false;
	}

	m_setting = dialog.setting();
	rItem->resultGroupItem()->updateChildren(m_setting);

	applySettings();
	view()->replot();

	return true;
}

void Graph2dScatteredWindowDataModel::applyAxisSetting()
{
	Graph2dWindowView* v = view();
	if (m_setting.xAxisLog()) {
		v->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine());
	} else {
		v->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine());
	}
	if (m_setting.xAxisReverse()) {
		v->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, true);
	} else {
		v->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Inverted, false);
	}
	if (m_setting.xAxisAutoRange()) {
		v->setAxisAutoScale(QwtPlot::xBottom);
	} else {
		if (m_setting.xAxisReverse()) {
			v->setAxisScale(QwtPlot::xBottom, m_setting.xAxisValueMax(), m_setting.xAxisValueMin());
		} else {
			v->setAxisScale(QwtPlot::xBottom, m_setting.xAxisValueMin(), m_setting.xAxisValueMax());
		}
	}
	QwtText xAxisTitle(m_setting.xAxisLabel());
	xAxisTitle.setFont(m_setting.xAxisTitleFont());

	v->setAxisTitle(QwtPlot::xBottom, xAxisTitle);
	v->setAxisFont(QwtPlot::xBottom, m_setting.xAxisTickFont());

	Graph2dScatteredWindowRootDataItem* root = dynamic_cast<Graph2dScatteredWindowRootDataItem*>(m_rootDataItem);
	if (root->axisNeeded(Graph2dScatteredWindowResultSetting::asLeft)) {
		v->enableAxis(QwtPlot::yLeft);
		v->setAxisFont(QwtPlot::yLeft, m_setting.yAxisTickFont());
		QwtText yAxisLeftLabelText(m_setting.yAxisLeftTitle());
		yAxisLeftLabelText.setFont(m_setting.yAxisTitleFont());
		v->setAxisTitle(QwtPlot::yLeft, yAxisLeftLabelText);
		if (m_setting.yAxisLeftLog()) {
			v->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
		} else {
			v->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
		}
		// control range.
		if (m_setting.yAxisLeftAutoRange()) {
			v->setAxisAutoScale(QwtPlot::yLeft);
		} else {
			if (m_setting.yAxisLeftReverse()) {
				v->setAxisScale(QwtPlot::yLeft, m_setting.yAxisLeftMax(), m_setting.yAxisLeftMin());
			} else {
				v->setAxisScale(QwtPlot::yLeft, m_setting.yAxisLeftMin(), m_setting.yAxisLeftMax());
			}
		}
		v->axisScaleEngine(QwtPlot::yLeft)->setAttribute(QwtScaleEngine::Inverted, m_setting.yAxisLeftReverse());
	} else {
		v->enableAxis(QwtPlot::yLeft, false);
	}
	if (root->axisNeeded(Graph2dScatteredWindowResultSetting::asRight)) {
		v->enableAxis(QwtPlot::yRight);
		v->setAxisFont(QwtPlot::yRight, m_setting.yAxisTickFont());
		QwtText yAxisRightLabelText(m_setting.yAxisRightTitle());
		yAxisRightLabelText.setFont(m_setting.yAxisTitleFont());
		v->setAxisTitle(QwtPlot::yRight, yAxisRightLabelText);

		if (m_setting.yAxisRightLog()) {
			v->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine());
		} else {
			v->setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine());
		}
		// control range.
		if (m_setting.yAxisRightAutoRange()) {
			v->setAxisAutoScale(QwtPlot::yRight);
		} else {
			if (m_setting.yAxisRightReverse()) {
				v->setAxisScale(QwtPlot::yRight, m_setting.yAxisRightMax(), m_setting.yAxisRightMin());
			} else {
				v->setAxisScale(QwtPlot::yRight, m_setting.yAxisRightMin(), m_setting.yAxisRightMax());
			}
		}
		v->axisScaleEngine(QwtPlot::yRight)->setAttribute(QwtScaleEngine::Inverted, m_setting.yAxisRightReverse());
	} else {
		v->enableAxis(QwtPlot::yRight, false);
	}
}

void Graph2dScatteredWindowDataModel::getXAxisValueRange(double* min, double* max)
{
	Graph2dWindowView* v = view();
	QwtScaleDiv sDiv = v->axisScaleDiv(QwtPlot::xBottom);
	*min = qMin(sDiv.lowerBound(), sDiv.upperBound());
	*max = qMax(sDiv.lowerBound(), sDiv.upperBound());
}

void Graph2dScatteredWindowDataModel::getYAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max)
{
	Graph2dWindowView* v = view();
	QwtScaleDiv sDiv;
	if (as == Graph2dWindowDataModel::asLeft) {
		sDiv = v->axisScaleDiv(QwtPlot::yLeft);
	} else {
		sDiv = v->axisScaleDiv(QwtPlot::yRight);
	}
	*min = qMin(sDiv.lowerBound(), sDiv.upperBound());
	*max = qMax(sDiv.lowerBound(), sDiv.upperBound());
}

void Graph2dScatteredWindowDataModel::updateTime()
{
	updateData();
	view()->replot();
}

void Graph2dScatteredWindowDataModel::dataSourceSetting()
{
	Graph2dScatteredWindowRootDataItem* rItem = dynamic_cast<Graph2dScatteredWindowRootDataItem*>(m_rootDataItem);

	Graph2dScatteredWindowDataSourceDialog dialog(mainWindow());
	dialog.setDimension(m_setting.dimension());
	dialog.setZoneDataContainer(postSolutionInfo()->zoneContainer(m_setting.dimension(), m_setting.zoneName()));
	dialog.setSetting(m_setting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return;
	}

	m_setting = dialog.setting();
	rItem->resultGroupItem()->updateChildren(m_setting);

	applySettings();
	view()->replot();
}

void Graph2dScatteredWindowDataModel::applySettings()
{
	// update axis setting.
	applyAxisSetting();
	updateData();
}

void Graph2dScatteredWindowDataModel::updateData()
{
	static bool updating = false;
	if (updating == true) {
		return;
	}
	updating = true;
	Graph2dScatteredWindowRootDataItem* root = dynamic_cast<Graph2dScatteredWindowRootDataItem*>(m_rootDataItem);
	root->updateData();
	updating = false;

	updateTitle();
}

void Graph2dScatteredWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	bool ok = m_setting.init(postSolutionInfo());
	if (! ok) {
		throw ErrorMessage("No solution found.");
	}
	QDomNode sNode = iRIC::getChildNode(node, "Setting");
	if (! sNode.isNull()) {
		m_setting.loadFromProjectMainFile(sNode);
	}
	Graph2dWindowDataModel::doLoadFromProjectMainFile(node);

	Graph2dScatteredWindowRootDataItem* rItem = dynamic_cast<Graph2dScatteredWindowRootDataItem*>(m_rootDataItem);
	rItem->resultGroupItem()->updateChildren(m_setting);

	applySettings();
	updateData();
	view()->replot();
}

void Graph2dScatteredWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	Graph2dWindowDataModel::doSaveToProjectMainFile(writer);
	writer.writeStartElement("Setting");
	m_setting.saveToProjectMainFile(writer);
	writer.writeEndElement();
}
