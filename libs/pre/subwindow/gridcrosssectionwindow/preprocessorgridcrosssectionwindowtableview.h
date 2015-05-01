#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOWTABLEVIEW_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOWTABLEVIEW_H

#include <QTableView>

class PreProcessorGridCrosssectionWindowTableView : public QTableView
{
	Q_OBJECT
public:
	explicit PreProcessorGridCrosssectionWindowTableView(QWidget* parent = 0);
protected:
	void resizeEvent(QResizeEvent* event);
private:
	QModelIndex m_editIndex;
	const static int indexWidth = 50;
	const static int columnMargin = 0;
	QObject* m_editor;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOWTABLEVIEW_H
