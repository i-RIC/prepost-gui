#ifndef PROJECTMAINFILE_IMPL_H
#define PROJECTMAINFILE_IMPL_H

#include "../projectmainfile.h"

#include <guibase/timeformat/timeformat.h>
#include <misc/versionnumber.h>

#include <QDateTime>
#include <QPointF>
#include <QTimeZone>

#include <string>

class ProjectMainFile::Impl
{
public:
	Impl(ProjectData* data, ProjectMainFile* parent);
	~Impl();

	void loadMeasuredDatas(const QDomNode& node);
	void saveMeasuredDatas(QXmlStreamWriter& writer);
	void clearMeasuredDatas();

	void loadBackgrounds(const QDomNode& node);
	void saveBackgrounds(QXmlStreamWriter& writer);
	QStringList backgroundImageFiles() const;
	void clearBackgroundImages();
	void deleteGarbageBackgroundImages();

	std::string m_solverName;
	VersionNumber m_solverVersion;
	VersionNumber m_iRICVersion;

	ProjectCgnsManager* m_cgnsManager;
	std::vector<MeasuredData*> m_measuredDatas;
	std::vector<BackgroundImageInfo*> m_backgroundImages;
	PostSolutionInfo* m_postSolutionInfo;
	ProjectPostProcessors* m_postProcessors;
	ProjectData* m_projectData;

	CoordinateSystem* m_coordinateSystem;
	QDateTime m_zeroDateTime; // time that corresponds to t = 0
	QTimeZone m_timeZone;

	TimeFormat m_timeFormat;
	QString m_customTimeFormat;
	bool m_showTimeZone;

	bool m_separateResult;
	bool m_geoDataLink;

	QPointF m_offset;

	bool m_isModified;

	iRICLib::H5CgnsFile* m_cgnsFile;

private:
	ProjectMainFile* m_parent;
};

#endif // PROJECTMAINFILE_IMPL_H
