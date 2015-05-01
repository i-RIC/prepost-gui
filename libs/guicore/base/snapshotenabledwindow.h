#ifndef SNAPSHOTENABLEDWINDOW_H
#define SNAPSHOTENABLEDWINDOW_H

#include "../guicore_global.h"

#include <QPixmap>

class VTKGraphicsView;
class vtkRenderWindow;

/// This class is the abstract class of windows that support
/// taking snapshot as raster image.
class GUICOREDLL_EXPORT SnapshotEnabledWindow
{
public:
	/// Constructor
	SnapshotEnabledWindow();
	/// Take snapshot
	virtual QPixmap snapshot() = 0;
	virtual vtkRenderWindow* getVtkRenderWindow() {return 0;}
	virtual bool hasTransparentPart() {return false;}

	void setTransparent(bool b) {m_isTransparent = b;}
	bool transparent() const {return m_isTransparent;}

protected:
	bool m_isTransparent;

	void makeBackgroundTransparent(VTKGraphicsView* view, QPixmap& pixmap);
};

#endif // SNAPSHOTENABLEDWINDOW_H
