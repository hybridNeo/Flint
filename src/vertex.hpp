#include <vector>

template <class VertexValue, class EdgeValue, class MessageValue>
class Vertex{
public:

	VertexValue vertex_id_;
	std::vector<EdgeValue> outgoing_list_;
	int step;	
	MessageValue score;
	std::vector<MessageValue> queue;
	std::vector<MessageValue> next_queue;
	virtual MessageValue run_step(){

	}
	Vertex(int vertex_id , std::vector<EdgeValue> outgoing_list){
		vertex_id_ = vertex_id;
		outgoing_list_ = outgoing_list;
	}
	Vertex(int vertex_id ){
		vertex_id_ = vertex_id;
	}
	// virtual void compute() = 0;
	// virtual void send_to_vertex();
};