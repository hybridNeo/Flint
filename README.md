Distributed Graph Processing System :

Have a sequence of iterations called supersteps.
During each superstep a user defined function is called for each vertex.

The function specifies behavior at a single vertex V and a single superstep S

Imstructions to run:
run client INTRODUCER IP -i on introducer

run client INTRODUCER on the other nodes

C++ API:
involves subclassing the predefined vertex class 

Compute() will be executed at each superstep 


Message passing b/w vertices : udp_sendmsg()


class Vertex{
	virtual void run_step(MessageIterator* msgs) = 0;
	const string& vertex_id() const;
	int64 superstep() const;

	const VertexValue& GetValue();
	VertexValue* MutableValue;
	OutEdgeIterator GetOutEdgeIterator();

	void send_msg_to(const string& dest_vertex , const MessageValue& message);
	void VoteToHalt();
}

