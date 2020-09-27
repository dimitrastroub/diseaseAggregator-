diseaseAggregator: worker

diseaseAggregator: diseaseAggregator.c
	gcc -o diseaseAggregator diseaseAggregator.c

worker: worker.c
	gcc -o worker worker.c
clean:
	rm -f worker diseaseAggregator
