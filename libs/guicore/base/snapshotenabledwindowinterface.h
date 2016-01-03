#ifndef SNAPSHOTENABLEDWINDOWINTERFACE_H
#define SNAPSHOTENABLEDWINDOWINTERFACE_H

#include "../guicore_global.h"

class QPixmap;
class VTKGraphicsView;
class vtkRenderWindow;

/// This class is the abstract class of windows that support
/// taking snapshot as raster image.
class GUICOREDLL_EXPORT SnapshotEnabledWindowInterface
{

public:
	SnapshotEnabledWindowInterface();
	virtual ~SnapshotEnabledWindowInterface();

	virtual QPixmap snapshot() = 0;
	virtual vtkRenderWindow* getVtkRenderWindow() const;
	virtual bool hasTransparentPart() const;

	bool isTransparent() const;
	void setTransparent(bool b);

protected:
	static void makeBackgroundTransparent(VTKGraphicsView* view, QPixmap& pixmap);

	bool m_isTransparent;
};

#endif // SNAPSHOTENABLEDWINDOWINTERFACE_H
