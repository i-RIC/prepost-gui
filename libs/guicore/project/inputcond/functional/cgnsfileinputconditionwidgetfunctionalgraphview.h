#ifndef CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALGRAPHVIEW_H
#define CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALGRAPHVIEW_H

#include <QAbstractItemView>
#include <QDomNode>

class CgnsFileInputConditionWidgetFunctionalGraphview : public QAbstractItemView
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetFunctionalGraphview(QWidget* w) : QAbstractItemView(w) {
		fLeftMargin = 0.1f;
		fRightMargin = 0.1f;
		fTopMargin = 0.1f;
		fBottomMargin = 0.1f;
	}
	void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void paintEvent(QPaintEvent* event) override;
	QRect visualRect(const QModelIndex&) const override {return QRect();}
	void scrollTo(const QModelIndex& /*index*/, ScrollHint /*hint*/) override {}
	QModelIndex indexAt(const QPoint&) const override {
		viewport()->update();
		return QModelIndex();
	}
	QModelIndex moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers) override {
		viewport()->update();
		return QModelIndex();
	}
	int horizontalOffset() const override {return 0;}
	int verticalOffset() const override {return 0;}
	bool isIndexHidden(const QModelIndex&) const override {return false;}
	void setSelection(const QRect& /*rect*/, QItemSelectionModel::SelectionFlags /*command*/) override {
		viewport()->update();
	}
	void selectionChanged(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/) override {
		viewport()->update();
	}
	QRegion visualRegionForSelection(const QItemSelection& /*selection*/) const override {return QRegion();}
private:
	const static int iLeftMargin = 20;
	const static int iRightMargin = 20;
	const static int iTopMargin = 20;
	const static int iBottomMargin = 20;

	const static int ellipseR = 4;

	double fLeftMargin;
	double fRightMargin;
	double fTopMargin;
	double fBottomMargin;

	QPointF getPointF(double x, double y, const QRect& viewport);
	QRectF getRegion();
	QMatrix getMatrix(QRect& viewport);
	void drawRegion(QPainter& painter, const QRectF& region, const QMatrix& matrix);
};
#endif //CGNSFILEINPUTCONDITIONWIDGETFUNCTIONALGRAPHVIEW_H
