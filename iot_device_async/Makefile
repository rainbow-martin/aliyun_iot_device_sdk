###################################################################
# iot_device

OUTPUTDIR       = ../bin
LIBDIR			= ../libs
OBJDIR          = $(OUTPUTDIR)/iot_device_async_obj
TARGET          = $(OUTPUTDIR)/iot_device_async

RM              = rm -f
INCPATH         = -I../../bcm2835-1.52/src/ 	\
				  -I../ 						\
				  -I../3rd/jsoncpp/include 		\
				  -I../3rd/mqtt
FLAGS           = -DPOSIX -O0 -g3 -Wno-invalid-offsetof
LIBPATH         = -L$(LIBDIR)
STDLIBS         = -ldl -lgcc -lstdc++ -lpthread
CPPFILES        = $(wildcard ./*.cpp)
LIBFILES        = -lrt -lssl -lcrypto -lbaselib -ljson -lmqtta
LDFLAGS         = $(LIBFILES) $(STDLIBS)

.PHONY:all
all:setup $(TARGET)
	@echo "$(TARGET) is made"

$(OBJDIR)/%.o:%.cpp
	$(SLIENCE) $(CXX) $(INCPATH) $(FLAGS) -c $< -o $@

$(TARGET): $(CPPFILES:%.cpp=$(OBJDIR)/%.o)
	@echo Building executable binary
	$(CC) $(LIBPATH) -o $@ $^ $(LDFLAGS)

setup:
	mkdir -p $(OUTPUTDIR)
	mkdir -p $(OBJDIR)

.PHONY:clean
clean:
	- $(RM) $(OBJDIR)/*.o
	- $(RM) $(TARGET)

