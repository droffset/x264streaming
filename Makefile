TARGET     = h264_streamer
VER        = 1.0.0

SRCDIR     = src

OBJDIR     = .obj

INCLUDES   = -I$(SRCDIR) -I/usr/include/jthread
LIBS       = -lv4l2 -lx264 -ljrtp -lconfig

CXXFLAGS   += -O2 -D_REENTRANT -DPIC -fpic -fPIC -Wall -W

TARGET_VER = $(TARGET).$(VER)

SOURCES    := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/utils/*.cpp $(SRCDIR)/video/*.cpp $(SRCDIR)/network/*.cpp $(SRCDIR)/except/*.cpp)

OBJECTS    := $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SOURCES)))

CXX = g++

make: $(OBJDIR)/$(TARGET_VER)

$(OBJDIR)/$(TARGET_VER): $(OBJDIR) $(OBJECTS)
	$(CXX) $(LFLAGS) $(LIBS) $(OBJECTS) -o $(OBJDIR)/$(TARGET_VER)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/utils/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/except/%.cpp
	$(CXX) -c $(CCFLAGS) $(INCLUDES) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/network/%.cpp
	$(CXX) -c $(CCFLAGS) $(INCLUDES) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/video/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJDIR)

$(TARGET_DIRS):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

.PHONY : clean

