#include "ui_postallzoneselectingdialog.h"

#include "postallzoneselectingdialog.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

PostAllZoneSelectingDialog::PostAllZoneSelectingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PostAllZoneSelectingDialog)
{
	ui->setupUi(this);
	connect(ui->dimensionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setupGridTypeCombobox(int)));
	connect(ui->gridTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setupZoneList(int)));
}

PostAllZoneSelectingDialog::~PostAllZoneSelectingDialog()
{
	delete ui;
}

bool PostAllZoneSelectingDialog::setPostSolutionInfo(PostSolutionInfo* info)
{
	m_postSolutionInfo = info;
	ui->dimensionComboBox->blockSignals(true);
	if (info->isDataAvailable1D()) {
		QList<SolverDefinitionGridType*> typeList;
		QMap<SolverDefinitionGridType*, QList<PostZoneDataContainer*> > zoneMap;

		setupMaps(info->zoneContainers1D(), typeList, zoneMap);
		m_gridTypeMap.insert(1, typeList);
		m_zoneMap.insert(1, zoneMap);

		ui->dimensionComboBox->addItem(tr("One dimensional data"));
		m_dimensions.append(1);
	}
	if (m_postSolutionInfo->isDataAvailable2D()) {
		QList<SolverDefinitionGridType*> typeList;
		QMap<SolverDefinitionGridType*, QList<PostZoneDataContainer*> > zoneMap;

		setupMaps(info->zoneContainers2D(), typeList, zoneMap);
		m_gridTypeMap.insert(2, typeList);
		m_zoneMap.insert(2, zoneMap);

		ui->dimensionComboBox->addItem(tr("Two dimensional data"));
		m_dimensions.append(2);
	}
	if (m_postSolutionInfo->isDataAvailable3D()) {
		QList<SolverDefinitionGridType*> typeList;
		QMap<SolverDefinitionGridType*, QList<PostZoneDataContainer*> > zoneMap;

		setupMaps(info->zoneContainers3D(), typeList, zoneMap);
		m_gridTypeMap.insert(3, typeList);
		m_zoneMap.insert(3, zoneMap);

		ui->dimensionComboBox->addItem(tr("Three dimensional data"));
		m_dimensions.append(3);
	}
	ui->dimensionComboBox->blockSignals(false);

	// hide needless components
	if (m_dimensions.size() == 1) {
		ui->dimensionLabel->hide();
		ui->dimensionComboBox->hide();
	}
	bool multiGridTypes = false;
	foreach(int dim, m_gridTypeMap.keys()) {
		QList<SolverDefinitionGridType*> gridtypes = m_gridTypeMap.value(dim);
		multiGridTypes = multiGridTypes || (gridtypes.size() > 1);
	}
	if (! multiGridTypes) {
		ui->gridTypeLabel->hide();
		ui->gridTypeComboBox->hide();
	}
	bool multiZones = false;
	foreach(int dim, m_gridTypeMap.keys()) {
		QList<SolverDefinitionGridType*> gridtypes = m_gridTypeMap.value(dim);
		foreach(SolverDefinitionGridType* type, gridtypes) {
			QList<PostZoneDataContainer*> zoneList = m_zoneMap.value(dim).value(type);
			multiZones = multiZones || (zoneList.size() > 1);
		}
	}
	if (! multiZones) {
		ui->zoneLabel->hide();
		ui->zoneList->hide();
	}
	updateGeometry();

	ui->dimensionComboBox->setCurrentIndex(0);
	setupGridTypeCombobox(0);
	m_needToSelect = m_dimensions.size() > 1 || multiGridTypes || multiZones;
	return m_dimensions.size() > 0;
}

void PostAllZoneSelectingDialog::setupMaps(const QList<PostZoneDataContainer*>& srcList, QList<SolverDefinitionGridType*>& typeList, QMap<SolverDefinitionGridType*, QList<PostZoneDataContainer*> >& zoneMap)
{
	for (int i = 0; i < srcList.size(); ++i) {
		PostZoneDataContainer* cont = srcList.at(i);
		if (! typeList.contains(cont->gridType())) {
			typeList.append(cont->gridType());
			QList<PostZoneDataContainer*> newlist;
			newlist.append(cont);
			zoneMap.insert(cont->gridType(), newlist);
		} else {
			QList<PostZoneDataContainer*>& list = zoneMap[cont->gridType()];
			list.append(cont);
		}
	}
}

void PostAllZoneSelectingDialog::setupGridTypeCombobox(int index)
{
	int dimension = m_dimensions.at(index);
	m_gridTypes = m_gridTypeMap.value(dimension);
	ui->gridTypeComboBox->clear();
	for (int i = 0; i < m_gridTypes.size(); ++i) {
		SolverDefinitionGridType* gt = m_gridTypes.at(i);
		ui->gridTypeComboBox->addItem(gt->caption());
	}
	ui->gridTypeComboBox->setCurrentIndex(0);
}

void PostAllZoneSelectingDialog::setupZoneList(int index)
{
	int dimension = m_dimensions.at(ui->dimensionComboBox->currentIndex());
	SolverDefinitionGridType* gt = m_gridTypes.at(index);
	QList<PostZoneDataContainer*> zoneList = m_zoneMap.value(dimension).value(gt);
	ui->zoneList->clear();
	for (int i = 0; i < zoneList.size(); ++i) {
		PostZoneDataContainer* cont = zoneList.at(i);
		ui->zoneList->addItem(cont->zoneName().c_str());
	}
	ui->zoneList->setCurrentRow(0);
}

PostSolutionInfo::Dimension PostAllZoneSelectingDialog::dimension()
{
	return PostSolutionInfo::fromIntDimension(m_dimensions.at(ui->dimensionComboBox->currentIndex()));
}

PostZoneDataContainer* PostAllZoneSelectingDialog::container() const
{
	int dimension = m_dimensions.at(ui->dimensionComboBox->currentIndex());
	SolverDefinitionGridType* gt = m_gridTypes.at(ui->gridTypeComboBox->currentIndex());
	QList<PostZoneDataContainer*> zoneList = m_zoneMap.value(dimension).value(gt);
	PostZoneDataContainer* cont = zoneList.at(ui->zoneList->currentRow());
	return cont;
}
