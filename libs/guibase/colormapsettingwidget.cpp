#include "ui_colormapsettingwidget.h"

#include "colormapcustomsettingdialog.h"
#include "colormapsettingwidget.h"

#include <QDomNode>
#include <QXmlStreamWriter>

ColorMapSettingWidget::CustomSetting::CustomColor::CustomColor() :
	CompositeContainer ({&value, &color}),
	value {"value", 0},
	color {"color", Qt::black}
{}

ColorMapSettingWidget::CustomSetting::CustomSetting() :
	type {"type", ColorMapSettingWidget::CustomSetting::tTwoColors},
	maxColor {"maxColor", Qt::red},
	midColor {"midColor", Qt::white},
	minColor {"minColor", Qt::blue},
	midValue {"midValue", 0},
	arbitrarySettings {}
{}

void ColorMapSettingWidget::CustomSetting::load(const QDomNode& node)
{
	type.load(node);
	maxColor.load(node);
	minColor.load(node);
	midColor.load(node);
	midValue.load(node);

	if (type == tArbitrary) {
		arbitrarySettings.clear();
		QDomNodeList cols = node.childNodes();
		for (int i = 0; i < cols.count(); ++i) {
			QDomNode colNode = cols.at(i);
			if (colNode.nodeName() != "Color") {continue;}

			CustomColor cc;
			cc.load(colNode);
			arbitrarySettings.append(cc);
		}
	}
}

void ColorMapSettingWidget::CustomSetting::save(QXmlStreamWriter& writer) const
{
	type.save(writer);
	maxColor.save(writer);
	minColor.save(writer);
	midColor.save(writer);
	midValue.save(writer);

	if (type == tArbitrary) {
		for (int i = 0; i < arbitrarySettings.count(); ++i) {
			const ColorMapSettingWidget::CustomSetting::CustomColor& c = arbitrarySettings.at(i);
			writer.writeStartElement("Color");
			c.save(writer);
			writer.writeEndElement();
		}
	}
}

ColorMapSettingWidget::ColorMapSettingWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ColorMapSettingWidget)
{
	ui->setupUi(this);
	connect(ui->customSettingButton, SIGNAL(clicked()), this, SLOT(openCustomSettingDialog()));
}

ColorMapSettingWidget::~ColorMapSettingWidget()
{
	delete ui;
}

void ColorMapSettingWidget::setColorMap(ColorMap colorMap)
{
	switch (colorMap) {
	case Rainbow:
		ui->rainbowRadioButton->setChecked(true);
		break;
	case Grayscale:
		ui->grayscaleRadioButton->setChecked(true);
		break;
	case Custom:
		ui->customRadioButton->setChecked(true);
		break;
	}
}

ColorMapSettingWidget::ColorMap ColorMapSettingWidget::colorMap()
{
	if (ui->rainbowRadioButton->isChecked()) { return Rainbow; }
	if (ui->grayscaleRadioButton->isChecked()) { return Grayscale; }
	if (ui->customRadioButton->isChecked()) { return Custom; }
	//default
	return Rainbow;
}

void ColorMapSettingWidget::openCustomSettingDialog()
{
	ColorMapCustomSettingDialog dialog(this);
	dialog.setCustomSetting(m_customSetting);
	int ret = dialog.exec();
	if (ret == QDialog::Accepted) {
		m_customSetting = dialog.customSetting();
	}
}

void ColorMapSettingWidget::disableCustom()
{
	ui->customRadioButton->setDisabled(true);
	if (ui->customRadioButton->isChecked()) {
		ui->rainbowRadioButton->setChecked(true);
	}
}
