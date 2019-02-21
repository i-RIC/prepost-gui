#ifndef POSTCALCULATEDRESULTLISTDIALOG_H
#define POSTCALCULATEDRESULTLISTDIALOG_H

#include "../guicore_global.h"

#include <QDialog>

namespace Ui {
class PostCalculatedResultListDialog;
}

class PostZoneDataContainer;

class GUICOREDLL_EXPORT PostCalculatedResultListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostCalculatedResultListDialog(QWidget *parent = nullptr);
	~PostCalculatedResultListDialog();

	void setZoneDataContainer(PostZoneDataContainer* container);

public slots:
	void reject() override;

private slots:
	void add();
	void editSelected();
	void deleteSelected();

	void moveUpSelected();
	void moveDownSelected();

private:
	void updateTable();

	PostZoneDataContainer* m_zoneDataContainer;
	Ui::PostCalculatedResultListDialog *ui;
};

#endif // POSTCALCULATEDRESULTLISTDIALOG_H
