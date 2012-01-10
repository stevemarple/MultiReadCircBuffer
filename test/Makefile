DIFF = diff
RM = rm -f
CXXFLAGS += -ggdb -Wall -I. -DMRCB_DEBUG
LDFLAGS += 
# -lduma

default: mrcb_test

.PHONY : MultiReadCircBuffer.h
MultiReadCircBuffer.h : ../MultiReadCircBuffer.h
	@echo '' > $@
	@echo '#include "arduino_compat.h"' >> $@
	@echo '#include <iostream>' >> $@
	@echo 'using namespace ::std;' >> $@
	cat $< >> $@

# .PHONY : MultiReadCircBuffer.cpp
MultiReadCircBuffer.cpp : ../MultiReadCircBuffer.cpp
	cp $< $@

MultiReadCircBuffer.o : MultiReadCircBuffer.cpp MultiReadCircBuffer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

mrcb_test.o : mrcb_test.cpp MultiReadCircBuffer.h

mrcb_test : mrcb_test.o MultiReadCircBuffer.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

.PHONY : run
run : mrcb_test
	./mrcb_test

# Regression testing. Run as "make regressiontest", should display
# "TEST PASSED" if everything ok.
.PHONY : regressiontest
regressiontest :
	$(MAKE) clean
	$(MAKE) mrcb_test
	./mrcb_test > mrcb_test.regressiontest.tmp
	$(DIFF) -s -u mrcb_test.regressiontest mrcb_test.regressiontest.tmp
	-$(RM) mrcb_test.regressiontest.tmp
	@echo
	@echo TEST PASSED

.PHONY : clean
clean : 
	-$(RM) *.o MultiReadCircBuffer.h MultiReadCircBuffer.cpp mrcb_test

