#include <vector>

template <class VertexValue, class EdgeValue>
class Vertex{
public:

	VertexValue vertex_id;
	std::vector<EdgeValue> outgoing_list;
	int step;	
	// virtual void compute() = 0;
	// virtual void send_to_vertex();
};