module purge
module load gcc-8.2
g++ -Wall -fopenmp -o driver driver.cpp

# LOCK FREE
for t in 1 2 4 8 16 32 64
do
	./driver f $t 1000
	echo ""
done
# for t in 1 2 4 8 16 32 64
# do
# 	./driver f $t 10000
# 	echo ""
# done

# LOCK BASED
for t in 1 2 4 8 16 32 64
do
	./driver b $t 1000
	echo ""
done
# for t in 1 2 4 8 16 32 64
# do
# 	./driver b $t 10000
# 	echo ""
# done