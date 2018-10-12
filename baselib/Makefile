#
# libbaselib.a

OUTPUTDIR       = ../libs
OBJDIR          = $(OUTPUTDIR)/baselib
TARGET          = $(OUTPUTDIR)/libbaselib.a
RM              = rm -f
AR              = ar
INCPATH         = -I../include/common
LIBS            = -lstdc++
CPPFILES        = $(wildcard ./*.cpp)
FLAGS			= -O0 -g3 -Wno-invalid-offsetof -D POSIX

.PHONY:all
all:setup $(TARGET)
	@echo "$(TARGET) is made"

$(OBJDIR)/%.o:%.cpp
	$(SLIENCE) $(CXX) $(INCPATH) $(FLAGS) -c $< -o $@

$(TARGET): $(CPPFILES:%.cpp=$(OBJDIR)/%.o)
	@echo "Building static library: $(TARGET)"
	$(AR) cr $@ $^

setup:
	mkdir -p $(OUTPUTDIR)
	mkdir -p $(OBJDIR)

.PHONY:clean
clean:
	- $(RM) $(OBJDIR)/*.o
	- $(RM) $(TARGET)

