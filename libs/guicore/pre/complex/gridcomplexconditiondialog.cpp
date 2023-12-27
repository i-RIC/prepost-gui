#include "ui_gridcomplexconditiondialog.h"

#include "../../base/iricmainwindowi.h"
#include "../../project/colorsource.h"
#include "../../project/inputcond/inputconditioncontainerset.h"
#include "../../project/projectdata.h"
#include "../../solverdef/solverdefinitiongridcomplexattribute.h"
#include "../base/preprocessorgeodatacomplexgroupdataitemi.h"
#include "gridcomplexconditiondialog.h"
#include "gridcomplexconditiongroup.h"
#include "gridcomplexconditionwidget.h"

#include <misc/csviohelper.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextCodec>
#include <QWidget>

#include <h5cgnsgridcomplexconditiongroup.h>
#include <iriclib_errorcodes.h>

#include <unordered_set>

namespace {

const std::string CSV_NAME = "_name";
const std::string CSV_COLOR = "_color";
const std::string CSV_DEFAULT = "_default";

} // namespace

GridComplexConditionDialog::GridComplexConditionDialog(SolverDefinition* def, const QDomElement& elem, QWidget* parent) :
	QDialog(parent),
	m_solverDefinition {def},
	m_element {elem},
	m_undefinedColor {Qt::gray},
	m_calculationConditionMode {false},
	ui(new Ui::GridComplexConditionDialog)
{
	ui->setupUi(this);

	m_colorSource = new ColorSource(nullptr);

	connect(ui->listWidget, &QListWidget::currentRowChanged, this, &GridComplexConditionDialog::selectItem);
	connect(ui->addButton, &QPushButton::clicked, this, &GridComplexConditionDialog::addItem);
	connect(ui->addButton_tableView, &QPushButton::clicked, this, &GridComplexConditionDialog::addItem);
	connect(ui->deleteButton, &QPushButton::clicked, this, &GridComplexConditionDialog::removeItem);
	connect(ui->switchButton, &QPushButton::clicked, this, &GridComplexConditionDialog::switchView);
	connect(ui->importButton, &QPushButton::clicked, this, &GridComplexConditionDialog::importCsvFile);
	connect(ui->exportButton, &QPushButton::clicked, this, &GridComplexConditionDialog::exportCsvFile);

	updateSwitchButtonText();
}

GridComplexConditionDialog::~GridComplexConditionDialog()
{
	delete ui;
	delete m_colorSource;
}

const std::vector<std::shared_ptr<GridComplexConditionGroup> > GridComplexConditionDialog::groups() const
{
	return m_groups;
}

void GridComplexConditionDialog::createDefaultGroup()
{
	std::shared_ptr<GridComplexConditionGroup> newGroup (new GridComplexConditionGroup(m_solverDefinition, m_element));
	newGroup->setCaption("Default");
	newGroup->setColor(m_undefinedColor);
	newGroup->setIsDefault(true);
	m_groups.push_back(newGroup);
}

void GridComplexConditionDialog::setupGroups(int count)
{
	m_groups.clear();
	for (int i = 0; i < count; ++i) {
		std::shared_ptr<GridComplexConditionGroup> g(new GridComplexConditionGroup(m_solverDefinition, m_element));
		if (i == 0) {g->setIsDefault(true);}

		m_groups.push_back(g);
	}
}

void GridComplexConditionDialog::clear()
{
	m_groups.clear();
}

int GridComplexConditionDialog::loadFromCgnsFile(iRICLib::H5CgnsGridComplexConditionGroup* group)
{
	m_groups.clear();

	int defaultId = -1;
	for (int i = 0; i < group->itemCount(); ++i) {
		auto item = group->item(i + 1);

		std::shared_ptr<GridComplexConditionGroup> group (new GridComplexConditionGroup(m_solverDefinition, m_element));
		int ier = group->load(*item);
		if (ier != IRIC_NO_ERROR) {return ier;}

		if (group->isDefault()) {
			defaultId = i;
		}
		m_groups.push_back(group);
	}
	if (! m_calculationConditionMode && defaultId == -1 && m_groups.size() > 0) {
		// make thr first group default.
		m_groups[0]->setIsDefault(true);
	}

	return IRIC_NO_ERROR;
}

int GridComplexConditionDialog::saveToCgnsFile(iRICLib::H5CgnsGridComplexConditionGroup* group)
{
	for (int i = 0; i < static_cast<int> (m_groups.size()); ++i) {
		auto* g = m_groups.at(i).get();
		auto item = group->item(i + 1);
		int ier = g->save(item);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	return IRIC_NO_ERROR;
}

bool GridComplexConditionDialog::importFromCsvFile(const QString& filename, QWidget* w)
{
	clear();

	QFileInfo fileInfo(filename);
	QDir dir = fileInfo.dir();

	auto names = GridComplexConditionGroup::widgetNames(m_element);
	std::unordered_set<std::string> nameSet;
	for (const auto& name : names) {
		nameSet.insert(name);
	}
	nameSet.insert(CSV_NAME);
	if (! m_calculationConditionMode) {
		nameSet.insert(CSV_COLOR);
		nameSet.insert(CSV_DEFAULT);
	}

	QFile f(filename);
	bool ok = f.open(QFile::ReadOnly | QFile::Text);
	if (! ok) {return false;}

	QTextStream stream(&f);
	QString line;
	auto header = stream.readLine();
	auto headerColumns = CsvIoHelper::fromCsv(header);
	std::vector<std::string> headerColumnsStr;
	for (const auto& h : headerColumns) {
		headerColumnsStr.push_back(iRIC::toStr(h));
	}

	QStringList ignoredColumns;

	for (const auto& h : headerColumnsStr) {
		auto it = nameSet.find(h);
		if (it == nameSet.end()) {ignoredColumns.push_back(h.c_str());}
	}

	if (ignoredColumns.size() > 0) {
		QStringList expectedNames;
		expectedNames.push_back(CSV_NAME.c_str());
		if (! m_calculationConditionMode) {
			expectedNames.push_back(CSV_COLOR.c_str());
			expectedNames.push_back(CSV_DEFAULT.c_str());
		}
		for (const auto& name : names) {
			expectedNames.push_back(name.c_str());
		}

		QMessageBox::warning(w, tr("Warning"), tr("Columns %1 are ignored. Expected names are %2").arg(ignoredColumns.join(", ")).arg(expectedNames.join(", ")));
	}

	int defaultId = -1;
	int itemNumber = 1;
	do {
		line = stream.readLine();
		if (line.isEmpty()) {break;}

		auto frags = CsvIoHelper::fromCsv(line);

		std::shared_ptr<GridComplexConditionGroup> group (new GridComplexConditionGroup(m_solverDefinition, m_element));
		group->setColor(m_colorSource->getColor(itemNumber - 1));
		for (int i = 0; i < frags.size(); ++i) {

			if (i >= static_cast<int> (headerColumnsStr.size())) {break;}

			std::string valueName = headerColumnsStr.at(i);
			QString frag = frags.at(i);

			if (valueName == CSV_NAME) {
				group->setCaption(frag);
			} else if (valueName == CSV_COLOR) {
				group->setColor(QColor(frag));
			} else if (valueName == CSV_DEFAULT) {
				int is_default = frag.toInt();
				group->setIsDefault(is_default != 0);
				if (group->isDefault()) {
					defaultId = itemNumber;
				}
			} else {
				auto c = group->containerSet()->container(valueName);
				if (c != nullptr) {
					auto fc = dynamic_cast<InputConditionContainerFunctional*> (c);
					if (fc != nullptr) {
						QString csvFileName = QString("%1_%2.csv").arg(valueName.c_str()).arg(itemNumber);
						fc->loadDataFromCsvFile(dir.absoluteFilePath(csvFileName));
					} else {
						c->importFromString(frag, dir.absolutePath());
					}
				}
			}
		}
		m_groups.push_back(group);

		++ itemNumber;
	} while (true);

	if (defaultId == -1 && m_groups.size() > 0) {
		m_groups[0]->setIsDefault(true);
	}

	f.close();

	m_backupGroups = m_groups;
	m_widgets.clear();

	for (const auto& g : m_groups) {
		auto widget = new GridComplexConditionWidget(this);
		if (m_calculationConditionMode) {
			widget->hideWidgetsNotForCalculationCondition();
		}
		widget->setGroup(g.get());
		m_widgets.push_back(widget);
	}
	ui->widgetContainer->setWidget(new QWidget(this));

	updateList();
	if (m_widgets.size() > 0) {
		ui->listWidget->setCurrentRow(0);
	}
	updateTable();

	return true;
}

bool GridComplexConditionDialog::exportToCsvFile(const QString& filename)
{
	QFileInfo fileInfo(filename);
	QDir dir = fileInfo.dir();

	QFile f(filename);
	bool ok = f.open(QFile::WriteOnly | QFile::Text);
	if (! ok) {return false;}

	std::vector<std::string> valueNames;
	valueNames.push_back(CSV_NAME);
	if (! m_calculationConditionMode) {
		valueNames.push_back(CSV_COLOR);
		valueNames.push_back(CSV_DEFAULT);
	}
	auto names = GridComplexConditionGroup::widgetNames(m_element);
	for (const auto& name : names) {
		valueNames.push_back(name);
	}

	QTextStream stream(&f);
	// export header
	QStringList header;
	for (const auto& valueName : valueNames) {
		header.push_back(valueName.c_str());
	}
	stream << CsvIoHelper::toCsv(header) << "\n";

	for (int i = 0; i < static_cast<int> (m_groups.size()); ++i) {
		auto group = m_groups.at(i).get();
		QStringList values;
		for (const auto& valueName : valueNames) {
			if (valueName == CSV_NAME) {
				values.push_back(group->caption());
			} else if (valueName == CSV_COLOR) {
				values.push_back(group->color().name());
			} else if (valueName == CSV_DEFAULT) {
				QString defaultStr = "0";
				if (group->isDefault()) {
					defaultStr = "1";
				}
				values.push_back(defaultStr);
			} else {
				auto c = group->containerSet()->container(valueName);
				auto fc = dynamic_cast<InputConditionContainerFunctional*> (c);
				QString value;
				if (fc != nullptr) {
					QString csvFileName = QString("%1_%2.csv").arg(valueName.c_str()).arg(i + 1);
					fc->saveDataToCsvFile(dir.absoluteFilePath(csvFileName));
					value = csvFileName;
				} else {
					c->exportToString(&value, dir);
				}
				values.push_back(value);
			}
		}
		stream << CsvIoHelper::toCsv(values) << "\n";
	}

	return true;
}

void GridComplexConditionDialog::setCalculationConditionMode(bool mode)
{
	m_calculationConditionMode = mode;
}

int GridComplexConditionDialog::exec()
{
	m_backupGroups = m_groups;
	for (auto g : m_groups) {
		g->backup();
	}

	m_widgets.clear();
	for (const auto& g : m_groups) {
		auto widget = new GridComplexConditionWidget(this);
		if (m_calculationConditionMode) {
			widget->hideWidgetsNotForCalculationCondition();
		}
		widget->setGroup(g.get());
		m_widgets.push_back(widget);
	}
	if (ui->stackedWidget->currentIndex() == 0) {
		updateList();
		if (m_widgets.size() > 0) {
			ui->listWidget->setCurrentRow(0);
		}
	} else if (ui->stackedWidget->currentIndex() == 1){
		updateTable();
	}

	int ret = QDialog::exec();

	m_backupGroups.clear();
	m_widgets.clear();
	ui->widgetContainer->setWidget(new QWidget(this));

	return ret;
}

void GridComplexConditionDialog::accept()
{
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	bool allok = true;
	for (const auto& w : m_widgets) {
		allok = allok && asciiCodec->canEncode(w->caption());
	}
	if (! allok) {
		QMessageBox::warning(this, tr("Warning"), tr("Name has to consist of only English characters."));
		return;
	}
	// Check whether one of the items are set to be default.
	int defIndex = -1;
	for (int i = 0; i < static_cast<int> (m_widgets.size()); ++i) {
		if (m_widgets[i]->isDefault()) {
			defIndex = i;
		}
	}

	disconnect(this, SLOT(defaultChecked(bool)));
	if (defIndex == -1) {
		// if no default specified and there is more than one widget, make the first one default.
		m_widgets[0]->setIsDefault(true);
	}

	ui->widgetContainer->setWidget(nullptr);
	QDialog::accept();
}

void GridComplexConditionDialog::reject()
{
	m_groups = m_backupGroups;
	for (auto g : m_groups) {
		g->restore();
	}

	ui->widgetContainer->setWidget(nullptr);

	QDialog::reject();
}

void GridComplexConditionDialog::defaultChecked(bool checked)
{
	if (! checked) {return;}

	int current = ui->listWidget->currentRow();
	for (int i = 0; i < static_cast<int> (m_widgets.size()); ++i) {
		if (i != current) {
			m_widgets[i]->setIsDefault(false);
		}
	}
}

void GridComplexConditionDialog::selectItem(int item)
{
	if (item == -1) {return;}
	auto w = m_widgets.at(item);
	ui->widgetContainer->setWidget(w);

	disconnect(this, SLOT(updateCurrentName(QString)));
	connect(w, &GridComplexConditionWidget::captionChanged, this, &GridComplexConditionDialog::updateCurrentName);

	disconnect(this, SLOT(defaultChecked(bool)));
	connect(w, &GridComplexConditionWidget::defaultChecked, this, &GridComplexConditionDialog::defaultChecked);
}

void GridComplexConditionDialog::addItem()
{
	std::shared_ptr<GridComplexConditionGroup> newGroup(new GridComplexConditionGroup(m_solverDefinition, m_element));
	m_groups.push_back(newGroup);

	auto newWidget = new GridComplexConditionWidget(this);
	if (m_calculationConditionMode) {newWidget->hideWidgetsNotForCalculationCondition();}
	newWidget->setGroup(newGroup.get());
	newWidget->setCaption(QString("Item%1").arg(m_widgets.size() + 1));

	if (m_widgets.size() == 0) {
		// this is the first one. make it the default.
		newWidget->setIsDefault(true);
	}

	newWidget->setColor(m_colorSource->getColor(static_cast<int>(m_widgets.size())));
	m_widgets.push_back(newWidget);
	if (ui->stackedWidget->currentIndex() == 0) {
		updateList();
		ui->listWidget->setCurrentRow(static_cast<int>(m_widgets.size()) - 1);
	} else {
		updateTable();
	}
}

void GridComplexConditionDialog::removeItem()
{
	if (! m_calculationConditionMode && m_widgets.size() == 1) {
		// The user tried to remove the last item.
		QMessageBox::warning(this, tr("Warning"), tr("There must be one group at least."));
		return;
	}

	int current = ui->listWidget->currentRow();
	m_groups.erase(m_groups.begin() + current);
	m_widgets.erase(m_widgets.begin() + current);

	updateList();
	if (m_widgets.size() == 0) {
		// removed the last one.
		QWidget* w = new QWidget(this);
		ui->widgetContainer->setWidget(w);
	} else {
		auto wSize = static_cast<int> (m_widgets.size());
		if (current >= wSize) {
			current = wSize - 1;
		}
		ui->listWidget->setCurrentRow(current);
	}
}

void GridComplexConditionDialog::updateCurrentName(const QString& name)
{
	ui->listWidget->item(ui->listWidget->currentRow())->setText(name);
}

void GridComplexConditionDialog::switchView()
{
	auto currentIndex = ui->stackedWidget->currentIndex();
	if (currentIndex == 0) {
		updateTable();
		ui->stackedWidget->setCurrentIndex(1);
	} else if (currentIndex == 1) {
		updateList();
		if (ui->listWidget->count() > 0) {
			ui->listWidget->setCurrentRow(0);
		}
		ui->stackedWidget->setCurrentIndex(0);
	}

	updateSwitchButtonText();
}

void GridComplexConditionDialog::importCsvFile()
{
	auto dir = LastIODirectory::get();
	auto fname = QFileDialog::getOpenFileName(this, tr("Select file to import"), dir, tr("CSV file (*.csv)"));
	if (fname.isNull()) {return;}

	bool ok = importFromCsvFile(fname, this);
	if (! ok) {
		QMessageBox::critical(this, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(fname)));
	}

	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

void GridComplexConditionDialog::exportCsvFile()
{
	auto dir = LastIODirectory::get();
	auto fname = QFileDialog::getSaveFileName(this, tr("Select file to export"), dir, tr("CSV file (*.csv)"));
	if (fname.isNull()) {return;}

	bool ok = exportToCsvFile(fname);
	if (! ok) {
		QMessageBox::critical(this, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(fname)));
	}

	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

void GridComplexConditionDialog::updateList()
{
	ui->listWidget->clear();
	for (const auto& w : m_widgets) {
		ui->listWidget->addItem(w->caption());
	}
}

void GridComplexConditionDialog::updateTable()
{
	for (int i = 0; i < static_cast<int> (m_groups.size()); ++i) {
		auto group = m_groups.at(i);
		auto widgets = group->tableWidgets();
		for (int j = 0; j < static_cast<int> (widgets.size()); ++j) {
			auto widget = widgets.at(j);
			widget->setParent(nullptr);
		}
	}

	auto oldWidget = ui->tableScrollContents->widget();

	auto newWidget = new QWidget(this);

	auto layout = new QGridLayout();
	layout->setSpacing(3);
	layout->setMargin(5);

	for (int i = 0; i < static_cast<int> (m_groups.size()); ++i) {
		auto group = m_groups.at(i);
		if (i == 0) {
			auto nameLabel = new QLabel(tr("Name"));
			layout->addWidget(nameLabel, 0, 0);
			auto labels = group->tableLabels();
			for (int j = 0; j < static_cast<int> (labels.size()); ++j) {
				QWidget* l = labels.at(j);
				if (l == nullptr) {
					l = new QLabel(newWidget);
				}
				layout->addWidget(l, 0, j + 1);
			}
		}
		auto nameEdit = new QLineEdit(newWidget);
		nameEdit->setText(group->caption());
		connect(nameEdit, &QLineEdit::textChanged, group.get(), &GridComplexConditionGroup::setCaption);
		layout->addWidget(nameEdit, i + 1, 0);

		auto widgets = group->tableWidgets();
		for (int j = 0; j < static_cast<int> (widgets.size()); ++j) {
			auto widget = widgets.at(j);
			widget->setParent(newWidget);
			layout->addWidget(widget, i + 1, j + 1);
		}
	}

	auto hLayout = new QHBoxLayout();
	hLayout->setMargin(0);
	hLayout->addLayout(layout);
	hLayout->addStretch();

	auto vLayout = new QVBoxLayout();
	vLayout->setMargin(0);
	vLayout->addLayout(hLayout);
	vLayout->addStretch();

	newWidget->setLayout(vLayout);
	ui->tableScrollContents->setWidget(newWidget);

	delete oldWidget;
}

void GridComplexConditionDialog::updateSwitchButtonText()
{
	QString text;
	if (ui->stackedWidget->currentIndex() == 0) {
		text = tr("Switch to Table view");
	} else if (ui->stackedWidget->currentIndex() == 1) {
		text = tr("Switch to List view");
	}
	ui->switchButton->setText(text);
}
