#ifndef MODIFYCOMMANDDIALOG_H
#define MODIFYCOMMANDDIALOG_H

#include "misc_global.h"

#include <QDialog>

namespace Ui {
class ModifyCommandDialog;
}

class ModifyCommandWidget;

class QAbstractButton;

class MISCDLL_EXPORT ModifyCommandDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ModifyCommandDialog(QWidget *parent = nullptr);
	~ModifyCommandDialog();

	ModifyCommandWidget* widget() const;
	void setWidget(ModifyCommandWidget* widget);

	void showImportButton();
	void showExportButton();

	void accept() override;
	void reject() override;

signals:
	void importClicked();
	void exportClicked();

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	virtual void pushCommand(QUndoCommand* command) = 0;

private:
	bool m_applied;

	ModifyCommandWidget* m_widget;
	Ui::ModifyCommandDialog *ui;
};

#endif // MODIFYCOMMANDDIALOG_H
