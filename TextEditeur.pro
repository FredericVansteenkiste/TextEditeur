# Addition of the library to open the library qtxlsx
include(xlsx/qtxlsx.pri)

QT += widgets core gui

HEADERS   = mainwindow.h \
   xlsx/xlsxabstractooxmlfile.h \
   xlsx/xlsxabstractooxmlfile_p.h \
   xlsx/xlsxabstractsheet.h \
   xlsx/xlsxabstractsheet_p.h \
   xlsx/xlsxcell.h \
   xlsx/xlsxcell_p.h \
   xlsx/xlsxcellformula.h \
   xlsx/xlsxcellformula_p.h \
   xlsx/xlsxcellrange.h \
   xlsx/xlsxcellreference.h \
   xlsx/xlsxchart.h \
   xlsx/xlsxchart_p.h \
   xlsx/xlsxchartsheet.h \
   xlsx/xlsxchartsheet_p.h \
   xlsx/xlsxcolor_p.h \
   xlsx/xlsxconditionalformatting.h \
   xlsx/xlsxconditionalformatting_p.h \
   xlsx/xlsxcontenttypes_p.h \
   xlsx/xlsxdatavalidation.h \
   xlsx/xlsxdatavalidation_p.h \
   xlsx/xlsxdocpropsapp_p.h \
   xlsx/xlsxdocpropscore_p.h \
   xlsx/xlsxdocument.h \
   xlsx/xlsxdocument_p.h \
   xlsx/xlsxdrawing_p.h \
   xlsx/xlsxdrawinganchor_p.h \
   xlsx/xlsxformat.h \
   xlsx/xlsxformat_p.h \
   xlsx/xlsxglobal.h \
   xlsx/xlsxmediafile_p.h \
   xlsx/xlsxnumformatparser_p.h \
   xlsx/xlsxrelationships_p.h \
   xlsx/xlsxrichstring.h \
   xlsx/xlsxrichstring_p.h \
   xlsx/xlsxsharedstrings_p.h \
   xlsx/xlsxsimpleooxmlfile_p.h \
   xlsx/xlsxstyles_p.h \
   xlsx/xlsxtheme_p.h \
   xlsx/xlsxutility_p.h \
   xlsx/xlsxworkbook.h \
   xlsx/xlsxworkbook_p.h \
   xlsx/xlsxworksheet.h \
   xlsx/xlsxworksheet_p.h \
   xlsx/xlsxzipreader_p.h \
   xlsx/xlsxzipwriter_p.h

SOURCES   = main.cpp \
            mainwindow.cpp \
            xlsx/xlsxabstractooxmlfile.cpp \
            xlsx/xlsxabstractsheet.cpp \
            xlsx/xlsxcell.cpp \
            xlsx/xlsxcellformula.cpp \
            xlsx/xlsxcellrange.cpp \
            xlsx/xlsxcellreference.cpp \
            xlsx/xlsxchart.cpp \
            xlsx/xlsxchartsheet.cpp \
            xlsx/xlsxcolor.cpp \
            xlsx/xlsxconditionalformatting.cpp \
            xlsx/xlsxcontenttypes.cpp \
            xlsx/xlsxdatavalidation.cpp \
            xlsx/xlsxdocpropsapp.cpp \
            xlsx/xlsxdocpropscore.cpp \
            xlsx/xlsxdocument.cpp \
            xlsx/xlsxdrawing.cpp \
            xlsx/xlsxdrawinganchor.cpp \
            xlsx/xlsxformat.cpp \
            xlsx/xlsxmediafile.cpp \
            xlsx/xlsxnumformatparser.cpp \
            xlsx/xlsxrelationships.cpp \
            xlsx/xlsxrichstring.cpp \
            xlsx/xlsxsharedstrings.cpp \
            xlsx/xlsxsimpleooxmlfile.cpp \
            xlsx/xlsxstyles.cpp \
            xlsx/xlsxtheme.cpp \
            xlsx/xlsxutility.cpp \
            xlsx/xlsxworkbook.cpp \
            xlsx/xlsxworksheet.cpp \
            xlsx/xlsxzipreader.cpp \
            xlsx/xlsxzipwriter.cpp

RESOURCES = TextEditeur.qrc
