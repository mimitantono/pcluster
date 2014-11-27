/*
 * parallel.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: mimitantono
 */

#include "parallel.h"

std::vector<cluster_result*> Parallel::results;

Parallel::Parallel() {
	db = new Db_data*[Property::threads];
	for (int i = 0; i < Property::threads; i++) {
		db[i] = new Db_data;
	}
	Db_data::read_file(db);
}

Parallel::~Parallel() {
	delete[] db;
	db = NULL;
}

typedef struct thread_data {
	unsigned long thread_id;
	Db_data * db;
} thread_data;

void *run_cluster(void *threadargs) {
	thread_data *my_data = (thread_data*) threadargs;
	cluster_job cluster_job(my_data->db);
	Parallel::results.push_back(cluster_job.algo_run(my_data->thread_id));
	pthread_exit(NULL);
}

void Parallel::run() {
	pthread_t threads[Property::threads];
	pthread_attr_t attr;
	int rc;
	long t;
	void *status;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	thread_data *thread_data_array = new thread_data[Property::threads];

	for (long i = 0; i < Property::threads; i++) {
		thread_data_array[i].thread_id = (unsigned long) i;
		thread_data_array[i].db = db[i];
		rc = pthread_create(&threads[i], &attr, run_cluster, (void *) &thread_data_array[i]);
		if (rc) {
			fprintf(stderr, "Error: unable to create thread, %d", rc);
			exit(-1);
		}
	}
	pthread_attr_destroy(&attr);
	for (t = 0; t < Property::threads; t++) {
		rc = pthread_join(threads[t], &status);
		if (rc) {
			fprintf(stderr, "ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
		printf("\nMain: completed join with thread %ld having a status of %ld\n", t + 1, (long) status);
	}
	delete[] thread_data_array;
	thread_data_array = NULL;
}

