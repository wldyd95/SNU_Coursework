#--------------------------------------------------------------
# 
#  4190.308 Computer Architecture (Spring 2019)
#
#  Project #3: Drawing diagonal lines in an image
#
#  April 24, 2019.
#
#  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#--------------------------------------------------------------

CC	= gcc
AS	= as
CFLAGS	= -g -Og -Wall
ASFLAGS	= -g
LDFLAGS	= 
RM	= rm
CMP = cmp

CSRCS	= bmp.c 
ASRCS	= bmpdiag.s
TARGET	= bmpdiag
OBJECTS	= $(CSRCS:.c=.o) $(ASRCS:.s=.o)
INBMP	= bordeaux.bmp
OUTBMP	= out.bmp
ANSBMP	= bordeaux-ans.bmp
GAP		= 100

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.s.o:
	$(AS) $(ASFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJECTS) $(TARGET) $(OUTBMP) *~

run:
	@$(RM) -f $(OUTBMP)
	./$(TARGET)	$(INBMP) $(OUTBMP) $(GAP)

test:
	$(CMP) $(OUTBMP) $(ANSBMP)
