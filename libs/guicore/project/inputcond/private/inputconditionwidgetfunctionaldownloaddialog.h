#ifndef INPUTCONDITIONWIDGETFUNCTIONALDOWNLOADDIALOG_H
#define INPUTCONDITIONWIDGETFUNCTIONALDOWNLOADDIALOG_H

#include "../inputconditioncontainerfunctional.h"

#include <QDialog>

class QNetworkAccessManager;

namespace Ui {
class InputConditionWidgetFunctionalDownloadDialog;
}

class InputConditionWidgetFunctionalDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputConditionWidgetFunctionalDownloadDialog(QWidget *parent = 0);
    ~InputConditionWidgetFunctionalDownloadDialog();

	const InputConditionContainerFunctional& container() const;
	void setData(const InputConditionContainerFunctional& c);

public slots:
	void accept() override;

private slots:
	void startDateChanged(const QDateTime &dateTime);
	void endDateChanged(const QDateTime &dateTime);
	void textChanged(const QString &text);

	void test();

private:
	bool parse(const QString& xml);
	bool get(const QString& urlstr);
	QString generateURL();
	void setupData();

private:
    Ui::InputConditionWidgetFunctionalDownloadDialog *ui;
	InputConditionContainerFunctional m_container;
	QNetworkAccessManager *m_manager;
	bool m_needsUpdate;
	QString m_wml;
	QString m_get_errors;
	bool validate();
};

#endif // INPUTCONDITIONWIDGETFUNCTIONALDOWNLOADDIALOG_H
