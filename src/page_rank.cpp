/*
 * Page Rank for the Distributed Graph Processing System
 *
 * each node has a server which receives input
 * 
 */

#include <fstream>
#include <iostream>
#include <vector>
#include "vertex.hpp"
#include <mutex>
#include <boost/thread.hpp>

int done;
boost::mutex done_lock;

void runner(int total_size, int step, Vertex<int,int> v, std::map<int,std::vector<int> >& msg_buf ){
	//std::cout << "In thread vertex " << v.vertex_id ;
	float score = 0;
	if(step == 0){
		score = 1.0/total_size;
		//std::cout << " score is " << score << std::endl;
	}
	else{
		if(msg_buf.find(v.vertex_id) != msg_buf.end()){
			for(int i=0;i < msg_buf.size() ; ++i){
				score += msg_buf[v.vertex_id][i];
			}

		}
		msg_buf[v.vertex_id].clear();
		score = (0.15/(v.outgoing_list.size())) + 0.85 * score;
		
	}
	if(v.vertex_id == 522009){
				std::cout << " score is " << score << std::endl;
	}
	for(int i=0; i < v.outgoing_list.size() ; ++i){
		//std::map<int,std::vector<int> >::iterator it = msg_buf.find(v.outgoing_list[i]);
		if(msg_buf.find(v.outgoing_list[i]) != msg_buf.end() ){
			msg_buf[v.outgoing_list[i]].push_back( score/v.outgoing_list.size());
		}else{
			msg_buf[v.outgoing_list[i]] = std::vector<int>();
			msg_buf[v.outgoing_list[i]].push_back(score/v.outgoing_list.size());
		}
	}
	done_lock.lock();
	done++;
	done_lock.unlock();
	//v.send_to_vertex();
}

void message_listener(){

}
/*
 * Function to partition the data set into workers
 */
void global_partitioner(){

}

/*
 * Function to locally partition the data. Runs on the local machine
 */
void local_partioner(std::vector<Vertex<int,int> > vertices,int total_size, int step){
	boost::thread_group grp;
	std::map<int,std::vector<int> > msg_buf;
	for(int itr=0; itr < 12; ++itr)
	{
		done = 0;
		std::cout << "IN itr " << itr << std::endl;
		for(int i=0; i < vertices.size() ; ++i){
			//std::cout << vertices[i].vertex_id << std::endl;
			boost::thread t(runner,total_size,itr, vertices[i], msg_buf);
			// grp.add_thread(t);
			//grp.create_thread( boost::bind( runner , total_size , step , vertices[i] ) );
		}
		while(done != vertices.size()){
			//std::cout << done << std::endl;
		}
		std::cout << "Done Spinning \n";
	}
	
	//grp.join_all();
}

int main(){
	std::ifstream infile("../test/dataset.txt");
	int a,b;
	std::vector<Vertex<int,int> > vertices;
	int prev = 0;
	while(infile >> a >> b){
		if(prev != a){
			Vertex<int,int> v;
			v.vertex_id = a;
			v.outgoing_list.push_back(b);
			vertices.push_back(v);
		}else{
			vertices[vertices.size() - 1].outgoing_list.push_back(b);
		}
		prev = a;
	}
	std::cout << "Inserted " << vertices.size() << " vertices\n";
	local_partioner(vertices, vertices.size(), 0 );

}