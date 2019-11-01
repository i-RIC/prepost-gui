#ifndef PROJECTMAINFILE_IMPL_H
#define PROJECTMAINFILE_IMPL_H

#include "../projectmainfile.h"

#include <guibase/timeformat/timeformat.h>
#include <misc/versionnumber.h>

#include <QDateTime>
#include <QPointF>

#include <string>

class ProjectMainFile::Impl
{
public:
	Impl(ProjectMainFile* parent);
	~Impl();

	void loadMeasuredDatas(const QDomNode& node);
	void saveMeasuredDatas(QXmlStreamWriter& writer);
	void clearMeasuredDatas();

	void loadBackgrounds(const QDomNode& node);
	void saveBackgrounds(QXmlStreamWriter& writer);
	QStringList backgroundImageFiles() const;
	void clearBackgroundImages();

	std::string m_solverName;
	VersionNumber m_solverVersion;
	VersionNumber m_iRICVersion;

	std::vector<MeasuredData*> m_measuredDatas;
	std::vector<BackgroundImageInfo*> m_backgroundImages;
	PostSolutionInfo* m_postSolutionInfo;
	ProjectPostProcessors* m_postProcessors;

	CoordinateSystem* m_coordinateSystem;
	QDateTime m_zeroDateTime; // time that corresponds to t = 0
	TimeFormat m_timeFormat;
	QString m_customTimeFormat;

	QPointF m_offset;

	bool m_isModified;

private:
	ProjectMainFile* m_parent;
};

#endif // PROJECTMAINFILE_IMPL_H
