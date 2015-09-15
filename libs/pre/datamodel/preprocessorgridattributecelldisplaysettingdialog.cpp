#include "ui_preprocessorgridattributecelldisplaysettingdialog.h"
#include "preprocessorgridattributecelldisplaysettingdialog.h"

#include <guibase/coloreditwidget.h>

#include <QColorDialog>
#include <QItemDelegate>
#include <QPainter>
#include <QTableWidgetItem>

class PreProcessorGridAttributeCellDisplaySettingDialogColorEditDelegate : public QItemDelegate
{
public:
	PreProcessorGridAttributeCellDisplaySettingDialogColorEditDelegate(QObject* parent = nullptr): QItemDelegate(parent) {}
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


PreProcessorGridAttributeCellDisplaySettingDialog::PreProcessorGridAttributeCellDisplaySettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PreProcessorGridAttributeCellDisplaySettingDialog}
{
	ui->setupUi(this);

	QStringList hlabels;
	hlabels << PreProcessorGridAttributeCellDisplaySettingDialog::tr("Attribute");
	hlabels << PreProcessorGridAttributeCellDisplaySettingDialog::tr("Color");

	ui->tableWidget->setColumnCount(2);
	ui->tableWidget->setColumnWidth(0, ATTRWIDTH);
	ui->tableWidget->setColumnWidth(1, COLORWIDTH);
	ui->tableWidget->setHorizontalHeaderLabels(hlabels);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	PreProcessorGridAttributeCellDisplaySettingDialogColorEditDelegate* deleg = new PreProcessorGridAttributeCellDisplaySettingDialogColorEditDelegate(this);
	ui->tableWidget->setItemDelegateForColumn(1, deleg);

	connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleItemEdit(QTableWidgetItem*)));
	connect(ui->tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(handleItemClick(QTableWidgetItem*)));
	connect(ui->tableWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(handleShownSelectionChange(QItemSelection,QItemSelection)));
}

PreProcessorGridAttributeCellDisplaySettingDialog::~PreProcessorGridAttributeCellDisplaySettingDialog()
{
	delete ui;
}


void PreProcessorGridAttributeCellDisplaySettingDialog::setupDialog()
{
	/*
		// create list.
		ui->tableWidget->clearContents();
		ui->tableWidget->setRowCount(0);
		for (auto it2 = m_settings.begin(); it2 != m_settings.end(); ++it2){
			ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
			int rownum = ui->tableWidget->rowCount() - 1;

			Post2dWindowCellFlagSetting s = *it2;
			const SolverDefinitionPreProcessorGridRelatedCondition* cond = m_gridType->PreProcessorGridRelatedCondition(s.attributeName);
			const IntegerEnumLoader* el = dynamic_cast<const IntegerEnumLoader*>(cond);
			QMap<int, QString> enums = el->enumerations();
			QTableWidgetItem* witem = new QTableWidgetItem();
			QString caption = QString("%1 (%2)").arg(cond->caption(), enums.value(s.value));
			witem->setData(Qt::DisplayRole, caption);
			Qt::ItemFlags flags = witem->flags();
			flags &= ~Qt::ItemIsEditable;
	//		flags |= Qt::ItemIsUserCheckable;
	//		flags &= ~Qt::ItemIsTristate;
			witem->setFlags(flags);
			ui->tableWidget->setItem(rownum, 0, witem);

			witem = new QTableWidgetItem();
			witem->setData(Qt::DisplayRole, s.color);
			witem->setData(Qt::BackgroundRole, s.color);
			ui->tableWidget->setItem(rownum, 1, witem);
		}
	*/
}

/*
const QList<Post2dWindowCellFlagSetting>& Post2dWindowCellFlagSettingDialog::settings()
{
	return m_settings;
}
*/

void PreProcessorGridAttributeCellDisplaySettingDialog::handleItemEdit(QTableWidgetItem* /*item*/)
{
	/*
		Post2dWindowCellFlagSetting& s = m_settings[item->row()];
		if (item->column() == 0){
			// checkstate changed.
	//		s.enabled = (item->checkState() == Qt::Checked);
		}else if (item->column() == 1){
			QColor col = item->data(Qt::DisplayRole).value<QColor>();
			s.color = col;
		}
	*/
}

void PreProcessorGridAttributeCellDisplaySettingDialog::handleItemClick(QTableWidgetItem* item)
{
	if (item->column() != 1) {return;}
	QColor col = item->data(Qt::DisplayRole).value<QColor>();
	QColor newcolor = QColorDialog::getColor(col, this);
	if (! newcolor.isValid()) {return;}
	item->setData(Qt::DisplayRole, newcolor);
}

void PreProcessorGridAttributeCellDisplaySettingDialog::handleShownSelectionChange(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
	QSet<int> rows;
//	int lastrow = 0;
	for (auto it = selected.begin(); it != selected.end(); ++it) {
		QItemSelectionRange range = *it;
		for (int row = range.top(); row <= range.bottom(); ++row) {
			rows.insert(row);
//			lastrow = row;
		}
	}
}
