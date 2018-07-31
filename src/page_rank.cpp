// /*
//  * Page Rank for the Distributed Graph Processing System
//  *
//  * each node has a server which receives input
//  * 
//  */

#include <fstream>
#include <iostream>
#include <vector>
#include "page_rank_vertex.hpp"
#include "sava.hpp"
#include <mutex>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

void write_log(std::string str){

  std::ofstream ofs;
  ofs.open ("out", std::ofstream::out | std::ofstream::app);

  ofs << str << std::endl;

  ofs.close();
}

void find_mx(std::map<int, PageRankVertex > vertices){
	
	
	for(int i=0; i < 20; ++i){
		int max = 0;
		std::map<int, PageRankVertex>::iterator bt ;
		std::map<int, PageRankVertex>::iterator it = vertices.begin();	
		while(it != vertices.end()){
			if(it->second.score > max){
				
				max = it->second.score;
				bt = it;
			}
			it++;
			
		}
		std::cout << i+1 << ". " << bt->first << " " << bt->second.score << std::endl;
		vertices.erase(bt->first);
	}


}

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


// int done;
// boost::mutex done_lock;
// boost::mutex map_lock;

// void runner(int total_size, int step, Vertex<int,int> v, std::map<int,std::vector<float> >& 	 ){
// 	//std::cout << "In thread vertex " << v.vertex_id ;
// 	float score = 0;
// 	if(step == 0){
// 		score = 1.0/total_size;
// 		//std::cout << " score is " << score << std::endl;
// 	}
// 	else{
// 		map_lock.lock();
// 		if(msg_buf.find(v.vertex_id) != msg_buf.end()){
// 			std::cout << "here\n";
// 			for(int i=0;i < msg_buf[v.vertex_id].size() ; ++i){
// 				score += msg_buf[v.vertex_id][i];
// 			}

// 		}
// 		map_lock.unlock();
// 		msg_buf[v.vertex_id].clear();
// 		score = (0.15/(v.outgoing_list.size())) + 0.85 * score;
		
// 	}
// 	if(v.vertex_id == 79910 ){
// 				std::cout << "Score is " << score << std::endl;
// 				std::cout << "Send score is " << score/v.outgoing_list.size() << std::endl;
// 				std::cout << "Vertex id is " << v.vertex_id << std::endl ; 
// 				if(msg_buf.find(v.vertex_id) != msg_buf.end()){
// 				for(int i=0;i < msg_buf[v.vertex_id].size() ; ++i){
// 					std::cout <<msg_buf[v.vertex_id][i];
// 				}

// 		}
// 	}
// 	map_lock.lock();
// 	for(int i=0; i < v.outgoing_list.size() ; ++i){
// 		//std::map<int,std::vector<int> >::iterator it = msg_buf.find(v.outgoing_list[i]);

// 		if(msg_buf.find(v.outgoing_list[i]) != msg_buf.end() ){
// 			if(v.outgoing_list[i] == 79910){
// 				std::cout << "Found this guy \n" ; 
// 			}
// 			msg_buf[v.outgoing_list[i]].push_back( score/v.outgoing_list.size());
// 		}else{
// 			if(v.outgoing_list[i] == 79910){
// 				std::cout << "Not Found this guy \n" ; 
// 			}
			
// 			{
// 				std::vector<float> vec;
// 				vec.push_back(score/v.outgoing_list.size());
// 				msg_buf.insert(std::make_pair(v.outgoing_list[i],vec));
// 			}
// 			if(msg_buf.find(v.outgoing_list[i]) != msg_buf.end() ){
// 				//std::cout << "Yes\n";
// 			}else{
// 				std::cout << "No\n" ;
// 			}
// 			//msg_buf[v.outgoing_list[i]] = std::vector<float>();
// 			msg_buf[v.outgoing_list[i]].push_back(score/v.outgoing_list.size());
// 		}
// 	}
// 	map_lock.unlock();
// 	done_lock.lock();
// 	done++;
// 	done_lock.unlock();
// 	//v.send_to_vertex();
// }

// void message_listener(){

// }

std::string get_info(int num_parts){
	std::string res = "";
	for(int i=0; i < num_parts; ++i){
		std::ifstream infile;
		std::string file_name = "xaa";
		file_name[2] += i;
		infile.open(file_name);
		std::cout << file_name << std::endl;
		//std::string line;
		int first = 0;
		infile >> first;
		res += std::to_string(first) + ",";
		//std::cout << first << std::endl;
		//std::getline(infile,line);
		//std::cout << line  << std::endl;
		infile.close();
	}
	return res;
	
}

/*
 * Function to partition the data set into workers
 */
void global_partitioner(std::string file_name, int num_parts){
	std::map<int,int> part_info;
	char cmd[256] = "wc -l ";
	std::string exec_res = exec(strcat(cmd, file_name.c_str()));
	std::vector<std::string> vs1;
	boost::split(vs1, exec_res , boost::is_any_of(" "));
	int num_lines = stoi(vs1[0]);
	std::cout << "Number of lines is " << num_lines << std::endl;
	int lines_per_part =  (num_lines + num_parts - 1)/num_parts;
	std::cout << "Number of lines per part is " << lines_per_part << std::endl;
	//char split[256] = "split --lines=";
	std::string command = ( "split --lines=" + std::to_string(lines_per_part) + " " + file_name );
	exec(command.c_str());
}

// /*
//  * Function to locally partition the data. Runs on the local machine
//  */
// void local_partioner(std::vector<Vertex<int,int> > vertices,int total_size, int step){
// 	boost::thread_group grp;
// 	//std::map<int,std::vector<float>> temp;
// 	std::map<int,std::vector<float> > msg_buf;
// 	for(int itr=0; itr < 12; ++itr)
// 	{
// 		done = 0;
// 		std::cout << "IN itr " << itr << std::endl;
// 		for(int i=0; i < vertices.size() ; ++i){
// 			//std::cout << vertices[i].vertex_id << std::endl;
// 			boost::thread t(runner,total_size,itr, vertices[i], msg_buf);
// 			// grp.add_thread(t);
// 			//grp.create_thread( boost::bind( runner , total_size , step , vertices[i] ) );
// 		}
// 		while(done != vertices.size()){
// 			//std::cout << done << std::endl;
// 		}
// 		std::cout << "Done Spinning \n";
// 	}
	
// 	//grp.join_all();
// }




int main(){
	//global_partitioner("../test/dataset.txt", 7 );

	std::cout << get_info(7);
	std::ifstream infile("../test/dataset.txt");
	int a,b;
	std::map<int, PageRankVertex > vertices;
	int prev = 0;
	int lines = 0;

	while(infile >> a >> b){
		lines++;
		PageRankVertex v(a);
		std::map<int, PageRankVertex>::iterator it = vertices.find(a);
		if(it == vertices.end()){
			v.outgoing_list_.push_back(b);
			vertices.insert(std::pair<int,PageRankVertex>(a,v));
		}else{
			it->second.outgoing_list_.push_back(b);
		}
		it = vertices.find(b);
		if( it == vertices.end()){
			PageRankVertex v(b);
			vertices.insert(std::pair<int,PageRankVertex>(b,v));
		}
		// if(prev != a){
		// 	PageRankVertex v(a);
		// 	v.vertex_id_ = a;
		// 	v.outgoing_list_.push_back(b);
		// 	//vertices[a] = v;
		// 	vertices.insert(std::pair<int,PageRankVertex>(a,v));
		// }else{
		// 	std::map<int, PageRankVertex>::iterator it = vertices.find(a);
		// 	if(it != vertices.end()){
		// 		it->second.outgoing_list_.push_back(b);
		// 	}
		// 	//vertices[a].outgoing_list_.push_back(b);
		// }
		// prev = a;
	}
	int count = 0;
	std::map<int, PageRankVertex>::iterator it = vertices.begin();
	while(it != vertices.end()){
		count += it->second.outgoing_list_.size();
		it++;
	}
	std::cout << lines <<"lines "<< "Inserted " << vertices.size() << " vertices and edges = " << count << std::endl;
	for(int i=0;i <= 20; ++i){
		std::cout << "Iteration : " << i << std::endl;
		std::map<int, PageRankVertex>::iterator it = vertices.begin();
		while(it != vertices.end()){
			float val = it->second.run_step(vertices, vertices.size());
			//if(it->first == 364470)
			std::cout << it->first << ":" << val <<  std::endl;
			write_log(std::to_string(it->first) + " " + std::to_string(val) + "\n" );
			it++;
		}
		it = vertices.begin();
		while(it != vertices.end()){
			//std::cout << "size" << it->second.queue.size() << std::endl;
			it->second.queue.clear();
			//std::cout << "size" << it->second.queue.size() << std::endl;

			it++;
		}
		it = vertices.begin();
		while(it != vertices.end()){
			it->second.simple_send_msgs(vertices);
			it++;
		}


	}
	find_mx(vertices);
	// //local_partioner(vertices, vertices.size(), 0 );

}