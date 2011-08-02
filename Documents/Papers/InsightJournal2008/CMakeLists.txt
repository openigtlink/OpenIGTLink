PROJECT(OpenIGTLinkInsightJournalPaper2008)

FIND_PACKAGE(LATEX)

IF(NOT PDFLATEX_COMPILER)
  MESSAGE("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
ENDIF(NOT PDFLATEX_COMPILER)

FIND_PACKAGE(Gnuplot)

#
# Set some directories that we use below.
#
SET(PLOT_DIR ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR})


#
# These are source files needed for the report generation.
#
SET(REPORT_SRCS
  algorithmic.sty
  algorithm.sty
  amssymb.sty
  fancyhdr.sty
  floatflt.sty
  fncychap.sty
  InsightArticle.cls
  InsightJournal.bib
  InsightJournal.ist
  InsightJournal.sty
  OpenIGTLinkIJ2008.tex
  times.sty
  picins.sty
  )

#
# This adds a custom command for each source file in REPORT_SRCS
# that copies the file from the source directory to the binary
# directory where the pdf will be generated.
#
FOREACH(SOURCE_FILE ${REPORT_SRCS})
   ADD_CUSTOM_COMMAND(
     OUTPUT   ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}/${SOURCE_FILE}
     DEPENDS  ${OpenIGTLinkInsightJournalPaper2008_SOURCE_DIR}/${SOURCE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OpenIGTLinkInsightJournalPaper2008_SOURCE_DIR}/${SOURCE_FILE} ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}/${SOURCE_FILE}
    )
   SET(COPY_RESULTS ${COPY_RESULTS} ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}/${SOURCE_FILE})
ENDFOREACH(SOURCE_FILE ${REPORT_SRCS})

#
# Add here the list of graphics that are generated at run time and must be
# inserted in the document.
#
SET(GRAPHICS_TO_BE_INSERTED "")

#
# This adds a custom target that generates OpenIGTLinkIJ2008.pdf
# This target depends on the list of copied files created
# with the custom command above and the Plots target.
#
ADD_CUSTOM_TARGET(OpenIGTLinkIJ2008 ALL 
   COMMAND ${PDFLATEX_COMPILER} 
        ${OpenIGTLinkInsightJournalPaper2008_SOURCE_DIR}/OpenIGTLinkIJ2008.tex 
        -output-directory ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}
   COMMAND ${PDFLATEX_COMPILER} 
        ${OpenIGTLinkInsightJournalPaper2008_SOURCE_DIR}/OpenIGTLinkIJ2008.tex 
        -output-directory ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}
   DEPENDS ${COPY_RESULTS} ${GRAPHICS_TO_BE_INSERTED}
   WORKING_DIRECTORY ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}
   )
 
#  
# This adds a custom target that will display the report.
#   
IF(WIN32)
  SET(PDF_READER "c:\\Program Files\\Adobe\\Reader 8.0\\Reader\\AcroRd32.exe")
ELSE(WIN32)
  SET(PDF_READER "/usr/bin/acroread")
ENDIF(WIN32)

ADD_CUSTOM_TARGET(ViewReport ALL
  COMMAND ${CMAKE_COMMAND} -E echo ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}/OpenIGTLinkIJ2008.pdf
#  COMMAND ${PDF_READER} 
#  ARGS ${OpenIGTLinkInsightJournalPaper2008_BINARY_DIR}/OpenIGTLinkIJ2008.pdf
)

