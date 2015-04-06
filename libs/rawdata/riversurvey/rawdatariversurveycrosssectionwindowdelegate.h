#ifndef RAWDATARIVERSURVEYCROSSSECTIONWINDOWDELEGATE_H
#define RAWDATARIVERSURVEYCROSSSECTIONWINDOWDELEGATE_H

#include <QStyledItemDelegate>
class RawDataRiverCrosssection;
class CenteredCheckBox;

class RawDataRiverSurveyCrosssectionWindowDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	RawDataRiverSurveyCrosssectionWindowDelegate();
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setCrosssection(RawDataRiverCrosssection* c){m_crosssection = c;}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget* editor() const {return dynamic_cast<QWidget*>(m_editor);}
private slots:
	void handleEditorDestroy(QObject* editor);
	void handleCheckboxToggle(bool toggled);
private:
	RawDataRiverCrosssection* m_crosssection;
	mutable QObject* m_editor;
	mutable CenteredCheckBox* m_checkBox;
	mutable QAbstractItemModel* m_model;
	mutable QModelIndex m_index;
};

#endif // RAWDATARIVERSURVEYCROSSSECTIONWINDOWDELEGATE_H
