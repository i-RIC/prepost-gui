#ifndef V4POSTCALCULATEDRESULTLISTDIALOG_H
#define V4POSTCALCULATEDRESULTLISTDIALOG_H

#include "../guicore_global.h"

#include <QDialog>

namespace Ui {
class v4PostCalculatedResultListDialog;
}

class v4PostZoneDataContainer;

class GUICOREDLL_EXPORT v4PostCalculatedResultListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit v4PostCalculatedResultListDialog(QWidget *parent = nullptr);
	~v4PostCalculatedResultListDialog();

	void setZoneDataContainer(v4PostZoneDataContainer* container);

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

	v4PostZoneDataContainer* m_zoneDataContainer;
	Ui::v4PostCalculatedResultListDialog *ui;
};

#endif // V4POSTCALCULATEDRESULTLISTDIALOG_H
