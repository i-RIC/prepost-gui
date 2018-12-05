#include "geodatapointmap_triangleswithlongedgeremover_dialog.h"
#include "ui_geodatapointmap_triangleswithlongedgeremover_dialog.h"

#include <guicore/pre/base/preprocessorwindowinterface.h>

GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog::Dialog(TrianglesWithLongEdgeRemover* remover) :
	QDialog(remover->m_parent->preProcessorWindow()),
	m_remover {remover},
	ui(new Ui::GeoDataPointmap_TrianglesWithLongEdgeRemover_Dialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog::~Dialog()
{
	delete ui;
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog::setlimitLength(double length)
{
	ui->lengthSpinBox->setValue(length);
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog::accept()
{
	m_remover->removeTriangles(limitLength(), false);
	QDialog::accept();
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		preview();
	}
}

double GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog::limitLength() const
{
	return ui->lengthSpinBox->value();
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::Dialog::preview()
{
	m_remover->updatePreviewPolyData(limitLength(), false);
}
