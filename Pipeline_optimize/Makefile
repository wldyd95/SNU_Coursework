#------------------------------------------------------------------
# 
#  4190.308 Computer Architecture (Spring 2019)
#
#  Project #4: Optimizing Program for a Pipelined Y86-64 Processor
#
#  May 28, 2019
#
#  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#------------------------------------------------------------------

CAT 	= /bin/cat

YAS 	= ./yas				# set to ./yas-mac on macOS
SSIM 	= ./ssim			# set to ./ssim-mac on macOS
SSIMFLAGS = -s

TARGET	= bmptest.yo
ASRCS	= bmptest.ys
YSRCS	= bmpmain.ys bmpdiag.ys
MEMOUT  = memory.out
MEMANS  = result.out

all: $(TARGET)

$(TARGET): $(ASRCS)
	$(YAS) $(ASRCS) $@

$(ASRCS): $(YSRCS)
	$(CAT) $(YSRCS) > $(ASRCS)

clean:
	$(RM) -f $(TARGET) $(ASRCS) $(MEMOUT) *~

run: $(TARGET)
	$(SSIM) $(TARGET)

testrun: $(TARGET)
	$(SSIM) $(SSIMFLAGS) $(TARGET)

test: 
	@rm -f $(MEMOUT)
	@make testrun
	@diff $(MEMOUT) $(MEMANS) && echo "Test: SUCCESS"
