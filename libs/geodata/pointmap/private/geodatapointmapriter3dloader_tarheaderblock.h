#ifndef GEODATAPOINTMAPRITER3DLOADER_TARHEADERBLOCK_H
#define GEODATAPOINTMAPRITER3DLOADER_TARHEADERBLOCK_H

#include "../geodatapointmapriter3dloader.h"

namespace Poco {
class BinaryReader;
class BinaryWriter;
}

class GeoDataPointmapRiter3dLoader::TarHeaderBlock
{
public:
	TarHeaderBlock();
	~TarHeaderBlock();

	void read(Poco::BinaryReader* reader);
	void write(Poco::BinaryWriter* writer);

	std::string name();
	void setName(std::string name);
	unsigned int mode();
	void setMode(unsigned int mode);
	std::string uid();
	void setUid(std::string uid);
	std::string gid();
	void setGid(std::string gid);
	unsigned int size();
	void setSize(unsigned int size);
	unsigned int mtime();
	void setMtime(unsigned int mtime);
	unsigned int chksum();
	void setChksum(unsigned int chksum);
	std::string typeflag();
	void setTypeflag(std::string typeflag);
	std::string linkname();
	void setLinkname(std::string linkname);
	std::string magic();
	void setMagic(std::string magic);
	std::string version();
	void setVersion(std::string version);
	std::string uname();
	void setUname(std::string uname);
	std::string gname();
	void setGname(std::string gname);
	std::string devmajor();
	void setDevmajor(std::string devmajor);
	std::string devminor();
	void setDevminor(std::string devminor);
	std::string prefix();
	void setPrefix(std::string prefix);

	int offset();
	int contentSize();

	static void writeOffset(Poco::BinaryWriter* writer, int offset);
	static void writeEndOfArchive(Poco::BinaryWriter* writer);
	static void skipEndOfArchive(Poco::BinaryReader* reader);
	static int endOfArchiveSize();
	static int headerSize();

private:
	void updateChksum();

	std::string m_name;
	unsigned int m_mode;
	std::string m_uid;
	std::string m_gid;
	unsigned int m_size;
	unsigned int m_mtime;
	unsigned int m_chksum;
	std::string m_typeflag;
	std::string m_linkname;
	std::string m_magic;
	std::string m_version;
	std::string m_uname;
	std::string m_gname;
	std::string m_devmajor;
	std::string m_devminor;
	std::string m_prefix;
};

#endif // GEODATAPOINTMAPRITER3DLOADER_TARHEADERBLOCK_H
