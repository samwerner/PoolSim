SRCS = $(wildcard *.cpp)
OBJ = $(SRCS:.cpp=.o)

EXE = poolsim

CXX = g++ -std=c++11
CXXFLAGS = -Wall -g -MMD
LDFLAGS = -lboost_program_options


$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

json.hpp:
	wget https://github.com/nlohmann/json/releases/download/v3.5.0/json.hpp -O $@

%.o: %.cpp json.hpp
	$(CXX) $(CXXFLGS) -MMD -MP -c $< -o $@

-include $(OBJ:.o=.d)

clean:
	rm -f $(EXE) $(OBJ) $(OBJ:.o=.d)

.PHONY: clean
