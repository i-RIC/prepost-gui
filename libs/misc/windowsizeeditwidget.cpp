#include "windowsizeeditwidget.h"
#include "ui_windowsizeeditwidget.h"

#include <QSettings>

WindowSizeEditWidget::WindowSizeEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WindowSizeEditWidget)
{
	ui->setupUi(this);

	QSettings settings;
	auto s = settings.value("standardWindowSizes");
	auto list = s.toStringList();

	if (! list.isEmpty()) {
		for (const auto& str : list) {
			WindowSizeStandardSetting setting;
			setting.fromString(str);
			m_settings.push_back(setting);
		}
	} else {
		m_settings = WindowSizeStandardSetting::buildDefaultSettings();
	}
	ui->standardListWidget->clear();
	for (auto& s : m_settings) {
		ui->standardListWidget->addItem(s.labelString());
	}
	ui->standardListWidget->setCurrentRow(0);
}

WindowSizeEditWidget::~WindowSizeEditWidget()
{
	delete ui;
}

QSize WindowSizeEditWidget::size() const
{
	if (ui->standardRadioButton->isChecked()) {
		auto row = ui->standardListWidget->currentRow();
		return m_settings.at(row).size();
	} else if (ui->manualRadioButton->isChecked()) {
		QSize size;
		size.setWidth(ui->widthSpinBox->value());
		size.setHeight(ui->heightSpinBox->value());
		return size;
	}
	return QSize();
}

void WindowSizeEditWidget::setSize(const QSize& size)
{
	auto stdIndex = 0;
	for (int i = 0; i < static_cast<int> (m_settings.size()); ++i) {
		const auto& setting = m_settings.at(i);
		if (setting.size() == size) {
			stdIndex = i;
			break;
		}
		if (size.width() >= setting.width) {
			stdIndex = i;
		}
	}
	ui->standardListWidget->setCurrentRow(stdIndex);
	ui->widthSpinBox->setValue(size.width());
	ui->heightSpinBox->setValue(size.height());

	if (m_settings.at(stdIndex).size() == size) {
		ui->standardRadioButton->setChecked(true);
	} else {
		ui->manualRadioButton->setChecked(true);
	}
}
