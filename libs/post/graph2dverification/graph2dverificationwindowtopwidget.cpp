#include "graph2dverificationwindowtopwidget.h"
#include "ui_graph2dverificationwindowtopwidget.h"

Graph2dVerificationWindowTopWidget::Graph2dVerificationWindowTopWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graph2dVerificationWindowTopWidget)
{
    ui->setupUi(this);

	connect(ui->typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleIndexChanged(int)));
}

Graph2dVerificationWindowTopWidget::~Graph2dVerificationWindowTopWidget()
{
    delete ui;
}

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <vtkPointSet.h>
#include <vtkStructuredGrid.h>

void Graph2dVerificationWindowTopWidget::setSetting(const Graph2dVerificationWindowResultSetting& setting)
{
	Q_ASSERT(setting.activePostData() != nullptr);
	vtkPointSet* ps = setting.activePostData()->data();
	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);

	ui->typeComboBox->blockSignals(true);
	ui->typeComboBox->clear();

	if (sgrid == nullptr) {
		// unstructured grid.
	}
	else {
		// structured grid.
		ui->typeComboBox->addItem(tr("Stream-wise Distance vs. Measured Values and Calculation Result"));
		ui->typeComboBox->addItem(tr("Stream-wise Distance vs. Residual Errors"));
	}
	ui->typeComboBox->addItem(tr("Measured Values vs. Calculation Result"));
	ui->typeComboBox->addItem(tr("Measured Values vs. Residual Errors"));

	ui->typeComboBox->blockSignals(false);
	if (sgrid == nullptr) {
		switch (setting.graphType()) {
		case Graph2dVerificationWindowResultSetting::gtMVvsCR:
			ui->typeComboBox->setCurrentIndex(0);
			break;
		case Graph2dVerificationWindowResultSetting::gtMVvsError:
			ui->typeComboBox->setCurrentIndex(1);
			break;
		default:
			assert(false); 
			//m_graphType = Graph2dVerificationWindowResultSetting::gtMVvsCR;
			ui->typeComboBox->setCurrentIndex(0);
			break;
		}
	} else {
		switch (setting.graphType()) {
		case Graph2dVerificationWindowResultSetting::gtSWDvsValues:
			ui->typeComboBox->setCurrentIndex(0);
			break;
		case Graph2dVerificationWindowResultSetting::gtSWDvsError:
			ui->typeComboBox->setCurrentIndex(1);
			break;
		case Graph2dVerificationWindowResultSetting::gtMVvsCR:
			ui->typeComboBox->setCurrentIndex(2);
			break;
		case Graph2dVerificationWindowResultSetting::gtMVvsError:
			ui->typeComboBox->setCurrentIndex(3);
			break;
		}
	}
}

void Graph2dVerificationWindowTopWidget::handleIndexChanged(int index)
{
	emit graphTypeChanged(index);
}

