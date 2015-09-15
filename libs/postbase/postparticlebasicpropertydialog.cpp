#include "ui_postparticlebasicpropertydialog.h"

#include "postparticlebasicpropertydialog.h"
#include <QSettings>

PostParticleBasicPropertyDialog::Setting::Setting() :
	CompositeContainer ({&color, &size}),
	color {"color"},
	size {"size"}
{
	QSettings settings;
	color = settings.value("graphics/particlecolor", QColor(Qt::black)).value<QColor>();
	size = settings.value("graphics/particlesize", 3).toInt();
}

PostParticleBasicPropertyDialog::PostParticleBasicPropertyDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PostParticleBasicPropertyDialog}
{
	ui->setupUi(this);
}

PostParticleBasicPropertyDialog::~PostParticleBasicPropertyDialog()
{
	delete ui;
}

PostParticleBasicPropertyDialog::Setting PostParticleBasicPropertyDialog::setting() const
{
	Setting ret;

	ret.color = ui->colorWidget->color();
	ret.size = ui->sizeSpinBox->value();

	return ret;
}

void PostParticleBasicPropertyDialog::setSetting(const Setting& s)
{
	ui->colorWidget->setColor(s.color);
	ui->sizeSpinBox->setValue(s.size);
}
