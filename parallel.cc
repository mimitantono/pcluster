/*
 * parallel.cpp
 *
 *  Created on: Nov 2, 2014
 *      Author: mimitantono
 */

#include "parallel.h"

std::vector<cluster_result> Parallel::results;

Parallel::Parallel() {
//	for (int i = 0; i < Property::threads; i++) {
//		Db_data * db_data = new Db_data();
//		db.push_back(db_data);
//	}
//	Db_data::read_file (&db);
}

Parallel::~Parallel() {
	for (int i = 0; i < results.size(); i++) {
		delete &results[i];
	}
}

typedef struct thread_data {
	unsigned long thread_id;
	Db_data * db;
} thread_data;

void *run_cluster(void *threadargs) {
	thread_data *my_data = (thread_data*) threadargs;
	cluster_job cluster_job(my_data->db);
	cluster_result * result = cluster_job.algo_run(my_data->thread_id);
	Parallel::results.push_back(*result);
	delete result;
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
	std::vector<Db_data*> db;
	Db_data::read_file(db);

	for (long i = 0; i < Property::threads; i++) {
		thread_data_array[i].thread_id = (unsigned long) i;
		db[i]->print_debug();
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
	for (int i = 0; i < db.size(); i++) {
		delete db[i];
	}
	thread_data_array = NULL;
}

