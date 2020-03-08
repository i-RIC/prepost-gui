#ifndef POSTSTRINGRESULTEDITWIDGET_H
#define POSTSTRINGRESULTEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class PostStringResultEditWidget;
}

class PostStringResult;
class PostZoneDataContainer;

class GUICOREDLL_EXPORT PostStringResultEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PostStringResultEditWidget(QWidget *parent = nullptr);
	~PostStringResultEditWidget();

	void setZoneDataContainer(PostZoneDataContainer* container);

	PostStringResult* stringResult() const;
	void setStringResult(PostStringResult* result);

	bool check();

private slots:
	void addArgument();
	void editSelectedArgument();
	void deleteSelectedArgument();

	void test();

private:
	void applySetting();
	void updateArgumentsTable();
	QString setupType(int row);
	QString setupIndex(int row);

	PostStringResult* m_stringResult;
	bool m_rejected;
	PostZoneDataContainer* m_zoneDataContainer;

	Ui::PostStringResultEditWidget *ui;
};

#endif // POSTSTRINGRESULTEDITWIDGET_H
