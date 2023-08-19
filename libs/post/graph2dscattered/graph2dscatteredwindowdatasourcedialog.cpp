#include "ui_graph2dscatteredwindowdatasourcedialog.h"

#include "graph2dscatteredwindowdatasourcedialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QString>
#include <QStringList>

#include <vtkPointData.h>
#include <vtkPointSet.h>
#include <vtkStructuredGrid.h>

Graph2dScatteredWindowDataSourceDialog::Graph2dScatteredWindowDataSourceDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dScatteredWindowDataSourceDialog)
{
	ui->setupUi(this);
}

Graph2dScatteredWindowDataSourceDialog::~Graph2dScatteredWindowDataSourceDialog()
{
	delete ui;
	for (int i = 0; i < m_initialSettings.size(); ++i) {
		Graph2dScatteredWindowResultSetting::Setting* s = m_initialSettings.at(i);
		if (s != 0) {delete s;}
	}
}

void Graph2dScatteredWindowDataSourceDialog::setDimension(PostSolutionInfo::Dimension dim)
{
	m_dimension = dim;
}

void Graph2dScatteredWindowDataSourceDialog::setZoneDataContainer(PostZoneDataContainer* cont)
{
	m_container = cont;
	setup();
}

void Graph2dScatteredWindowDataSourceDialog::setSetting(Graph2dScatteredWindowResultSetting& setting)
{
	m_setting = setting;
	const auto& tmpdata = setting.targetDatas();

	// x axis
	int idx = m_xAxisValues.indexOf(setting.xAxis());
	if (idx == -1) {idx = 0;}
	ui->xAxisComboBox->setCurrentIndex(idx);

	// y axis values
	for (int i = 0; i < m_yAxisValues.size(); ++i) {
		m_initialSettings.append(0);
	}
	ui->yAxisListWidget->clearSelection();
	for (int i = 0; i < tmpdata.size(); ++i) {
		Graph2dScatteredWindowResultSetting::Setting s = tmpdata.at(i);
		int idx = m_yAxisValues.indexOf(s.name());
		if (idx == -1) {continue;}
		Graph2dScatteredWindowResultSetting::Setting* sp = new Graph2dScatteredWindowResultSetting::Setting();
		*sp = s;
		m_initialSettings[idx] = sp;
		ui->yAxisListWidget->item(idx)->setSelected(true);
	}
}

Graph2dScatteredWindowResultSetting Graph2dScatteredWindowDataSourceDialog::setting() const
{
	Graph2dScatteredWindowResultSetting ret = m_setting;

	// x axis values
	int idx = ui->xAxisComboBox->currentIndex();
	std::string newXAxis = m_xAxisValues.at(idx);
	if (m_setting.xAxis() != newXAxis) {
		ret.setXAxis(newXAxis, ui->xAxisComboBox->currentText());
		ret.setXAxisAutoRange(true);
		ret.setXAxisLog(false);
		ret.setXAxisReverse(false);
	}

	// y values
	QList<Graph2dScatteredWindowResultSetting::Setting>& tmpdata = ret.targetDatas();
	tmpdata.clear();

	for (int i = 0; i < ui->yAxisListWidget->count(); ++i) {
		QListWidgetItem* item = ui->yAxisListWidget->item(i);
		if (item->isSelected()) {
			Graph2dScatteredWindowResultSetting::Setting* s = m_initialSettings.at(i);
			if (s != 0) {
				tmpdata.append(*s);
			} else {
				std::string name = m_yAxisValues.at(i);
				QString caption = ui->yAxisListWidget->item(i)->text();
				Graph2dScatteredWindowResultSetting::Setting s(name);
				s.setCaption(caption);
				tmpdata.append(s);
			}
		}
	}
	ret.setAutoXAxisLabel();
	ret.setYAxisLeftTitle(ret.autoYAxisLabel(Graph2dScatteredWindowResultSetting::asLeft));
	ret.setYAxisRightTitle(ret.autoYAxisLabel(Graph2dScatteredWindowResultSetting::asRight));

	return ret;
}

void Graph2dScatteredWindowDataSourceDialog::setup()
{
	ui->xAxisComboBox->clear();
	ui->yAxisListWidget->clear();
	m_xAxisValues.clear();
	m_yAxisValues.clear();

	// add data values.
	auto ps = m_container->data()->data();
	auto pd = ps->GetPointData();
	auto dt = m_container->gridType();
	int arrs = pd->GetNumberOfArrays();
	for (int i = 0; i < arrs; ++i) {
		vtkAbstractArray* aa = pd->GetAbstractArray(i);
		vtkDataArray* da = dynamic_cast<vtkDataArray*>(aa);
		if (da == nullptr) {continue;}
		if (da->GetNumberOfComponents() != 1) {continue;}

		std::string name = da->GetName();
		QString caption = dt->outputCaption(name);
		ui->xAxisComboBox->addItem(caption);
		ui->yAxisListWidget->addItem(caption);
		m_xAxisValues.append(name);
		m_yAxisValues.append(name);
	}
	// add position values for X.
	int dim = PostSolutionInfo::toIntDimension(m_dimension);
	ui->xAxisComboBox->addItem(tr("Position X"));
	m_xAxisValues.append(Graph2dScatteredWindowResultSetting::XAXIS_POSITION_X);
	if (dim > 1) {
		ui->xAxisComboBox->addItem(tr("Position Y"));
		m_xAxisValues.append(Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Y);
	}
	if (dim > 2) {
		ui->xAxisComboBox->addItem(tr("Position Z"));
		m_xAxisValues.append(Graph2dScatteredWindowResultSetting::XAXIS_POSITION_Z);
	}
	// if it is a structured grid add distance from upper.
//	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(ps);
//	if (grid != nullptr){
//		ui->xAxisComboBox->addItem(tr("Stream-wise Distance"));
//		m_xAxisValues.append(Graph2dScatteredWindowResultSetting::XAXIS_STREAM_WISE_DISTANCE);
//	}
}
