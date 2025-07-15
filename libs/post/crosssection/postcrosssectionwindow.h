#ifndef POSTCROSSSECTIONWINDOW_H
#define POSTCROSSSECTIONWINDOW_H

#include "postcrosssection_global.h"

#include <guicore/base/edgevisualizewindowi.h>
#include <guicore/post/postprocessorwindow.h>

class PostCrosssectionInternalWindow;

class POSTCROSSSECTION_EXPORT PostCrosssectionWindow : public PostProcessorWindow, public EdgeVisualizeWindowI
{
	Q_OBJECT

public:
	PostCrosssectionWindow(QWidget* parent, int index, PostProcessorWindowProjectDataItem* pdi);

	PostCrosssectionInternalWindow* internalWindow() const;

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	void updateEdgeFocus() override;

public slots:
	void cameraFit();

private:
	void setupDefaultGeometry(int index) override;

	PostCrosssectionInternalWindow* m_internalWindow;

	friend class PostCrosssectionInternalWindow;
};

#endif // POSTCROSSSECTIONWINDOW_H
