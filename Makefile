####################################################################################################
#                                     Make file LargeInt 	                                   #
####################################################################################################

SUBDIRS:= src    

all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done
