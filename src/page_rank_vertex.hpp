#include "vertex.hpp"
#include <map>
/*
 *	Custom class for pagerankvertex
 */
class PageRankVertex : public Vertex<int,int,float>{
	public:
	PageRankVertex(int vertex_id, std::vector<int> outgoing_list)
	: Vertex<int,int,float>(vertex_id,outgoing_list)
	{

	}
	PageRankVertex(int vertex_id)
	: Vertex<int,int,float>(vertex_id)
	{
		step = 0;
	}
	float send_msgs(std::map<int, PageRankVertex>& vertices, std::vector<int> lis,std::string dir, std::string file_name);
	float run_step(std::map<int, PageRankVertex>& vertices, int total_nodes){
		//score = 0;
		if(step == 0){
			score = 1.0;

		}else{
			score = 0.15;
			for(int i=0; i < queue.size();++i){
				score+= 0.85 * queue[i];
			}
			// if(outgoing_list_.size() != 0)
			// 	score = (0.15/(outgoing_list_.size())) + 0.85 * score;
		}


		step++;
		
		return score;
		// score = 0;
		// float df = 0.15;
		// if(queue.size() != 0 ){
		// 	for(int i=0 ; i < queue.size() ; ++i){
		// 		score += (1-df)*score;
		// 	}
		// }
		// if(step == 0)
		// {
		// 	score = 1;
		// }
		// step++;
		// return score;
	}
	float simple_send_msgs(std::map<int, PageRankVertex>& vertices){
		for(int i=0; i < outgoing_list_.size() ; ++i){
			std::map<int,PageRankVertex>::iterator itr = vertices.find(outgoing_list_[i]);
			if(itr != vertices.end()){
				itr->second.queue.push_back(score/outgoing_list_.size());
			}
		}
	}

};