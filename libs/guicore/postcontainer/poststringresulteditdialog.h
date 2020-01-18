#ifndef POSTSTRINGRESULTEDITDIALOG_H
#define POSTSTRINGRESULTEDITDIALOG_H

#include "../guicore_global.h"

#include <QDialog>

namespace Ui {
class PostStringResultEditDialog;
}

class PostStringResult;
class PostZoneDataContainer;

class GUICOREDLL_EXPORT PostStringResultEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostStringResultEditDialog(QWidget *parent = 0);
	~PostStringResultEditDialog();

	void setZoneDataContainer(PostZoneDataContainer* container);

	PostStringResult* stringResult() const;
	void setStringResult(PostStringResult* result);

public slots:
	void accept() override;

private slots:
	void addArgument();
	void editSelectedArgument();
	void deleteSelectedArgument();

	void test();

private:
	bool check();
	void applySetting();
	void updateArgumentsTable();
	QString setupType(int row);
	QString setupIndex(int row);

	PostStringResult* m_stringResult;
	bool m_rejected;
	PostZoneDataContainer* m_zoneDataContainer;

	Ui::PostStringResultEditDialog *ui;
};

#endif // POSTSTRINGRESULTEDITDIALOG_H
