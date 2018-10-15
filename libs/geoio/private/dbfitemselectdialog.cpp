#include "dbfitemselectdialog.h"
#include "ui_dbfitemselectdialog.h"

#include <misc/stringtool.h>

#include <QByteArray>
#include <QList>
#include <QTextCodec>

#include <shapefil.h>

#include <vector>

DBFItemSelectDialog::DBFItemSelectDialog(const QString& DBFFileName, QWidget *parent) :
	QDialog(parent),
	m_dbfFileName(iRIC::toStr(DBFFileName)),
	ui(new Ui::DBFItemSelectDialog)
{
	ui->setupUi(this);

	QList<QByteArray> codecs = QTextCodec::availableCodecs();
	qSort(codecs);
	for (const QByteArray& name : codecs) {
		ui->encodingComboBox->addItem(QString(name));
	}
	QTextCodec* defaultCodec = QTextCodec::codecForLocale();
	int index = codecs.indexOf(defaultCodec->name());
	ui->encodingComboBox->setCurrentIndex(index);

	connect(ui->encodingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTable()));

	updateTable();
}

DBFItemSelectDialog::~DBFItemSelectDialog()
{
	delete ui;
}

int DBFItemSelectDialog::selectedItem() const
{
	QList<QTableWidgetSelectionRange> ranges = ui->itemsTableWidget->selectedRanges();
	QTableWidgetSelectionRange range = ranges.first();

	return range.topRow();
}

void DBFItemSelectDialog::updateTable()
{
	QByteArray codecName = ui->encodingComboBox->currentText().toLatin1();
	QTextCodec* codec = QTextCodec::codecForName(codecName);

	DBFHandle dbfh = DBFOpen(m_dbfFileName.c_str(), "rb");

	int fieldCount = DBFGetFieldCount(dbfh);
	int recordCount = DBFGetRecordCount(dbfh);

	QStringList labels;
	std::vector<DBFFieldType> types;
	labels.push_back("ID");

	for (int i = 0; i < fieldCount; ++i) {
		DBFFieldType type;
		char fieldName[12];
		type = DBFGetFieldInfo(dbfh, i, fieldName, NULL, NULL);
		labels.append(codec->toUnicode(fieldName));
		types.push_back(type);
	}
	std::vector<std::vector<QVariant> > valArr;

	for (int i = 0; i < recordCount; ++i) {
		std::vector<QVariant> vals;
		for (int j = 0; j < fieldCount; ++j) {
			DBFFieldType type = types.at(j);
			QVariant val;
			if (type == FTString) {
				QString strval = codec->toUnicode(DBFReadStringAttribute(dbfh, i, j));
				val = strval;
			} else if (type == FTInteger) {
				int intval = DBFReadIntegerAttribute(dbfh, i, j);
				val = intval;
			} else if (type == FTDouble) {
				double doubleval = DBFReadDoubleAttribute(dbfh, i, j);
				val = doubleval;
			} else if (type == FTLogical) {
				QString logval = DBFReadLogicalAttribute(dbfh, i, j);
				val = logval;
			}
			vals.push_back(val);
		}
		valArr.push_back(vals);
	}
	DBFClose(dbfh);

	setLabels(labels);
	setData(valArr);

	ui->itemsTableWidget->selectRow(0);
}

void DBFItemSelectDialog::setLabels(const QStringList& labels)
{
	ui->itemsTableWidget->clear();
	ui->itemsTableWidget->setColumnCount(labels.count());
	ui->itemsTableWidget->setHorizontalHeaderLabels(labels);
}

void DBFItemSelectDialog::setData(const std::vector<std::vector<QVariant> >& data)
{
	ui->itemsTableWidget->setRowCount(static_cast<int>(data.size()) + 1);
	for (int i = 0; i < data.size(); ++i) {
		QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(i + 1));
		ui->itemsTableWidget->setItem(i, 0, item);

		std::vector<QVariant> d2 = data.at(i);
		for (int j = 0; j < d2.size(); ++j) {
			QVariant v = d2.at(j);
			QTableWidgetItem* item = new QTableWidgetItem(v.toString());
			ui->itemsTableWidget->setItem(i, j + 1, item);
		}
	}
}
