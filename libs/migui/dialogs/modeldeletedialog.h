#ifndef MODELDELETEDIALOG_H
#define MODELDELETEDIALOG_H

#include <QDialog>

namespace Ui {
class ModelDeleteDialog;
}

class ModelDeleteDialog : public QDialog
{
	Q_OBJECT

public:
	enum class DeleteMode {
		Keep,
		Delete
	};
	explicit ModelDeleteDialog(QWidget *parent = nullptr);
	~ModelDeleteDialog();

	void setFolder(const QString& folder);

	DeleteMode deleteMode() const;

private:
	Ui::ModelDeleteDialog *ui;
};

#endif // MODELDELETEDIALOG_H
