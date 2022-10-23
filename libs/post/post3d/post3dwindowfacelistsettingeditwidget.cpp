#include "post3dwindowfacesettingcontainer.h"
#include "post3dwindowfacelistsettingeditwidget.h"
#include "ui_post3dwindowfacelistsettingeditwidget.h"

#include <QList>

Post3dWindowFaceListSettingEditWidget::Post3dWindowFaceListSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_currentRow {-1},
	ui(new Ui::Post3dWindowFaceListSettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->faceAddButton, &QPushButton::clicked, [=](bool) {addFace();});
	connect(ui->faceRemoveButton, &QPushButton::clicked, [=](bool) {removeFace();});
	connect(ui->faceListWidget, &QListWidget::currentRowChanged, this, &Post3dWindowFaceListSettingEditWidget::setCurrentFace);

	QList<int> sizes;
	sizes.append(50);
	sizes.append(200);
	ui->splitter->setSizes(sizes);
}

Post3dWindowFaceListSettingEditWidget::~Post3dWindowFaceListSettingEditWidget()
{
	delete ui;
}

void Post3dWindowFaceListSettingEditWidget::setDimensions(int* dims)
{
	ui->faceSettingWidget->setDimensions(dims);
	for (int i = 0; i < 3; ++i) {
		m_gridDimensions[i] = *(dims + i);
	}
}

std::vector<Post3dWindowFaceSettingContainer> Post3dWindowFaceListSettingEditWidget::faces()
{
	saveCurrentFace();

	return m_faces;
}

void Post3dWindowFaceListSettingEditWidget::setFaces(const std::vector<Post3dWindowFaceSettingContainer>& faces)
{
	m_faces = faces;
	updateFaceList();

	if (m_faces.size() == 0) {
		ui->faceSettingWidget->setDisabled(true);
	} else {
		ui->faceListWidget->setCurrentRow(0);
	}
}

void Post3dWindowFaceListSettingEditWidget::addFace()
{
	ui->faceSettingWidget->setEnabled(true);

	Post3dWindowFaceSettingContainer newSetting;

	if (m_faces.size() == 0) {
		// create default
		newSetting.enabled = true;
		newSetting.iMin = 0;
		newSetting.iMax = m_gridDimensions[0] - 1;
		newSetting.jMin = 0;
		newSetting.jMax = m_gridDimensions[1] - 1;
		newSetting.kMin = 0;
		newSetting.kMax = m_gridDimensions[2] - 1;
	} else {
		// copy current;
		saveCurrentFace();
		newSetting = m_faces.at(ui->faceListWidget->currentRow());
	}

	m_faces.push_back(newSetting);
	updateFaceList();

	int row = m_faces.size() - 1;
	ui->faceListWidget->setCurrentRow(row);
	ui->faceListWidget->scrollToItem(ui->faceListWidget->item(row));

	setCurrentFace(row);
}

void Post3dWindowFaceListSettingEditWidget::removeFace()
{
	if (m_faces.size() == 0) {return;}

	int row = ui->faceListWidget->currentRow();
	m_faces.erase(m_faces.begin() + row);
	m_currentRow = -1;

	updateFaceList();

	if (m_faces.size() == 0) {
		Post3dWindowFaceSettingContainer dummy;
		ui->faceSettingWidget->setSetting(dummy);
		ui->faceSettingWidget->setDisabled(true);
		return;
	} else if (row >= m_faces.size()) {
		row = m_faces.size() - 1;
	}
	ui->faceListWidget->setCurrentRow(row);
}

void Post3dWindowFaceListSettingEditWidget::setCurrentFace(int row)
{
	if (m_currentRow == row) {return;}

	saveCurrentFace();

	const auto& rs = m_faces[row];
	ui->faceSettingWidget->setSetting(rs);
	m_currentRow = row;
}

void Post3dWindowFaceListSettingEditWidget::updateFaceList()
{
	auto w = ui->faceListWidget;
	w->blockSignals(true);

	w->clear();
	int idx = 1;
	for (auto rs : m_faces) {
		w->addItem(tr("Face%1").arg(idx));
		++ idx;
	}
	w->blockSignals(false);
}

void Post3dWindowFaceListSettingEditWidget::saveCurrentFace()
{
	if (m_currentRow == -1) {return;}

	m_faces[m_currentRow] = ui->faceSettingWidget->setting();
}
