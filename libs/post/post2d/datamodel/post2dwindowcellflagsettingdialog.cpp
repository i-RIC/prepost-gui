#include "ui_post2dwindowcellflagsettingdialog.h"

#include "post2dwindowcellflagsettingdialog.h"

#include <guibase/widget/coloreditwidget.h>
#include <guicore/project/colorsource.h>
#include <guicore/solverdef/integerenumloader.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/opacitycontainer.h>

#include <QBrush>
#include <QColorDialog>
#include <QItemDelegate>
#include <QItemSelection>
#include <QMap>
#include <QPainter>
#include <QSet>
#include <QTableWidgetItem>

class ColorEditDelegate : public QItemDelegate
{
public:
	ColorEditDelegate(QObject* parent = nullptr): QItemDelegate(parent) {}
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		QColor col = dat.value<QColor>();
		QBrush brush(col);
		painter->fillRect(option.rect, brush);
	}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
		ColorEditWidget* w = new ColorEditWidget(parent);
		return w;
	}
	void setEditorData(QWidget* editor, const QModelIndex& index) const {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		ColorEditWidget* w = static_cast<ColorEditWidget*>(editor);
		w->setColor(dat.value<QColor>());
	}
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
		ColorEditWidget* w = static_cast<ColorEditWidget*>(editor);
		QColor c = w->color();
		model->setData(index, c, Qt::DisplayRole);
		model->setData(index, c, Qt::BackgroundRole);
	}
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const {
		editor->setGeometry(option.rect);
	}
};

Post2dWindowCellFlagSettingDialog::Post2dWindowCellFlagSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowCellFlagSettingDialog)
{
	ui->setupUi(this);

	ui->upButton->setEnabled(false);
	ui->downButton->setEnabled(false);

	QStringList hlabels;
	hlabels << tr("Attribute");
	hlabels << tr("Color");

	ui->tableWidget->setColumnCount(2);
	ui->tableWidget->setColumnWidth(0, ATTRWIDTH);
	ui->tableWidget->setColumnWidth(1, COLORWIDTH);
	ui->tableWidget->setHorizontalHeaderLabels(hlabels);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ColorEditDelegate* deleg = new ColorEditDelegate(this);
	ui->tableWidget->setItemDelegateForColumn(1, deleg);

	connect(ui->upButton, SIGNAL(clicked()), this, SLOT(moveUp()));
	connect(ui->downButton, SIGNAL(clicked()), this, SLOT(moveDown()));
	connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleItemEdit(QTableWidgetItem*)));
	connect(ui->tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(handleItemClick(QTableWidgetItem*)));
	connect(ui->tableWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(handleShownSelectionChange(QItemSelection,QItemSelection)));
}

Post2dWindowCellFlagSettingDialog::~Post2dWindowCellFlagSettingDialog()
{
	delete ui;
}

void Post2dWindowCellFlagSettingDialog::setGridType(SolverDefinitionGridType* gt)
{
	m_gridType = gt;
}

void Post2dWindowCellFlagSettingDialog::setSettings(const QList<Post2dWindowCellFlagSetting>& settings)
{
	m_settings = settings;
	setupDialog();
}

void Post2dWindowCellFlagSettingDialog::setupDialog()
{
	// create list.
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
	for (auto it2 = m_settings.begin(); it2 != m_settings.end(); ++it2) {
		ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
		int rownum = ui->tableWidget->rowCount() - 1;

		Post2dWindowCellFlagSetting s = *it2;
		const SolverDefinitionGridAttribute* cond = m_gridType->gridAttribute(s.attributeName);
		const IntegerEnumLoader* el = dynamic_cast<const IntegerEnumLoader*>(cond);
		QMap<int, QString> enums = el->enumerations();
		QTableWidgetItem* witem = new QTableWidgetItem();
		QString caption = QString("%1 (%2)").arg(cond->caption(), enums.value(s.value));
		witem->setData(Qt::DisplayRole, caption);
		Qt::ItemFlags flags = witem->flags();
		flags &= ~Qt::ItemIsEditable;
		witem->setFlags(flags);
		ui->tableWidget->setItem(rownum, 0, witem);

		witem = new QTableWidgetItem();
		witem->setData(Qt::DisplayRole, s.color);
		witem->setData(Qt::BackgroundRole, s.color);
		ui->tableWidget->setItem(rownum, 1, witem);
	}
}

const QList<Post2dWindowCellFlagSetting>& Post2dWindowCellFlagSettingDialog::settings() const
{
	return m_settings;
}

void Post2dWindowCellFlagSettingDialog::setOpacity(const OpacityContainer& o)
{
	ui->transparencyWidget->setOpacity(o);
}

OpacityContainer Post2dWindowCellFlagSettingDialog::opacity() const
{
	return ui->transparencyWidget->opacity();
}

void Post2dWindowCellFlagSettingDialog::moveUp()
{
	QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
	QTableWidgetItem* item = nullptr;
	int newrow = 0;
	for (int i = 0; i < items.count(); ++i) {
		item = items.at(i);
		if (item->column() == 0) {
			Post2dWindowCellFlagSetting s = m_settings.takeAt(item->row());
			int insertTo = item->row() - 1;
			if (insertTo < 0) {insertTo = 0;}
			m_settings.insert(insertTo, s);
			newrow = insertTo;
		}
	}
	setupDialog();
	ui->tableWidget->selectRow(newrow);
}

void Post2dWindowCellFlagSettingDialog::moveDown()
{
	QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
	QTableWidgetItem* item = nullptr;
	int newrow = 0;
	for (int i = 0; i < items.count(); ++i) {
		item = items.at(i);
		if (item->column() == 0) {
			Post2dWindowCellFlagSetting s = m_settings.takeAt(item->row());
			int insertTo = item->row() + 1;
			if (insertTo >= m_settings.count()) {
				m_settings.append(s);
			} else {
				m_settings.insert(insertTo, s);
			}
			newrow = insertTo;
			if (newrow >= m_settings.count()) {
				newrow = m_settings.count() - 1;
			}
		}
	}
	setupDialog();
	ui->tableWidget->selectRow(newrow);
}

void Post2dWindowCellFlagSettingDialog::handleItemEdit(QTableWidgetItem* item)
{
	Post2dWindowCellFlagSetting& s = m_settings[item->row()];
	if (item->column() == 0) {
		// checkstate changed.
//		s.enabled = (item->checkState() == Qt::Checked);
	} else if (item->column() == 1) {
		QColor col = item->data(Qt::DisplayRole).value<QColor>();
		s.color = col;
	}
}

void Post2dWindowCellFlagSettingDialog::handleItemClick(QTableWidgetItem* item)
{
	if (item->column() != 1) {return;}
	QColor col = item->data(Qt::DisplayRole).value<QColor>();
	QColor newcolor = QColorDialog::getColor(col, this);
	if (! newcolor.isValid()) {return;}
	item->setData(Qt::DisplayRole, newcolor);
}

void Post2dWindowCellFlagSettingDialog::handleShownSelectionChange(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
	QSet<int> rows;
	int lastrow = 0;
	for (auto it = selected.begin(); it != selected.end(); ++it) {
		QItemSelectionRange range = *it;
		for (int row = range.top(); row <= range.bottom(); ++row) {
			rows.insert(row);
			lastrow = row;
		}
	}
	ui->upButton->setEnabled(rows.count() == 1 && (lastrow != 0));
	ui->downButton->setEnabled(rows.count() == 1 && (lastrow != m_settings.count() - 1));
}
