TARGETS = radio-proxy radio-client

CXX = g++
CCFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS = -lpthread -lboost_program_options

all: $(TARGETS)

radio-proxy: proxy.cc
	$(CXX) $(CCFLAGS) $< $(LDFLAGS) -o $@

radio-client: client.cc
	$(CXX) $(CCFLAGS) $< $(LDFLAGS) -o $@

.PHONY: clean

clean:
	rm -f $(TARGETS) *.o *.d *~ *.bak

-include $(DEPS)
