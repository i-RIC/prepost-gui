#include "ui_postparticlebasicpropertydialog.h"

#include "postparticlebasicpropertydialog.h"

PostParticleBasicPropertyDialog::PostParticleBasicPropertyDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PostParticleBasicPropertyDialog)
{
	ui->setupUi(this);
}

PostParticleBasicPropertyDialog::~PostParticleBasicPropertyDialog()
{
	delete ui;
}

const QColor PostParticleBasicPropertyDialog::color() const
{
	return ui->colorWidget->color();
}

int PostParticleBasicPropertyDialog::size() const
{
	return ui->sizeSpinBox->value();
}

void PostParticleBasicPropertyDialog::setColor(const QColor color)
{
	ui->colorWidget->setColor(color);
}

void PostParticleBasicPropertyDialog::setSize(int size)
{
	ui->sizeSpinBox->setValue(size);
}
