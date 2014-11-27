/*
 * clusterresult.h
 *
 *  Created on: Nov 7, 2014
 *      Author: mimitantono
 */

#ifndef CLUSTERRESULT_H_
#define CLUSTERRESULT_H_

#include<vector>
#include<string>
#include "db.h"

struct member_info {
	seqinfo_t * sequence;
	unsigned qgram_diff;
	unsigned generation;
	unsigned radius;
};

struct cluster_info {
	unsigned cluster_id;
	std::vector<member_info*> cluster_members;
};

class cluster_result {
public:
	cluster_result();
	virtual ~cluster_result();
	std::vector<cluster_info*> clusters;
	cluster_info * new_cluster(long cluster_id);
	long partition_id;
	void print();
};

#endif /* CLUSTERRESULT_H_ */