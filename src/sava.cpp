#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "sava.hpp"
#include "com.hpp"
#include "client.hpp"
#include <boost/algorithm/string.hpp>
#include <map>
#include <boost/thread.hpp>
#include <fstream>
#include "ftplib.hpp"
#include "page_rank_vertex.hpp"

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

void write_line(std::string str, std::string file_name){

  std::ofstream ofs;
  ofs.open (file_name, std::ofstream::out | std::ofstream::app);

  ofs << str << std::endl;

  ofs.close();
}
/*
 * This is a request handler for the sava master
 * @param string request 
 * @param string r_ep
 * TYPE of requests
 * 1 - START_TASK - 1;task_name;num_workers;nums_steps;1,222,3333,
 * 2 - COMPLETED_SUPERSTEP  - 2;ip_addr;number
 * 3
 */
std::map<std::string,int > task_status;
std::map<int, PageRankVertex > vertices;
std::vector<std::string> buffer;
boost::mutex task_status_lock;

void work_maker(std::string message,std::vector<std::string>& ips,std::string a){
	std::string response = "";
	try{

		udp_sendmsg(message,ips[0],SAVA_WORKER_PORT,response);

	}catch(...){
		std::cout << "Node Failed\n";
		
	}
	if(response != "pong"){
		std::cout << "Failed " << response << std::endl;
		
	}
	
}



std::string sava_master_handler(std::string& request , udp::endpoint r_ep){
    std::cout << "SAVA_MASTER: The entered request is " << request << std::endl ;
    std::vector<std::string> vs1;

    boost::split(vs1, request , boost::is_any_of(";"));

    if(vs1[0] == "1"){
    	// START_TASK
    	if(vs1.size() < 4 ){
    		return "ERROR";
    	}
    	//task_status[r_ep.address().to_string()] = std::vector<int>();
    	std::cout << "START_TASK " << vs1[1] << std::endl;
    	
    	for(int i=0; i < stoi(vs1[3]) ; ++i){
    		//bad spin lock
    		std::cout << "Step " << i << std::endl;
    		boost::thread_group superstep_threads;
    		for(int j=0; j < stoi(vs1[2]) ;++j){
    			//char a = 'a' + j;
    			std::string a = "a";
    			a[0] += j;
    			std::vector<std::string> ips = where_to(a + "-" + vs1[1]);
    			std::string response;
    			std::string message = r_ep.address().to_string() + ";" + (a+"-"+vs1[1]) + ";" + vs1[1] + ";" + std::to_string(j)+ ";" + std::to_string(i) + ";" + vs1[4];
    			boost::thread *t = new boost::thread(work_maker, message,ips, a);
    			superstep_threads.add_thread(t);
    			//udp_sendmsg(message , ips[0], SAVA_WORKER_PORT, response);
    		}
    		//while(1);
    		superstep_threads.join_all();
    		//while(task_status[r_ep.address().to_string()].size() != stoi(vs1[3]));
    	}

    }else if(vs1[0] == "2"){
    	// SUPERSTEP_COMPLETED
    	task_status_lock.lock();
    	task_status[vs1[1]] = stoi(vs1[2]);
    	//task_status[vs1[1]].push_back(stoi(vs1[2]));
    	task_status_lock.unlock();

    }

    return "pong\n";

}


//1,pagerank
std::string sava_message_handler(std::string& request, udp::endpoint r_ep){
	

	buffer.push_back(request);
	// std::map<int,PageRankVertex>::iterator it = vertices.find(node);
	// if(it != vertices.end()){
	// 	it->second.queue.push_back(value);
	// }
	
	return "pong\n";
}

void sava_msg_thread(){
	try{

        boost::asio::io_service io_service;
        udp_server server(io_service, SAVA_MSG_PORT, sava_message_handler);
		std::cout << "[SAVA] Message thread ready.\n";
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


/*
 * This is a request handler for the sava worker
 * @param string request 
 * @param string r_ep
 * request => ip_addr, file , function , worker_id , iteration ,  data
 */
int step = 0;

std::string sava_worker_handler(std::string& request , udp::endpoint r_ep){
    std::cout << "[SAVA-WORKER]The entered request is " << request << std::endl;
    std::vector<std::string> vs1,li;
    std::vector<int> lis;
	//static std::map<int, PageRankVertex > vertices;
    boost::split(vs1, request , boost::is_any_of(";"));
    boost::split(li,vs1[5] , boost::is_any_of(","));
   	for(int i = 0; i < li.size() ; ++i){
   		lis.push_back(stoi(li[i]));
   	}
    int total_size = 334863; //TODO
    std::string iteration = vs1[4];
    if(vs1[2] == "pagerank"){
    	if(iteration == "0")
    	{
    		boost::thread t1(exec, "rm ./uploads/*");
    		boost::thread t2(exec, "rm ./downloads/*");
    		vertices.clear();
    		std::cout << "START_TASK pagerank iteration : " << iteration;
	    	std::string f_name = vs1[1];
	    	//std::cout << "file name is " << f_name << std::endl;
	    	std::ifstream infile("./uploads/" + vs1[1]);
	    	int a,b;
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
	    		if(it == vertices.end()){
	    			int wrk = 0;
					for(int j =1 ; j < lis.size()-1; ++j ){
						if(lis[j] > b )
						{
							break;
						}else
							wrk = j;
					}
					if(wrk ==stoi(vs1[3]) || (b < lis[1]) ){
						//std::cout << "berrrrrrrrrrrrrere1\n" ;
						PageRankVertex v(b);
	    				vertices.insert(std::pair<int,PageRankVertex>(b,v));
					}
	    			
	    		}
	    	}
	    	std::cout << "lines is : " << lines << std::endl;
    	}else{
    		if(vertices.size() == 0){
    			std::cout << "Empty\n";
    		}else{
    			std::cout << "Not Empty\n";
    		}

    	}
   //  	for(int i=0 ; i < buffer.size(); ++i){
   //  		std::vector<std::string> vs;
   //  		boost::split(vs,buffer[i] , boost::is_any_of(";"));
			// int node = stoi(vs[0]);
			// float value = std::stof(vs[1]);
			// std::map<int,PageRankVertex>::iterator it = vertices.find(node);
			// if(it != vertices.end()){
			// 	it->second.queue.push_back(value);
			// }
   //  	}
   //  	buffer.clear();
    	if(iteration != "0")
    	{
	    	for(int j=0 ; j < lis.size()-1;++j){
	    		std::string file_name = "a-pagerank";
	    		std::cout << "itr " << iteration << std::endl;
	    		//iteration = "0";
	    		int val = stoi(iteration) - 1;
	    		std::string new_itr = std::to_string(val);
	    		std::string late = "abcd";
	    		late[0] += stoi(vs1[3]);
	    		//file_name[15] += stoi(iteration);
	    		file_name[0] += j;
	    		file_name += new_itr + "msg-" + late;
	    		
	    		std::vector<std::string> ips = where_to(file_name);
	    		//std::cout << "Computed filename is " << file_name << ":" << ips[0] << std::endl;
	    		//std::cout << "iteration : " << new_itr << " " << file_name <<  std::endl;
	    		download_file((char *)ips[0].c_str(),  FTP_SERVER_PORT , "./downloads/", (char *)file_name.c_str(),(char *) file_name.c_str());

	    		std::ifstream f("./downloads/" + file_name);
		    	int a;
		    	float b;
		    	int lines = 0;
		    	while(f >> a >> b){
		    		//std::cout << "EEEEEEEEEEEEEEEE\n" << file_name;
		    		std::map<int,PageRankVertex>::iterator it = vertices.find(a);

					if(it != vertices.end()){
						it->second.queue.push_back(b);
					}
		    	}

	    	}
    	}
		
    	std::map<int, PageRankVertex>::iterator it = vertices.begin();
    	while(it != vertices.end()){
    		float val = it->second.run_step(vertices,total_size);
    		std::string file_name = "abcd-pagerank";
    		//file_name[5] += j;
    		//file_name[15] += stoi(iteration);
    		file_name[0] += stoi(vs1[3]);
    		file_name += iteration + "out";
    		write_line(std::to_string(it->first) + " " + std::to_string(val), "./uploads/" + file_name );
    		if(iteration == "19")
    			std::cout << val << std::endl;
    		it++;
    	}
    	std::cout << "Computation complete\n";
    	it = vertices.begin();
    	while(it != vertices.end()){
    		it->second.queue.clear();
    		it++;
    	}
    	std::cout << "Clearing complete\n";
    	it = vertices.begin();
    	while(it != vertices.end()){
    		it->second.send_msgs(vertices, lis, "./uploads/" ,  ( vs1[1] + iteration + "msg" ));
    		it++;
    	}
    	std::cout << "sending complete\n";
    	find_mx(vertices);


    }
    return "pong";

}


void sava_master_thread(){
	try{

        boost::asio::io_service io_service;
        udp_server server(io_service, SAVA_MASTER_PORT, sava_master_handler);
		std::cout << "[SAVA] Master thread ready.\n";
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void sava_worker_thread(){
	//boost::thread t(sava_msg_thread);
	try{
        boost::asio::io_service io_service;
        udp_server server(io_service, SAVA_WORKER_PORT, sava_worker_handler);
		std::cout << "[SAVA] Worker thread ready.\n";
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
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
	std::cout << "Number of lies per part is " << lines_per_part << std::endl;
	//char split[256] = "split --lines=";
	std::string command = ( "split --lines=" + std::to_string(lines_per_part) + " " + file_name );
	exec(command.c_str());
}

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