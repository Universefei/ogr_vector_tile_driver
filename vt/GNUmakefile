

include ../../../GDALmake.opt

OBJ	=	ogrvtdriver.o ogrvtdatasource.o ogrvtlayer.o

CPPFLAGS	:=	-I.. -I../.. $(GDAL_INCLUDE) $(CPPFLAGS)

default:	$(O_OBJ:.o=.$(OBJ_EXT))

$(O_OBJ):	ogr_vt.h

clean:
	rm -f *.o $(O_OBJ)
