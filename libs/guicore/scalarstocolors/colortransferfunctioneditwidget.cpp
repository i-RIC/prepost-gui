#include "colortransferfunctioneditwidget.h"
#include "ui_colortransferfunctioneditwidget.h"
#include "colortransferfunctioncontainer.h"

#include <guibase/coloreditwidget.h>

#include <QStringList>
#include <QItemDelegate>
#include <QColorDialog>
#include <QPainter>

class ColorTransferFunctionEditWidgetColorEditDelegate : public QItemDelegate
{
public:
	ColorTransferFunctionEditWidgetColorEditDelegate(QObject* parent = nullptr): QItemDelegate(parent){}
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		QColor col = dat.value<QColor>();
		QBrush brush(col);
		painter->fillRect(option.rect, brush);
	}
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const {
		ColorEditWidget* w = new ColorEditWidget(parent);
		return w;
	}
	void setEditorData(QWidget *editor, const QModelIndex &index) const {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		ColorEditWidget* w = static_cast<ColorEditWidget*>(editor);
		w->setColor(dat.value<QColor>());
	}
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
		ColorEditWidget* w = static_cast<ColorEditWidget*>(editor);
		QColor c = w->color();
		model->setData(index, c, Qt::DisplayRole);
		model->setData(index, c, Qt::BackgroundRole);
	}
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & /*index*/) const {
		editor->setGeometry(option.rect);
	}
};

ColorTransferFunctionEditWidget::ColorTransferFunctionEditWidget(QWidget *parent) :
		ScalarsToColorsEditWidget(parent),
		ui(new Ui::ColorTransferFunctionEditWidget)
{
		ui->setupUi(this);

	QStringList hlabels;
	hlabels << tr("Attribute");
	hlabels << tr("Color");

	ui->tableWidget->setColumnCount(2);
	ui->tableWidget->setColumnWidth(0, ATTRWIDTH);
	ui->tableWidget->setColumnWidth(1, COLORWIDTH);
	ui->tableWidget->setHorizontalHeaderLabels(hlabels);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ColorTransferFunctionEditWidgetColorEditDelegate* deleg = new ColorTransferFunctionEditWidgetColorEditDelegate(this);
	ui->tableWidget->setItemDelegateForColumn(1, deleg);
	connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleItemEdit(QTableWidgetItem*)));
	connect(ui->tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(handleItemClick(QTableWidgetItem*)));
}

ColorTransferFunctionEditWidget::~ColorTransferFunctionEditWidget()
{
		delete ui;
}

void ColorTransferFunctionEditWidget::setupWidget()
{
	ColorTransferFunctionContainer* cont = dynamic_cast<ColorTransferFunctionContainer*>(m_container);
	QMap<double, QString> enums = cont->enumerations();
	m_colors.clear();
	ui->tableWidget->blockSignals(true);
	for (auto it = enums.begin(); it != enums.end(); ++it){
		ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
		int rownum = ui->tableWidget->rowCount() - 1;

		QTableWidgetItem* witem = new QTableWidgetItem();
		witem->setData(Qt::DisplayRole, it.value());
		Qt::ItemFlags flags = witem->flags();
		flags &= ~Qt::ItemIsEditable;
		witem->setFlags(flags);
		ui->tableWidget->setItem(rownum, 0, witem);

		witem = new QTableWidgetItem();
		QColor col = cont->colors().value(it.key());
		witem->setData(Qt::DisplayRole, col);
		witem->setData(Qt::BackgroundRole, col);
		ui->tableWidget->setItem(rownum, 1, witem);
		m_colors.append(col);
	}
	ui->tableWidget->blockSignals(false);
}

void ColorTransferFunctionEditWidget::save()
{
	ColorTransferFunctionContainer* cont = dynamic_cast<ColorTransferFunctionContainer*>(m_container);
	QMap<double, QString> enums = cont->enumerations();
	QMap<double, QColor> colors;

	int index = 0;
	for (auto it = enums.begin(); it != enums.end(); ++it, ++index){
		colors.insert(it.key(), m_colors.at(index));
	}
	cont->setColors(colors);
}

void ColorTransferFunctionEditWidget::handleItemEdit(QTableWidgetItem * item)
{
	if (item->column() == 1){
		QColor col = item->data(Qt::DisplayRole).value<QColor>();
		m_colors[item->row()] = col;
	}
}

void ColorTransferFunctionEditWidget::handleItemClick(QTableWidgetItem * item)
{
	if (item->column() != 1){return;}
	QColor col = item->data(Qt::DisplayRole).value<QColor>();
	QColor newcolor = QColorDialog::getColor(col, this);
	if (! newcolor.isValid()){return;}
	item->setData(Qt::DisplayRole, newcolor);
}
