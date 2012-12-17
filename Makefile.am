AM_CPPFLAGS = -std=c++0x  -pedantic -I $(srcdir)/include
AM_LDFLAGS = @LIBS@
#LIB_UNISEG = @LIB_UNISEG@

SRC_DIR = src
UTILS_DIR = utils

noinst_HEADERS = \
	$(SRC_DIR)/key_term.h \
	$(SRC_DIR)/stop_word.h \
	$(SRC_DIR)/config.h \
	$(SRC_DIR)/application_settings.h \
	$(SRC_DIR)/topic.h \
	$(SRC_DIR)/topic_set.h \
	$(SRC_DIR)/query.h \
	$(SRC_DIR)/corpus.h \
	$(SRC_DIR)/corpus_wikipedia.h \
	$(SRC_DIR)/corpus_target.h \
	$(SRC_DIR)/sys_file.h \
	$(SRC_DIR)/wikipedia.h \
	$(SRC_DIR)/language.h \
	$(SRC_DIR)/chinese_segmentation.h \
	$(SRC_DIR)/webpage_retriever.h \
	$(SRC_DIR)/algorithm.h \
	$(SRC_DIR)/algorithm_gw.h \
	$(SRC_DIR)/algorithm_gw2.h \
	$(SRC_DIR)/algorithm_question_phrase_chinese.h \
	$(SRC_DIR)/google_translator.h \
	$(SRC_DIR)/search_engine_ant.h
	
includes= \
	$(UTILS_DIR)/ant_link_parts.h \
	$(UTILS_DIR)/pattern_singleton.h \
	$(UTILS_DIR)/string_utils.h \
	$(UTILS_DIR)/utils.h
		
noinst_PROGRAMS = qtrec qqa qtrec_merge prune_question rmxmltags cp2utf8 utf8char char2utf8 split_wikidump wikidump2page wikibz2page
# 

qtrec_SOURCES = \
	$(SRC_DIR)/key_term.cpp \
	$(SRC_DIR)/stop_word.cpp \
	$(SRC_DIR)/config.cpp \
	$(SRC_DIR)/application_settings.cpp \
	$(SRC_DIR)/query.cpp \
	$(SRC_DIR)/topic.cpp \
	$(SRC_DIR)/topic_set.cpp \
	$(SRC_DIR)/search_engine_ant.cpp \
	$(SRC_DIR)/sys_file.cpp \
	$(SRC_DIR)/corpus.cpp \
	$(SRC_DIR)/corpus_wikipedia.cpp \
	$(SRC_DIR)/corpus_target.cpp \
	$(SRC_DIR)/language.cpp \
	$(SRC_DIR)/chinese_segmentation.cpp \
	$(SRC_DIR)/wikipedia.cpp \
	$(SRC_DIR)/webpage_retriever.cpp \
	$(SRC_DIR)/algorithm.cpp \
	$(SRC_DIR)/algorithm_gw.cpp \
	$(SRC_DIR)/algorithm_gw2.cpp \
	$(SRC_DIR)/algorithm_question_phrase_chinese.cpp \
	$(SRC_DIR)/google_translator.cpp \
	$(SRC_DIR)/qtrec.cpp
#qtrec_LDFLAGS = -static 
qtrec_LDADD = -lpthread -lcurl -latire_core -luniseg

qqa_SOURCES = \
	$(SRC_DIR)/sys_file.cpp \
	$(SRC_DIR)/qqa.cpp

rmxmltags_SOURCES = $(UTILS_DIR)/rmxmltags.cpp \
					$(SRC_DIR)/sys_file.cpp
					
cp2utf8_SOURCES = \
				$(UTILS_DIR)/cp2utf8.cpp

utf8char_SOURCES = \
				$(UTILS_DIR)/utf8char.cpp

char2utf8_SOURCES = \
				$(UTILS_DIR)/char2utf8.cpp					

split_wikidump_SOURCES = \
	$(SRC_DIR)/sys_file.cpp \
	$(UTILS_DIR)/split_wikidump.cpp
split_wikidump_LDADD = -latire_core
split_wikidump_LIBFLAGS = -static

wikidump2page_SOURCES = \
	$(SRC_DIR)/sys_file.cpp \
	$(UTILS_DIR)/wikidump2page.cpp
wikidump2page_LDADD = -latire_core
wikidump2page_LIBFLAGS = -static

wikibz2page_SOURCES = \
	$(SRC_DIR)/sys_file.cpp \
	$(UTILS_DIR)/wikibz2page.cpp
wikibz2page_LDADD = -latire_core
wikibz2page_LIBFLAGS = -static

qtrec_merge_SOURCES = \
	$(SRC_DIR)/sys_file.cpp \
	$(SRC_DIR)/chinese_segmentation.cpp \
	$(SRC_DIR)/qtrec_merge.cpp 
qtrec_merge_LDADD =  -luniseg

prune_question_SOURCES = \
	$(SRC_DIR)/sys_file.cpp \
	$(SRC_DIR)/prune_question.cpp

SCRIPT_DIR = scripts

EXTRA_DIST = \
	application.conf \
	$(SCRIPT_DIR)/find_chinese.sh \
	$(SCRIPT_DIR)/find_miss_topic.sh \
	$(SCRIPT_DIR)/gen.sh \
	$(SCRIPT_DIR)/IR4QA-runchecker2 \
	*.sh