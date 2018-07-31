#include <string>

#define SAVA_MASTER_PORT 20114
#define SAVA_WORKER_PORT 20115
#define SAVA_MSG_PORT 20116

/*
 * Thread which runs on every node listens for incoming sava master jobs
 */
void sava_master_thread();


void write_line(std::string str, std::string file_name);

/*
 * Thread which runs on every node listens for incoming sava tasks
 */
void sava_worker_thread();

/*
 * Function to split the dataset into 
 *
 * @param parts => number of parts to split the data set into
 * @param file_name => file name of the original data set
 * return error code
 */
int data_set_splitter(int parts, std::string file_name);


/*
 * Function run by master node which dispatches tasks to worker nodes
 * @param info contains node => file mapping 
 */
void master_dispatch_tasks(std::string info);


/*
 * Function run on every worker, it listens to incoming tasks and runs it locally
 * File_name , superstep , task
 * returns status back to the master
 */
void worker_task_listener();

/*
 * Get starting node of each file
 */
std::string get_info(int num_parts);

/*
 * Partition the dataset into multiple files
 */
void global_partitioner(std::string file_name, int num_parts);