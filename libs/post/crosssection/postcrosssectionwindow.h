#ifndef POSTCROSSSECTIONWINDOW_H
#define POSTCROSSSECTIONWINDOW_H

#include "postcrosssection_global.h"

#include <guicore/post/postprocessorwindow.h>

class PostCrosssectionInternalWindow;

class PostCrosssectionWindow : public PostProcessorWindow
{
	Q_OBJECT

public:
	PostCrosssectionWindow(QWidget* parent, int index, PostProcessorWindowProjectDataItem* pdi);

	PostCrosssectionInternalWindow* internalWindow() const;

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;

public slots:
	void cameraFit();

private:
	void setupDefaultGeometry(int index) override;

	PostCrosssectionInternalWindow* m_internalWindow;

	friend class PostCrosssectionInternalWindow;
};

#endif // POSTCROSSSECTIONWINDOW_H
