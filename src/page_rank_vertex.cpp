#include "com.hpp"
#include "page_rank_vertex.hpp"
#include "sava.hpp"
#include <boost/thread.hpp>

#include "client.hpp"
float PageRankVertex::send_msgs(std::map<int, PageRankVertex>& vertices, std::vector<int> lis,std::string dir, std::string file_name){
		std::vector<std::string> ips;
//		std::cout << "\n\n\nlis is " << lis.size() << "\n" ;
		for(int i=0; i < outgoing_list_.size() ; ++i){
			std::map<int,PageRankVertex>::iterator itr = vertices.find(outgoing_list_[i]);
			if(itr != vertices.end()){
				itr->second.queue.push_back(score/outgoing_list_.size());
			}else{
				std::string a = "abcd";
//				std::cout << "\n\n\nlis is " << lis.size() << "\n" ;

				int prev = lis[0];
				int wrk = 0;
				for(int j =1 ; j < lis.size()-1; ++j ){
					if(lis[j] > outgoing_list_[i] )
					{
						break;
					}
					wrk = j;
				}
				//std::cout << "outside " << wrk << std::endl;
				a[0] += wrk;
				if(a[0] == file_name[0]){
					std::cout << "here: \n";
					std::cout << a << ": " << file_name << std::endl;
					std::cout << outgoing_list_[i] << std::endl;

				}
				
				//std::vector<std::string> ips = where_to(a);
				float val = score/outgoing_list_.size();
				std::string response;
				//udp_sendmsg(std::to_string(outgoing_list_[i]) + ";" + std::to_string(val), ips[0],SAVA_MSG_PORT ,response );
				write_line(std::to_string(outgoing_list_[i]) + " " + std::to_string(val), dir + file_name + "-" + a);
				//udp_sendmsg(a,);
			}
			// for(int j =0; j < lis.size()-1; ++j){
			// 	std::string a = "abcd";
			// 	a[0] += j;
			// 	write_line(std::to_string(outgoing_list_[i]) + " " + std::to_string(val), dir + a + file_name )
			// }

		}
		
	}