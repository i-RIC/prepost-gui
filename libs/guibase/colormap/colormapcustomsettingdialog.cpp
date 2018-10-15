#include "ui_colormapcustomsettingdialog.h"

#include "../widget/coloreditwidget.h"
#include "colormapcustomsettingdialog.h"
#include "widget/realnumbereditwidget.h"

#include <misc/lastiodirectory.h>

#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QItemDelegate>
#include <QMessageBox>
#include <QPainter>
#include <QTextStream>

namespace {

	bool valueLessThan(const ColorMapCustomSettingColor& c1, const ColorMapCustomSettingColor& c2)
{
	return c1.value < c2.value;
}

/// Delegate class to handle creating color edit widget in ColorMapCustomSettingDialog.
class ColorMapCustomSettingDialogColorEditDelegate : public QItemDelegate
{
public:
	ColorMapCustomSettingDialogColorEditDelegate(QObject* parent = nullptr): QItemDelegate(parent) {}
	/// Paint event handler
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
		if (index.column() == 1) {
			QVariant dat = index.model()->data(index, Qt::DisplayRole);
			QColor col = dat.value<QColor>();
			QBrush brush(col);
			painter->fillRect(option.rect, brush);
		} else {
			QItemDelegate::paint(painter, option, index);
		}
	}
	/// Create editor
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const {
		if (index.column() == 1) {
			return new ColorEditWidget(parent);
		} else {
			return new RealNumberEditWidget(parent);
		}
	}
	/// Get data from the specified editor
	void setEditorData(QWidget* editor, const QModelIndex& index) const {
		if (index.column() == 1) {
			QVariant dat = index.model()->data(index, Qt::DisplayRole);
			ColorEditWidget* w = static_cast<ColorEditWidget*>(editor);
			w->setColor(dat.value<QColor>());
		} else {
			QVariant dat = index.model()->data(index, Qt::DisplayRole);
			RealNumberEditWidget* w = static_cast<RealNumberEditWidget*>(editor);
			w->setValue(dat.toDouble());
		}
	}
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
		if (index.column() == 1) {
			ColorEditWidget* w = static_cast<ColorEditWidget*>(editor);
			QColor c = w->color();
			model->setData(index, c, Qt::DisplayRole);
			model->setData(index, c, Qt::BackgroundRole);
		} else {
			RealNumberEditWidget* w = static_cast<RealNumberEditWidget*>(editor);
			double val = w->value();
			model->setData(index, val, Qt::DisplayRole);
		}
	}
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
		editor->setGeometry(option.rect);
	}
};

} // namespace


ColorMapCustomSettingDialog::ColorMapCustomSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::ColorMapCustomSettingDialog)
{
	ui->setupUi(this);
	ui->arbitraryTableWidget->setColumnWidth(0, VALUEWIDTH);
	ui->arbitraryTableWidget->setColumnWidth(1, COLORWIDTH);
	ui->arbitraryTableWidget->setSelectionBehavior((QAbstractItemView::SelectRows));
	ui->arbitraryTableWidget->setItemDelegate(new ColorMapCustomSettingDialogColorEditDelegate(this));
	connect(ui->typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(typeUpdated(int)));
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addColor()));
	connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(removeColor()));
	connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importColors()));
	connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportColors()));
	connect(ui->arbitraryTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleItemEdit(QTableWidgetItem*)));
	connect(ui->arbitraryTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(handleItemClick(QTableWidgetItem*)));
}

ColorMapCustomSettingDialog::~ColorMapCustomSettingDialog()
{
	delete ui;
}

ColorMapCustomSetting ColorMapCustomSettingDialog::customSetting() const
{
	ColorMapCustomSetting ret;

	switch (ui->typeComboBox->currentIndex()) {
	case 0:
		ret.type = ColorMapCustomSetting::Type::TwoColors;
		ret.minColor = ui->minColorWidget->color();
		ret.maxColor = ui->maxColorWidget->color();
		break;
	case 1:
		ret.type = ColorMapCustomSetting::Type::ThreeColors;
		ret.minColor = ui->minColorWidget->color();
		ret.maxColor = ui->maxColorWidget->color();
		ret.midColor = ui->midColorWidget->color();
		ret.midValue = ui->midValueEdit->value();
		break;
	case 2:
		ret.type = ColorMapCustomSetting::Type::Arbitrary;
		for (int i = 0; i < ui->arbitraryTableWidget->rowCount(); ++i) {
			QTableWidgetItem* item = ui->arbitraryTableWidget->item(i, 0);
			ColorMapCustomSettingColor cc;
			cc.value = item->data(Qt::DisplayRole).toDouble();
			cc.color = m_arbitraryColors.at(i);
			ret.arbitrarySettings.push_back(cc);
		}
		auto& settings = ret.arbitrarySettings;
		std::sort(settings.begin(), settings.end(), valueLessThan);
		break;
	}

	return ret;
}

void ColorMapCustomSettingDialog::setCustomSetting(const ColorMapCustomSetting& setting)
{
	switch (setting.type.value()) {
	case ColorMapCustomSetting::Type::TwoColors:
		ui->typeComboBox->setCurrentIndex(0);
		ui->minColorWidget->setColor(setting.minColor);
		ui->maxColorWidget->setColor(setting.maxColor);
		break;
	case ColorMapCustomSetting::Type::ThreeColors:
		ui->typeComboBox->setCurrentIndex(1);
		ui->minColorWidget->setColor(setting.minColor);
		ui->maxColorWidget->setColor(setting.maxColor);
		ui->midColorWidget->setColor(setting.midColor);
		ui->midValueEdit->setValue(setting.midValue);
		break;
	case ColorMapCustomSetting::Type::Arbitrary:
		ui->typeComboBox->setCurrentIndex(2);
		ui->arbitraryTableWidget->blockSignals(true);

		ui->arbitraryTableWidget->setRowCount(static_cast<int>(setting.arbitrarySettings.size()));
		int i = 0;
		for (auto s : setting.arbitrarySettings) {
			double value = s.value;
			QColor col = s.color;

			QTableWidgetItem* item = new QTableWidgetItem();
			item->setData(Qt::DisplayRole, value);
			ui->arbitraryTableWidget->setItem(i, 0, item);

			item = new QTableWidgetItem();
			item->setData(Qt::DisplayRole, col);
			item->setData(Qt::BackgroundRole, col);
			ui->arbitraryTableWidget->setItem(i, 1, item);
			ui->arbitraryTableWidget->setRowHeight(i, ROWHEIGHT);
			m_arbitraryColors.push_back(s.color);
			++ i;
		}

		ui->arbitraryTableWidget->blockSignals(false);
		break;
	}
	typeUpdated(ui->typeComboBox->currentIndex());
}

void ColorMapCustomSettingDialog::typeUpdated(int type)
{
	ui->minColorWidget->setDisabled(true);
	ui->midColorWidget->setDisabled(true);
	ui->maxColorWidget->setDisabled(true);
	ui->midValueEdit->setDisabled(true);
	ui->arbitraryGroupBox->setDisabled(true);
	switch (type) {
	case 0:
		// two colors
		ui->minColorWidget->setEnabled(true);
		ui->maxColorWidget->setEnabled(true);
		break;
	case 1:
		// three colors
		ui->minColorWidget->setEnabled(true);
		ui->midColorWidget->setEnabled(true);
		ui->maxColorWidget->setEnabled(true);
		ui->midValueEdit->setEnabled(true);
		break;
	case 2:
		// arbitrary colors
		ui->arbitraryGroupBox->setEnabled(true);
		break;
	}
}

void ColorMapCustomSettingDialog::addColor()
{
	double val = 0;
	QColor col = Qt::white;
	int rowCount = ui->arbitraryTableWidget->rowCount();
	if (rowCount > 0) {
		val = ui->arbitraryTableWidget->item(rowCount - 1, 0)->data(Qt::DisplayRole).toDouble() + 1;
		col = m_arbitraryColors.back();
	}
	ui->arbitraryTableWidget->setRowCount(rowCount + 1);
	QTableWidgetItem* item = new QTableWidgetItem();
	item->setData(Qt::DisplayRole, val);
	ui->arbitraryTableWidget->setItem(rowCount, 0, item);

	m_arbitraryColors.push_back(col);
	item = new QTableWidgetItem();
	item->setData(Qt::DisplayRole, col);
	item->setData(Qt::BackgroundRole, col);
	ui->arbitraryTableWidget->setItem(rowCount, 1, item);
	ui->arbitraryTableWidget->setRowHeight(rowCount, ROWHEIGHT);
}

void ColorMapCustomSettingDialog::removeColor()
{
	int row = ui->arbitraryTableWidget->currentRow();
	if (row < 0) {return;}
	m_arbitraryColors.erase(m_arbitraryColors.begin() + row);
	ui->arbitraryTableWidget->removeRow(row);
}

void ColorMapCustomSettingDialog::importColors()
{
	QString dir = LastIODirectory::get();
	QString fname = QFileDialog::getOpenFileName(this, tr("Import Colormap Setting"), dir, tr("CSV file (*.csv)"));
	if (fname == "") {return;}

	// check whether the file exists.
	if (! QFile::exists(fname)) {
		// the file does not exists.
		QMessageBox::warning(this, tr("Warning"), tr("File %1 does not exists.").arg(QDir::toNativeSeparators(fname)));
		return;
	}

	// import colors.
	QFile file(fname);
	if (! file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Warning"), tr("File %1 could not be opened.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	QList<double> values;
	QList<QColor> colors;

	QTextStream stream(&file);
	QString line;
	do {
		line = stream.readLine();
		if (! line.isEmpty()) {
			QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
			if (pieces.count() < 4) {continue;}
			bool ok;
			double val = pieces[0].toDouble(&ok);
			int r = pieces[1].toInt();
			int g = pieces[2].toInt();
			int b = pieces[3].toInt();
			if (r < 0 || r > 255) {continue;}
			if (g < 0 || g > 255) {continue;}
			if (b < 0 || b > 255) {continue;}

			values.append(val);
			colors.append(QColor(r, g, b));
		}
	} while (! line.isEmpty());

	file.close();

	ui->arbitraryTableWidget->blockSignals(true);
	ui->arbitraryTableWidget->setRowCount(values.count());
	for (int i = 0; i < values.count(); ++i) {
		double value = values.at(i);
		QColor col = colors.at(i);

		QTableWidgetItem* item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, value);
		ui->arbitraryTableWidget->setItem(i, 0, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, col);
		item->setData(Qt::BackgroundRole, col);
		ui->arbitraryTableWidget->setItem(i, 1, item);
		ui->arbitraryTableWidget->setRowHeight(i, ROWHEIGHT);
		m_arbitraryColors.push_back(col);
	}
	ui->arbitraryTableWidget->blockSignals(false);

	QFileInfo info(fname);
	LastIODirectory::set(info.absolutePath());
}

void ColorMapCustomSettingDialog::exportColors()
{
	QString dir = LastIODirectory::get();

	QString fname = QFileDialog::getSaveFileName(this, tr("Export Colormap Setting"), dir, tr("CSV file (*.csv)"));
	if (fname == "") {return;}

	// export colors.
	QFile file(fname);
	if (! file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Warning"), tr("File %1 could not be opened.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	QTextStream stream(&file);
	QString line;
	for (int i = 0; i < ui->arbitraryTableWidget->rowCount(); ++i) {
		double val = ui->arbitraryTableWidget->item(i, 0)->data(Qt::DisplayRole).toDouble();
		QColor col = m_arbitraryColors.at(i);
		stream << val << "," << col.red() << "," << col.green() << "," << col.blue() << endl;
	}

	file.close();

	QFileInfo info(fname);
	LastIODirectory::set(info.absolutePath());
}

void ColorMapCustomSettingDialog::handleItemEdit(QTableWidgetItem* item)
{
	if (item->column() != 1) {return;}
	QColor col = item->data(Qt::DisplayRole).value<QColor>();
	m_arbitraryColors[item->row()] = col;
}

void ColorMapCustomSettingDialog::handleItemClick(QTableWidgetItem* item)
{
	if (item->column() != 1) {return;}
	QColor col = item->data(Qt::DisplayRole).value<QColor>();
	QColor newcolor = QColorDialog::getColor(col, this);
	if (! newcolor.isValid()) {return;}
	item->setData(Qt::DisplayRole, newcolor);
}
