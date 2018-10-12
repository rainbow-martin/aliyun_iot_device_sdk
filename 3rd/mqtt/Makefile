#
# libmqtt.a

OUTPUTDIR       = ../../libs
SYNC_OBJDIR     = ./sync_build
ASYNC_OBJDIR    = ./async_build
SYNC_TARGET     = $(OUTPUTDIR)/libmqttc.a
ASYNC_TARGET    = $(OUTPUTDIR)/libmqtta.a
RM              = rm -f
AR              = ar
INCPATH         = -I.
LIBS            = -lstdc++
FLAGS			= -O0 -g3 -fPIC -Wall 

srcdir=.

SOURCE_FILES = $(wildcard $(srcdir)/*.c)
SOURCE_FILES_C = $(filter-out $(srcdir)/MQTTAsync.c $(srcdir)/MQTTVersion.c $(srcdir)/SSLSocket.c, $(SOURCE_FILES))
SOURCE_FILES_CS = $(filter-out $(srcdir)/MQTTAsync.c $(srcdir)/MQTTVersion.c, $(SOURCE_FILES))
SOURCE_FILES_A = $(filter-out $(srcdir)/MQTTClient.c $(srcdir)/MQTTVersion.c $(srcdir)/SSLSocket.c, $(SOURCE_FILES))
SOURCE_FILES_AS = $(filter-out $(srcdir)/MQTTClient.c $(srcdir)/MQTTVersion.c, $(SOURCE_FILES))


.PHONY:all
all:setup $(SYNC_TARGET) $(ASYNC_TARGET)
	@echo "$(SYNC_TARGET) is made"

$(SYNC_OBJDIR)/%.o:%.c
	$(SLIENCE) $(CC) $(INCPATH) $(FLAGS) -c $< -o $@

$(SYNC_TARGET): $(SOURCE_FILES_C:%.c=$(SYNC_OBJDIR)/%.o)
	@echo Building static library
	$(AR) cr $@ $^

$(ASYNC_OBJDIR)/%.o:%.c
	$(SLIENCE) $(CC) $(INCPATH) $(FLAGS) -c $< -o $@

$(ASYNC_TARGET): $(SOURCE_FILES_A:%.c=$(ASYNC_OBJDIR)/%.o)
	@echo Building static library
	$(AR) cr $@ $^


setup:
	mkdir -p $(OUTPUTDIR)
	mkdir -p $(SYNC_OBJDIR)
	mkdir -p $(ASYNC_OBJDIR)

.PHONY:clean
clean:
	- $(RM) $(SYNC_OBJDIR)/*.o
	- $(RM) $(ASYNC_OBJDIR)/*.o
	- $(RM) $(SYNC_TARGET)
	- $(RM) $(ASYNC_TARGET)

