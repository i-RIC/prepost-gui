#ifndef VALUESELECTDIALOG_H
#define VALUESELECTDIALOG_H

#include <QDialog>
#include <QString>

#include "misc_global.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace Ui {
class ValueSelectDialog;
}

class MISCDLL_EXPORT ValueSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ValueSelectDialog(QWidget *parent = nullptr);
	~ValueSelectDialog();

	void setValues(const std::unordered_map<std::string, QString>& values);
	std::string selectedValue() const;

private:
	std::vector<std::string> m_values;

	Ui::ValueSelectDialog *ui;
};

#endif // VALUESELECTDIALOG_H
