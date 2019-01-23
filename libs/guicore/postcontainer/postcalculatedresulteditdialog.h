#ifndef POSTCALCULATEDRESULTEDITDIALOG_H
#define POSTCALCULATEDRESULTEDITDIALOG_H

#include "postcalculatedresult.h"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class PostCalculatedResultEditDialog;
}

class PostZoneDataContainer;

class PostCalculatedResultEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostCalculatedResultEditDialog(QWidget *parent = nullptr);
	~PostCalculatedResultEditDialog();

	void setZoneDataContainer(PostZoneDataContainer* container);

	PostCalculatedResult* calculatedResult() const;
	void setCalculatedResult(PostCalculatedResult* result);
	void setCalculatedResultIndex(int index);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void addArgument();
	void deleteSelectedArgument();
	void handlePositionChange();

	void test();

private:
	bool check();
	void applySetting();
	void updateArgumentsTable();

	PostCalculatedResult* m_calculatedResult;
	int m_calculatedResultIndex;
	bool m_rejected;
	PostZoneDataContainer* m_zoneDataContainer;
	std::vector<PostCalculatedResult::DataType> m_dataTypes;

	QStandardItemModel m_model;

	Ui::PostCalculatedResultEditDialog *ui;

	class TableDelegate;
	TableDelegate* m_tableDelegate;
};

#endif // POSTCALCULATEDRESULTEDITDIALOG_H
