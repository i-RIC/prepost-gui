
#---------------------------------------------------------------------------
# Basic settings
#---------------------------------------------------------------------------

DOXYFILE_ENCODING      = UTF-8
OUTPUT_DIRECTORY       =
CREATE_SUBDIRS         = YES
ALIASES                =

ALLOW_UNICODE_NAMES    = NO
OUTPUT_LANGUAGE        = English

#---------------------------------------------------------------------------
# Document output control
#---------------------------------------------------------------------------

BRIEF_MEMBER_DESC      = NO
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       =
ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
AUTOLINK_SUPPORT       = YES

TAB_SIZE               = 2

#---------------------------------------------------------------------------
# File path
#---------------------------------------------------------------------------

FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =

SHORT_NAMES            = NO

#---------------------------------------------------------------------------
# Comment style
#---------------------------------------------------------------------------

JAVADOC_AUTOBRIEF      = NO
QT_AUTOBRIEF           = NO
MULTILINE_CPP_IS_BRIEF = NO

#---------------------------------------------------------------------------
# Language support settings
#---------------------------------------------------------------------------

BUILTIN_STL_SUPPORT    = YES

TCL_SUBST              =
CPP_CLI_SUPPORT        = NO
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
OPTIMIZE_FOR_FORTRAN   = NO
OPTIMIZE_OUTPUT_VHDL   = NO
EXTENSION_MAPPING      =
MARKDOWN_SUPPORT       = YES
SIP_SUPPORT            = NO
IDL_PROPERTY_SUPPORT   = YES
DISTRIBUTE_GROUP_DOC   = NO
GROUP_NESTED_COMPOUNDS = NO

SUBGROUPING            = YES
INLINE_GROUPED_CLASSES = NO
INLINE_SIMPLE_STRUCTS  = NO
TYPEDEF_HIDES_STRUCT   = NO

LOOKUP_CACHE_SIZE      = 0

#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------

EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_PACKAGE        = YES

EXTRACT_STATIC         = NO
EXTRACT_LOCAL_CLASSES  = NO
EXTRACT_LOCAL_METHODS  = NO
EXTRACT_ANON_NSPACES   = NO

HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = YES

INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = NO

HIDE_SCOPE_NAMES       = NO

# If the HIDE_COMPOUND_REFERENCE tag is set to NO (default) then doxygen will
# append additional text to a page's title, such as Class Reference. If set to
# YES the compound reference will be hidden.
# The default value is: NO.

HIDE_COMPOUND_REFERENCE= NO

SHOW_INCLUDE_FILES     = YES
SHOW_GROUPED_MEMB_INC  = NO
FORCE_LOCAL_INCLUDES   = NO
INLINE_INFO            = YES

# If the SORT_MEMBER_DOCS tag is set to YES then doxygen will sort the
# (detailed) documentation of file and class members alphabetically by member
# name. If set to NO, the members will appear in declaration order.
# The default value is: YES.

SORT_MEMBER_DOCS       = YES

SORT_BRIEF_DOCS        = NO
SORT_MEMBERS_CTORS_1ST = NO
SORT_GROUP_NAMES       = NO
SORT_BY_SCOPE_NAME     = NO
STRICT_PROTO_MATCHING  = NO

GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES

# The ENABLED_SECTIONS tag can be used to enable conditional documentation
# sections, marked by \if <section_label> ... \endif and \cond <section_label>
# ... \endcond blocks.

ENABLED_SECTIONS       =

# The MAX_INITIALIZER_LINES tag determines the maximum number of lines that the
# initial value of a variable or macro / define can have for it to appear in the
# documentation. If the initializer consists of more lines than specified here
# it will be hidden. Use a value of 0 to hide initializers completely. The
# appearance of the value of individual variables and macros / defines can be
# controlled using \showinitializer or \hideinitializer command in the
# documentation regardless of this setting.
# Minimum value: 0, maximum value: 10000, default value: 30.

MAX_INITIALIZER_LINES  = 30

# Set the SHOW_USED_FILES tag to NO to disable the list of files generated at
# the bottom of the documentation of classes and structs. If set to YES, the
# list will mention the files that were used to generate the documentation.
# The default value is: YES.

SHOW_USED_FILES        = YES

# Set the SHOW_FILES tag to NO to disable the generation of the Files page. This
# will remove the Files entry from the Quick Index and from the Folder Tree View
# (if specified).
# The default value is: YES.

SHOW_FILES             = YES

SHOW_NAMESPACES        = YES

# The FILE_VERSION_FILTER tag can be used to specify a program or script that
# doxygen should invoke to get the current version for each file (typically from
# the version control system). Doxygen will invoke the program by executing (via
# popen()) the command command input-file, where command is the value of the
# FILE_VERSION_FILTER tag, and input-file is the name of an input file provided
# by doxygen. Whatever the program writes to standard output is used as the file
# version. For an example see the documentation.

FILE_VERSION_FILTER    =

# The LAYOUT_FILE tag can be used to specify a layout file which will be parsed
# by doxygen. The layout file controls the global structure of the generated
# output files in an output format independent way. To create the layout file
# that represents doxygen's defaults, run doxygen with the -l option. You can
# optionally specify a file name after the option, if omitted DoxygenLayout.xml
# will be used as the name of the layout file.
#
# Note that if you run doxygen from a directory containing a file called
# DoxygenLayout.xml, doxygen will parse it automatically even if the LAYOUT_FILE
# tag is left empty.

LAYOUT_FILE            =

# The CITE_BIB_FILES tag can be used to specify one or more bib files containing
# the reference definitions. This must be a list of .bib files. The .bib
# extension is automatically appended if omitted. This requires the bibtex tool
# to be installed. See also http://en.wikipedia.org/wiki/BibTeX for more info.
# For LaTeX the style of the bibliography can be controlled using
# LATEX_BIB_STYLE. To use this feature you need bibtex and perl available in the
# search path. See also \cite for info how to create references.

CITE_BIB_FILES         =

#---------------------------------------------------------------------------
# Configuration options related to warning and progress messages
#---------------------------------------------------------------------------

QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = NO
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = NO
WARN_FORMAT            = "$file:$line: $text"
WARN_LOGFILE           = doxygen_warnings.txt

#---------------------------------------------------------------------------
# Configuration options related to the input files
#---------------------------------------------------------------------------

INPUT_ENCODING         = UTF-8
FILE_PATTERNS          =
EXCLUDE_PATTERNS       = ui_*.h debug/*.h debug/*.cpp release/*.h debug/*.cpp *_impl.h moc_*.cpp
RECURSIVE              = YES
EXCLUDE_SYMLINKS       = NO
EXCLUDE_SYMBOLS        =

EXAMPLE_PATH           = examples
EXAMPLE_PATTERNS       =
EXAMPLE_RECURSIVE      = NO

IMAGE_PATH             = images
INPUT_FILTER           =
FILTER_PATTERNS        =
FILTER_SOURCE_FILES    = NO
FILTER_SOURCE_PATTERNS =
USE_MDFILE_AS_MAINPAGE =

#---------------------------------------------------------------------------
# Configuration options related to source browsing
#---------------------------------------------------------------------------

SOURCE_BROWSER         = YES

INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES

REFERENCED_BY_RELATION = NO
REFERENCES_RELATION    = NO
REFERENCES_LINK_SOURCE = NO
SOURCE_TOOLTIPS        = YES

USE_HTAGS              = NO

VERBATIM_HEADERS       = YES

CLANG_ASSISTED_PARSING = NO
CLANG_OPTIONS          =

#---------------------------------------------------------------------------
# Configuration options related to the alphabetical class index
#---------------------------------------------------------------------------

ALPHABETICAL_INDEX     = YES

COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          =

#---------------------------------------------------------------------------
# Configuration options related to the HTML output
#---------------------------------------------------------------------------

GENERATE_HTML          = YES

HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html

HTML_HEADER            =
HTML_FOOTER            =
HTML_STYLESHEET        =
HTML_EXTRA_STYLESHEET  =
HTML_EXTRA_FILES       =
HTML_COLORSTYLE_HUE    = 220
HTML_COLORSTYLE_SAT    = 100
HTML_COLORSTYLE_GAMMA  = 80
HTML_TIMESTAMP         = NO

HTML_DYNAMIC_SECTIONS  = YES

HTML_INDEX_NUM_ENTRIES = 1

GENERATE_DOCSET        = NO
GENERATE_HTMLHELP      = NO
GENERATE_CHI           = NO
BINARY_TOC             = NO
GENERATE_QHP           = NO
GENERATE_ECLIPSEHELP   = NO

DISABLE_INDEX          = NO

GENERATE_TREEVIEW      = NO
TREEVIEW_WIDTH         = 250

ENUM_VALUES_PER_LINE   = 4

EXT_LINKS_IN_WINDOW    = NO
USE_MATHJAX            = NO

SEARCHENGINE           = YES

SERVER_BASED_SEARCH    = NO
EXTERNAL_SEARCH        = NO
SEARCHENGINE_URL       =
SEARCHDATA_FILE        = searchdata.xml
EXTERNAL_SEARCH_ID     =
EXTRA_SEARCH_MAPPINGS  =

#---------------------------------------------------------------------------
# Configuration options related to the RTF output
#---------------------------------------------------------------------------

GENERATE_RTF           = YES
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO

RTF_STYLESHEET_FILE    =
RTF_EXTENSIONS_FILE    =
RTF_SOURCE_CODE        = NO

#---------------------------------------------------------------------------
# Disabling needless outputs
#---------------------------------------------------------------------------

GENERATE_LATEX         = NO
GENERATE_MAN           = NO
GENERATE_XML           = NO
GENERATE_DOCBOOK       = NO
GENERATE_PERLMOD       = NO

#---------------------------------------------------------------------------
# Configuration options related to the preprocessor
#---------------------------------------------------------------------------

ENABLE_PREPROCESSING   = NO

#---------------------------------------------------------------------------
# Configuration options related to external references
#---------------------------------------------------------------------------

ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = NO
EXTERNAL_PAGES         = NO

PERL_PATH              = /usr/bin/perl

#---------------------------------------------------------------------------
# Configuration options related to the dot tool
#---------------------------------------------------------------------------

CLASS_DIAGRAMS         = YES

MSCGEN_PATH            =
DIA_PATH               =

HIDE_UNDOC_RELATIONS   = NO

HAVE_DOT               = YES

DOT_NUM_THREADS        = 0
DOT_FONTNAME           = Helvetica
DOT_FONTSIZE           = 10
DOT_FONTPATH           =

CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = NO
GROUP_GRAPHS           = YES
UML_LOOK               = NO
UML_LIMIT_NUM_FIELDS   = 10
TEMPLATE_RELATIONS     = NO

INCLUDE_GRAPH          = NO
INCLUDED_BY_GRAPH      = NO

CALL_GRAPH             = NO
CALLER_GRAPH           = NO

GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = NO

DOT_IMAGE_FORMAT       = png
INTERACTIVE_SVG        = NO

DOT_PATH               = 

DOTFILE_DIRS           = dots
MSCFILE_DIRS           =

DIAFILE_DIRS           =
PLANTUML_JAR_PATH      =
PLANTUML_INCLUDE_PATH  =
DOT_GRAPH_MAX_NODES    = 50
MAX_DOT_GRAPH_DEPTH    = 0
DOT_TRANSPARENT        = NO
DOT_MULTI_TARGETS      = NO

GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
