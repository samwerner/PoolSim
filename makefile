OBJ =  event.o event_queue.o compare.o miner.o miningpool.o share_events.o reward_scheme.o queue_based.o 

EXE = poolsim

CXX = g++ -std=c++11
CXXFLAGS = -Wall -g -MMD

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLGS) -c $<

-include $(OBJ:.o=.d)

clean:
	rm -f $(EXE) $(OBJ) $(OBJ:.o=.d)

.PHONY: clean
