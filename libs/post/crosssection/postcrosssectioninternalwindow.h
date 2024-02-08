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

	v4Structured2dGrid* grid() override;
	v4Structured2dGrid* additionalGrid() override;
	QString additionalGridPrefix() override;

private:
	PostCrosssectionWindow* window() const;

	PostSolutionInfo::Dimension m_dimension;
	std::string m_zoneName;
};

#endif // POSTCROSSSECTIONINTERNALWINDOW_H
