#ifndef GEODATAPOINTMAPREALTEXTIMPORTER_LINEPARSER_H
#define GEODATAPOINTMAPREALTEXTIMPORTER_LINEPARSER_H

#include "../geodatapointmaprealtextimporter.h"
#include "geodatapointmaprealtextimporter_values.h"

class QTextCodec;

class GeoDataPointmapRealTextImporter::LineParser
{
public:
	LineParser();

	void setTextCodec(QTextCodec* codec);
	void setDelimiters(std::vector<QChar>& delimiters);
	void setQuoteChars(std::vector<QChar>& quotes);
	void setEscapeChar(const QChar escape);
	void setHeaderLines(int lines);
	void setIsFirstLineFieldNames(bool isFieldNames);
	void setXColumn(int xcol);
	void setYColumn(int ycol);
	void setValueColumn(int vcol);
	void setSkipRate(int rate);

	QTextCodec* textCodec() const;
	int headerLines() const;
	int skipRate() const;

	QStringList parseToStrs(const QString& line, bool* ok, QString* error);
	Values parseToValues(const QString& line, bool* ok, QString* error);

private:
	bool isDelimiter(const QChar& c);
	bool isQuote(const QChar& c);

	QTextCodec* m_textCodec;
	std::vector<QChar> m_delimiters;
	std::vector<QChar> m_quoteChars;
	QChar m_escapeChar;
	int m_headerLines;
	bool m_isFirstLineFieldNames;
	int m_xColumn;
	int m_yColumn;
	int m_valueColumn;
	int m_skipRate;
};

inline bool GeoDataPointmapRealTextImporter::LineParser::isDelimiter(const QChar& c)
{
	for (const auto& delim : m_delimiters) {
		if (c == delim) {return true;}
	}
	return false;
}

inline bool GeoDataPointmapRealTextImporter::LineParser::isQuote(const QChar& c)
{
	for (const auto& q : m_quoteChars) {
		if (c == q) {return true;}
	}
	return false;
}

#endif // GEODATAPOINTMAPREALTEXTIMPORTER_LINEPARSER_H
