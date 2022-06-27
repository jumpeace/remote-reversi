#*******************************************************************************************/
#*       File name: Makefile
#*    Project name: socket/mylib
#*      Month/Year: Jan/2003
#*          Author: Shuichi YUKITA
#*******************************************************************************************/
# 
# Since mylib.a consists of rather a small set of objects,
# it will be easy just to clean up and build from scratch.
OBJS = setup_client.o setup_server.o chop_newline.o setup_mserver.o
AR = ar -qc
mylib.a: 		$(OBJS)
		$(RM) $@
		$(AR) $@ $(OBJS)
$(OBJS):		mylib.h
clean:
		$(RM) *.o mylib.a