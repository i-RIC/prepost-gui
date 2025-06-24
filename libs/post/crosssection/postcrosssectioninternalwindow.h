#ifndef POSTCROSSSECTIONINTERNALWINDOW_H
#define POSTCROSSSECTIONINTERNALWINDOW_H

#include "postcrosssection_global.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <crosssection/abstractcrosssectionwindow.h>

class PostCrosssectionWindow;

class POSTCROSSSECTION_EXPORT PostCrosssectionInternalWindow : public AbstractCrosssectionWindow
{
public:
	PostCrosssectionInternalWindow(QWidget* parent);
	bool setupInitialSetting();

	void loadFromProjectMainFile(const QDomNode& node) override;
	void saveToProjectMainFile(QXmlStreamWriter& writer) override;

	QMdiSubWindow* mdiSubWindow() const override;
	v4Grid* grid() override;
	v4Grid* additionalGrid() override;
	QString additionalGridPrefix() override;
	void updateEdgeFocus() override;

private:
	PostCrosssectionWindow* window() const;

	PostSolutionInfo::Dimension m_dimension;
	std::string m_zoneName;
};

#endif // POSTCROSSSECTIONINTERNALWINDOW_H
