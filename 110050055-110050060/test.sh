#!/bin/bash
#cd level-3b
make -f Makefile.cfglp 
cd test_files
#rm *.cs306*
for i in $(ls *.c)
do
	cd ..
	make -f Makefile.cfg FILE=$i > /dev/null
	#./cfglp64 -icode test_files/${i}s306.cfg
	#cp test_files/${i}s306.cfg.ic test_files/${i}s306.cfg.ic.my
	./cfglpsir -icode test_files/${i}s306.cfg
	#diff -b -B test_files/${i}s306.cfg.ic test_files/${i}s306.cfg.ic.my
	#./cfglp64 -compile test_files/${i}s306.cfg
	#cp test_files/${i}s306.cfg.spim test_files/${i}s306.cfg.spim.my
	#./cfglpsir -compile test_files/${i}s306.cfg
	#diff -b -B test_files/${i}s306.cfg.spim test_files/${i}s306.cfg.spim.my
	#./cfglp64 -lra -compile test_files/${i}s306.cfg
        #cp test_files/${i}s306.cfg.spim test_files/${i}s306.cfg.spim.my
        #./cfglpsir -lra -compile test_files/${i}s306.cfg
        #diff -b -B test_files/${i}s306.cfg.spim test_files/${i}s306.cfg.spim.my
	echo $i
	cd test_files
done
#rm *.my *.spim *.ic
cd ..
#rm *.tok *.eval *.ast
#make -f Makefile.cfglp clean
#make -f Makefile.cfg clean
exit
