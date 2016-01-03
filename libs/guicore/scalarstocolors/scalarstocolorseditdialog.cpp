#include "ui_scalarstocolorseditdialog.h"

#include "scalarstocolorseditdialog.h"
#include "scalarstocolorseditwidget.h"

#include <QVBoxLayout>

ScalarsToColorsEditDialog::ScalarsToColorsEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::ScalarsToColorsEditDialog),
	m_widget {nullptr}
{
	ui->setupUi(this);
}

ScalarsToColorsEditDialog::~ScalarsToColorsEditDialog()
{}

ScalarsToColorsEditWidget* ScalarsToColorsEditDialog::widget() const
{
	return m_widget;
}

void ScalarsToColorsEditDialog::setWidget(ScalarsToColorsEditWidget* w)
{
	m_widget = w;
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(w);
	layout->addStretch();
	layout->addWidget(ui->buttonBox);
	setLayout(layout);
	adjustSize();
}

void ScalarsToColorsEditDialog::setContainer(ScalarsToColorsContainer* container)
{
	m_widget->setContainer(container);
}

void ScalarsToColorsEditDialog::accept()
{
	m_widget->save();
	QDialog::accept();
}
