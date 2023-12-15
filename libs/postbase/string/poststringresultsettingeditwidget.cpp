#include "poststringresultargumentsettingeditdialog.h"
#include "poststringresultsettingcontainer.h"
#include "poststringresultsettingeditwidget.h"
#include "ui_poststringresultsettingeditwidget.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/lastiodirectory.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <QFileDialog>
#include <QMessageBox>

PostStringResultSettingEditWidget::PostStringResultSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	m_newSetting {},
	m_setting {nullptr},
	m_zoneDataContainer {nullptr},
	ui(new Ui::PostStringResultSettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->addButton, &QPushButton::clicked, this, &PostStringResultSettingEditWidget::addArgument);
	connect(ui->editButton, &QPushButton::clicked, this, &PostStringResultSettingEditWidget::editSelectedArgument);
	connect(ui->deleteButton, &QPushButton::clicked, this, &PostStringResultSettingEditWidget::deleteSelectedArgument);
	connect(ui->testButton, &QPushButton::clicked, this, &PostStringResultSettingEditWidget::test);
	connect(ui->inputTableWidget, &QTableWidget::cellDoubleClicked, this, &PostStringResultSettingEditWidget::editSelectedArgument);

	QList<int> sizes;
	sizes << 100 << 1;
	ui->totalSplitter->setSizes(sizes);
}

PostStringResultSettingEditWidget::~PostStringResultSettingEditWidget()
{
	delete ui;
}

void PostStringResultSettingEditWidget::setZoneDataContainer(v4PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;
}

PostStringResultSettingContainer PostStringResultSettingEditWidget::setting() const
{
	PostStringResultSettingContainer ret = m_newSetting;
	ret.script = ui->scriptEdit->toPlainText();

	ret.font = ui->fontWidget->font();
	ret.fontColor = ui->fontColorEditWidget->color();
	ret.backgroundColor = ui->backgroundColorEditWidget->color();
	ret.imageSetting = ui->imageSettingWidget->setting();

	return ret;
}

void PostStringResultSettingEditWidget::setSetting(const PostStringResultSettingContainer& setting)
{
	m_newSetting = setting;

	ui->scriptEdit->setPlainText(setting.script);
	updateArgumentsTable();

	ui->fontWidget->setFont(setting.font);
	ui->fontColorEditWidget->setColor(setting.fontColor);
	ui->backgroundColorEditWidget->setColor(setting.backgroundColor);
	ui->imageSettingWidget->setSetting(setting.imageSetting);
}

void PostStringResultSettingEditWidget::setSetting(PostStringResultSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);

	connect(&setting->imageSetting, &ImageSettingContainer::updated, this, &PostStringResultSettingEditWidget::updateImageSetting);
}

QUndoCommand* PostStringResultSettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<PostStringResultSettingContainer>(iRIC::generateCommandId("PostStringResultSettingEditWidget"), apply, setting(), m_setting);
}

void PostStringResultSettingEditWidget::importSetting()
{
	auto fname = QFileDialog::getOpenFileName(this, tr("Input file name to import"),
												LastIODirectory::get(), tr("Label setting (*.labelsetting)"));
	if (fname.isNull()) {return;}

	PostStringResultSettingContainer s;
	bool ok = s.importData(fname);
	if (! ok) {
		auto msg = tr("Error occured while opening %1")
				.arg(QDir::toNativeSeparators(fname));
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}
	setSetting(s);

	LastIODirectory::setFromFilename(fname);
}

void PostStringResultSettingEditWidget::exportSetting()
{
	auto fname = QFileDialog::getSaveFileName(this, tr("Input file name to export"),
												LastIODirectory::get(), tr("Label setting (*.labelsetting)"));
	if (fname.isNull()) {return;}

	auto s = setting();
	bool ok = s.exportData(fname);
	if (! ok) {
		auto msg = tr("Error occured while opening %1")
				.arg(QDir::toNativeSeparators(fname));
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}

	LastIODirectory::setFromFilename(fname);
}

void PostStringResultSettingEditWidget::addArgument()
{
	PostStringResultArgumentSettingEditDialog dialog(this);
	dialog.setZoneDataContainer(m_zoneDataContainer);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto arg = dialog.setting();
	m_newSetting.arguments.push_back(arg);

	updateArgumentsTable();
}

void PostStringResultSettingEditWidget::editSelectedArgument()
{
	if (ui->inputTableWidget->currentRow() < 0) {return;}
	int row = ui->inputTableWidget->currentRow();

	PostStringResultArgumentSettingEditDialog dialog(this);
	dialog.setZoneDataContainer(m_zoneDataContainer);
	auto& args = m_newSetting.arguments;
	auto arg = args.at(row);
	dialog.setSetting(arg);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	args[row] = dialog.setting();

	updateArgumentsTable();
}

void PostStringResultSettingEditWidget::deleteSelectedArgument()
{
	int row = ui->inputTableWidget->currentIndex().row();
	if (row < 0) {return;}

	auto arg = m_newSetting.arguments.at(row);
	QString name = arg.refName;
	int result = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete %1?").arg(name), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}

	auto it = m_newSetting.arguments.begin() + row;
	m_newSetting.arguments.erase(it);
	updateArgumentsTable();

	if (m_newSetting.arguments.size() == 0) {return;}

	if (row == static_cast<int> (m_newSetting.arguments.size())) {--row;}
	ui->inputTableWidget->selectRow(row);
}

void PostStringResultSettingEditWidget::test()
{
	applyScript();
	QString result;

	bool ok = m_engine.test(m_newSetting, &result, this);
	if (ok) {
		ui->testEdit->setPlainText(result);
	} else {
		ui->testEdit->setPlainText("----");
	}
}

void PostStringResultSettingEditWidget::updateImageSetting()
{
	ui->imageSettingWidget->setSetting(m_setting->imageSetting);
}

void PostStringResultSettingEditWidget::applyScript()
{
	m_newSetting.script = ui->scriptEdit->toPlainText();
}

void PostStringResultSettingEditWidget::updateArgumentsTable()
{
	auto w = ui->inputTableWidget;
	const auto& args = m_newSetting.arguments;

	w->setRowCount(static_cast<int>(args.size()));
	for (int i = 0; i < static_cast<int> (args.size()); ++i) {
		const auto& a = args.at(i);
		QTableWidgetItem* item = nullptr;

		QString type = typeString(a);
		item = new QTableWidgetItem(type);
		w->setItem(i, 0, item);

		item = new QTableWidgetItem(a.name);
		w->setItem(i, 1, item);

		item = new QTableWidgetItem(a.refName);
		w->setItem(i, 2, item);

		QString index = indexString(a);
		item = new QTableWidgetItem(index);
		w->setItem(i, 3, item);

		item = new QTableWidgetItem(a.valueForTest);
		w->setItem(i, 4, item);
	}
}

QString PostStringResultSettingEditWidget::typeString(const PostStringResultArgumentContainer& a)
{
	if (a.type == PostStringResultArgumentContainer::Type::BaseIterative) {
		return tr("Global");
	} else if (a.type == PostStringResultArgumentContainer::Type::GridNode) {
		return tr("Node");
	} else if (a.type == PostStringResultArgumentContainer::Type::GridCell) {
		return tr("Cell");
	} else if (a.type == PostStringResultArgumentContainer::Type::GridIEdge) {
		return tr("EdgeI");
	} else if (a.type == PostStringResultArgumentContainer::Type::GridJEdge) {
		return tr("EdgeJ");
	} else if (a.type == PostStringResultArgumentContainer::Type::GridKEdge) {
		return tr("EdgeK");
	}
	// to avoid compiler warning
	return "";
}

QString PostStringResultSettingEditWidget::indexString(const PostStringResultArgumentContainer& a)
{
	auto data = m_zoneDataContainer->gridData()->grid();
	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (data);
	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (data);
	auto uGrid = dynamic_cast<v4Unstructured2dGrid*> (data);

	QStringList vals;
	if (a.type == PostStringResultArgumentContainer::Type::BaseIterative) {
		return "";
	} else {
		if (uGrid != nullptr) {
			if (a.type == PostStringResultArgumentContainer::Type::GridNode) {
				vals.push_back(QString::number(a.index + 1));
			} else {
				Q_ASSERT_X(false, "PostStringResultEditWidget::setupIndex", "Invalid type for unstructured grid");
			}
		} else if (sGrid2d != nullptr) {
			vals.push_back(QString::number(a.i + 1));
			vals.push_back(QString::number(a.j + 1));
		} else if (sGrid3d != nullptr) {
			vals.push_back(QString::number(a.i + 1));
			vals.push_back(QString::number(a.j + 1));
			vals.push_back(QString::number(a.k + 1));
		}
	}
	return vals.join(", ");
}
