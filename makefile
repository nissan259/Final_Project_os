# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Executable names
EXEC_LEADER = leader_follower_server
EXEC_PIPELINE = pipeline_server

# Source files for Leader-Follower pattern
SRCS_LEADER = Graph.cpp Kruskal.cpp Prim.cpp Leader-Follower.cpp mst_factory.cpp
OBJS_LEADER = $(SRCS_LEADER:.cpp=.o)

# Source files for Pipeline pattern
SRCS_PIPELINE = Graph.cpp Kruskal.cpp Prim.cpp Pipeline_Pattern_server.cpp mst_factory.cpp
OBJS_PIPELINE = $(SRCS_PIPELINE:.cpp=.o)

# Default target to build both executables
all: $(EXEC_LEADER) $(EXEC_PIPELINE)

# Rule to build Leader-Follower server
$(EXEC_LEADER): $(OBJS_LEADER)
	$(CXX) $(CXXFLAGS) -o $(EXEC_LEADER) $(OBJS_LEADER)

# Rule to build Pipeline server
$(EXEC_PIPELINE): $(OBJS_PIPELINE)
	$(CXX) $(CXXFLAGS) -o $(EXEC_PIPELINE) $(OBJS_PIPELINE)

# Rule to compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove object files and executables
clean:
	rm -f $(OBJS_LEADER) $(OBJS_PIPELINE) $(EXEC_LEADER) $(EXEC_PIPELINE)

# Phony targets (not files)
.PHONY: all clean
