runtimes=100
optflag="-O2"
filenamebase="base"
filenameftss="ftss"

niter=16000

# compile
gcc $optflag -o $filenamebase $filenamebase.c -lpthread -lrt -lm
gcc $optflag -o $filenameftss $filenameftss.c -lpthread -lrt -lm

# run
for a in g f t; do
	for nthreads in 2 4 8 16 32; do
		for((i=0;i<$runtimes;i=i+1));do
			./$filenamebase $a $nthreads $niter
		done;
	done;
done;

sleep 5;

for a in g f t; do
	for nthreads in 2 4 8 16 32; do
		for((i=0;i<$runtimes;i=i+1));do
			./$filenameftss $a $nthreads $niter
		done; 
	done;
done;

./parse