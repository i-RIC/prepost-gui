#ifndef QMAINWINDOWWITHSNAPSHOT_H
#define QMAINWINDOWWITHSNAPSHOT_H

#include "../guicore_global.h"

#include <QMainWindow>

class vtkRenderWindow;
class VTKGraphicsView;

class QMdiSubWindow;

class GUICOREDLL_EXPORT QMainWindowWithSnapshot : public QMainWindow
{
	Q_OBJECT

public:
	QMainWindowWithSnapshot(QWidget* parent);

	virtual QPixmap snapshot() const = 0;
	virtual QWidget* snapshotArea() const = 0;

	virtual vtkRenderWindow* getVtkRenderWindow() const;
	virtual bool hasTransparentPart() const;
	void resizeSnapshotAreaTo(const QSize& size, QMdiSubWindow* parent);

	bool isTransparent() const;
	void setTransparent(bool b);

signals:
	void snapshotAreaResized(const QSize& size);

protected:
	void resizeEvent(QResizeEvent *event);

	static void makeBackgroundTransparent(VTKGraphicsView* view, QPixmap& pixmap);

	bool m_isTransparent;
};

#endif // QMAINWINDOWWITHSNAPSHOT_H
