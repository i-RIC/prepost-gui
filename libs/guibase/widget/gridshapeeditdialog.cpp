#include "ui_gridshapeeditdialog.h"

#include "gridshapeeditdialog.h"

#include <QSettings>

GridShapeEditDialog::Setting::Setting() :
	CompositeContainer ({&color, &shape, &indexVisible, &indexTextSetting}),
	color {"color"},
	shape {"shape", GridShapeEditDialog::Outline},
	indexVisible {"indexVisible", false},
	indexTextSetting {}
{
	QSettings setting;
	color = setting.value("graphics/gridcolor", QColor(Qt::black)).value<QColor>();

	indexTextSetting.addPrefix("indexTextSetting");
	indexTextSetting.fontSize = 15;
	indexTextSetting.loadSetting(setting, "fontsetting/gridindices");
}

GridShapeEditDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GridShapeEditDialog::Setting& GridShapeEditDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GridShapeEditDialog::GridShapeEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridShapeEditDialog)
{
	ui->setupUi(this);

	connect(ui->outlineRadioButton, SIGNAL(toggled(bool)), this, SLOT(outlineToggled(bool)));
	setMaximumSize(sizeHint());
}

GridShapeEditDialog::~GridShapeEditDialog()
{
	delete ui;
}

void GridShapeEditDialog::hideShape()
{
	ui->allRadioButton->setChecked(true);
	ui->allRadioButton->hide();
	ui->outlineRadioButton->hide();
	adjustSize();
	setMaximumSize(sizeHint());
}

void GridShapeEditDialog::hideIndex()
{
	ui->indicesGroupBox->hide();
	adjustSize();
	setMaximumSize(sizeHint());
}

void GridShapeEditDialog::setSetting(const Setting& s)
{
	ui->gridColorEditWidget->setColor(s.color);
	ui->indexVisibleCheckBox->setChecked(s.indexVisible);
	ui->indexTextSettingWidget->setSetting(s.indexTextSetting);
	if (s.shape == Outline) {
		ui->outlineRadioButton->setChecked(true);
	} else {
		ui->allRadioButton->setChecked(true);
	}
}

GridShapeEditDialog::Setting GridShapeEditDialog::setting() const
{
	GridShapeEditDialog::Setting ret;

	ret.color = ui->gridColorEditWidget->color();
	ret.indexVisible = ui->indexVisibleCheckBox->isChecked();
	ret.indexTextSetting = ui->indexTextSettingWidget->setting();
	if (ui->outlineRadioButton->isChecked()) {
		ret.shape = Outline;
	} else {
		ret.shape = Wireframe;
	}

	return ret;
}


void GridShapeEditDialog::outlineToggled(bool toggled)
{
	if (toggled) {
		ui->indexVisibleCheckBox->setChecked(false);
	}
}
