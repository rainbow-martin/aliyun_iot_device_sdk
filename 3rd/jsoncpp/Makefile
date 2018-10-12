#
# libjson.a

OUTPUTDIR       = ../../libs
OBJDIR          = ./build
TARGET          = $(OUTPUTDIR)/libjson.a
RM              = rm -f
AR              = ar
INCPATH         = -I./include/
LIBS            = -lstdc++
FLAGS			= -O0 -g3 -Wno-invalid-offsetof -DPOSIX 

CPPFILES        = $(notdir $(wildcard src/*.cpp))

.PHONY:all
all:setup $(TARGET)
	@echo "$(TARGET) is made"

$(OBJDIR)/%.o:src/%.cpp
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

