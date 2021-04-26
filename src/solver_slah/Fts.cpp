#include "solver_slah/Fts.h"
#include "exception/SemanticException.h"

void Fts::init_state_edge(z3::expr pi_i,z3::expr_vector m_pars,int m_z_size){
	std::vector<int> state1;
	std::vector<int> state2;
	for(int j=0;j<m_z_size;j++){
		state1.push_back(0);
		state2.push_back(0);
	}
	
	int index=-1;
	//zsize>1 so pi_i is (and ... (z=n) (z'=n') ...) 
	if(pi_i.decl().name().str()!="and") throw SemanticException("the definition of hck is not well-formed(pi_i). \n");
	
	for(int i=0;i<pi_i.num_args();i++){
		if(index>=m_z_size) throw SemanticException("the definition of hck is not well-formed(size of z=n1 and z=n2 is greater than zsize). \n");
		z3::expr exp1 = pi_i.arg(i);
		if(exp1.arg(0).to_string()==m_pars[1].to_string()) index=0;
		if(index>=0){
			if(exp1.decl().name().str()!="=" || exp1.arg(0).to_string()!=m_pars[1+index].to_string() || !exp1.arg(1).is_numeral() ||i+1>=pi_i.num_args()) 
				throw SemanticException("the definition of hck is not well-formed(z=n1 and z=n2). \n");
			z3::expr exp2 = pi_i.arg(i+1);i++;
			if(exp2.decl().name().str()!="=" || exp2.arg(0).to_string()!=m_pars[2+m_z_size+index].to_string()|| !exp2.arg(1).is_numeral()) 
				throw SemanticException("the definition of hck is not well-formed(z=n1 and z=n2). \n");
			
			state1[index]=exp1.arg(1).get_numeral_int();
			state2[index]=exp2.arg(1).get_numeral_int();
			
			index++;
		}
	}
	if(index<m_z_size) throw SemanticException("the definition of hck is not well-formed(size of z=n1 and z=n2 is less than zsize). \n");
	if(state_set.find(state1)==state_set.end()){
		int stsz=state_set.size();
		state_set.insert(std::pair<std::vector<int> ,int>(state1,stsz));
	}
	if(state_set.find(state2)==state_set.end()){
		int stsz=state_set.size();
		state_set.insert(std::pair<std::vector<int>,int>(state2,stsz));
	}
	edge_set.insert(std::pair<int,int>(state_set[state1],state_set[state2]));
}

void Fts::init(z3::expr_vector m_pars,z3::expr m_data,int m_z_size){
	if(m_z_size==0){
		empty = true;
		return;
	}
	empty = false;
	m_data = m_data.arg(0);//m_data : (and (...)) -> (...)
	//pi is (or pi_1 pi_2 ...)
	if(m_data.decl().name().str() == "or"){
		for(int i=0;i<m_data.num_args();i++){
			z3::expr pi_i=m_data.arg(i);
			init_state_edge(pi_i,m_pars,m_z_size);
		}
	}else{//pi is pi_1
		z3::expr pi_i = m_data;
		init_state_edge(pi_i,m_pars,m_z_size);
	} 
	
	std::set<std::pair<int,int>>::iterator it;
	for(it=edge_set.begin();it!=edge_set.end();it++){
		boost::add_edge(it->first,it->second,graph);
	}
}

void Fts::check_out_degree(){
	//std::cout<<"check_out_degree"<<std::endl;
	if(empty) return;
	myfts_graph::vertex_iterator vertexIt, vertexEnd;
	boost::tie(vertexIt, vertexEnd) = boost::vertices(graph);
    for(; vertexIt != vertexEnd; ++vertexIt){
    	if(boost::out_degree(*vertexIt, graph) > 1)
    		throw SemanticException("the definition of hck is not well-formed(out_degree is greater than 1). \n");
    }
}

int Fts::iscycle(myfts_graph::vertex_iterator vertexIt){
	int cyclelen=0;
	myfts_graph::adjacency_iterator neighbourIt, neighbourEnd;
	boost::tie(neighbourIt, neighbourEnd) = boost::adjacent_vertices(*vertexIt, graph);
	while(*neighbourIt != *neighbourEnd && *neighbourIt!=*vertexIt){
		cyclelen++;
		boost::tie(neighbourIt, neighbourEnd) = boost::adjacent_vertices(*neighbourIt, graph);
	}
	if(*neighbourIt==*vertexIt){
		cyclelen++;
		return cyclelen;	
	}
	return 0;
}

void Fts::make_path_set(){
	if(empty) return;
	myfts_graph::vertex_iterator sourceIt, sourceEnd;
	boost::tie(sourceIt, sourceEnd) = boost::vertices(graph);
	for(; sourceIt != sourceEnd; ++sourceIt){ 
		myfts_graph::vertex_iterator destIt, destEnd;
		boost::tie(destIt, destEnd) = boost::vertices(graph);
		for(;destIt != destEnd; ++destIt){
			int *visited = new int[state_set.size()]();
			
			int min_len=0;
			visited[*sourceIt]=1;
			myfts_graph::adjacency_iterator neighbourIt, neighbourEnd;
			boost::tie(neighbourIt, neighbourEnd) = boost::adjacent_vertices(*sourceIt, graph);
			while(*neighbourIt != *neighbourEnd && *neighbourIt!=*destIt && visited[*neighbourIt]!=1){//走到尽头，或者走到dest，或者有环且没找到 
				min_len++;
				visited[*neighbourIt]=1;
				boost::tie(neighbourIt, neighbourEnd) = boost::adjacent_vertices(*neighbourIt, graph);
			}
			if(*neighbourIt==*destIt){
				min_len++;
				path newPath;
				newPath.min_len=min_len;
				std::map<std::vector<int>,int>::iterator it=state_set.begin();
				for(;it!=state_set.end();it++){
					if(it->second == *sourceIt){
						for(int i=0;i<it->first.size();i++) newPath.source.push_back(it->first[i]);
					}
					if(it->second == *destIt){
						for(int i=0;i<it->first.size();i++) newPath.dest.push_back(it->first[i]);
					}
				}
				int cylen=iscycle(destIt);
				if(cylen>0){
					newPath.type=1;
					newPath.cycle_len=cylen;
				}else{
					newPath.type=0;
				}
				path_set.push_back(newPath);
			}
			delete[] visited;
		}
	}
}

void Fts::make_transition_set(){
	if(empty) return;
	myfts_graph::vertex_iterator sourceIt, sourceEnd;
	boost::tie(sourceIt, sourceEnd) = boost::vertices(graph);
	for(; sourceIt != sourceEnd; ++sourceIt){ 
		myfts_graph::adjacency_iterator neighbourIt, neighbourEnd;
		boost::tie(neighbourIt, neighbourEnd) = boost::adjacent_vertices(*sourceIt, graph);
		for(; neighbourIt != neighbourEnd; ++neighbourIt){
			path tran;
			tran.min_len=1;
			std::map<std::vector<int>,int>::iterator it=state_set.begin();
			for(;it!=state_set.end();it++){
				if(it->second == *sourceIt){
					for(int i=0;i<it->first.size();i++) tran.source.push_back(it->first[i]);
				}
				if(it->second == *neighbourIt){
					for(int i=0;i<it->first.size();i++) tran.dest.push_back(it->first[i]);
				}
			}
			transition_set.push_back(tran);
		}
	}
}

void Fts::print() {
	std::cout << "print fts:---------------------------\n" ;
	if(empty) return;
	std::cout<<"states:"<<std::endl;
	std::map<std::vector<int>,int>::iterator it=state_set.begin();
	for(;it!=state_set.end();it++){
		for(int i=0;i<it->first.size();i++) std::cout<< it->first[i] <<" ";
		std::cout<<"--------------------"<<it->second<<std::endl;
	}
	std::cout<<"transitions:"<<std::endl;
	myfts_graph::vertex_iterator vertexIt, vertexEnd;
	myfts_graph::adjacency_iterator neighbourIt, neighbourEnd;
	boost::tie(vertexIt, vertexEnd) = boost::vertices(graph);
	for(; vertexIt != vertexEnd; ++vertexIt){ 
		std::cout << *vertexIt << " ----> "; 
		boost::tie(neighbourIt, neighbourEnd) = boost::adjacent_vertices(*vertexIt, graph); 
		for (; neighbourIt != neighbourEnd; ++neighbourIt) 
			std::cout << *neighbourIt << " "; 
		std::cout << "\n"; 
	}	
}

void Fts::print_path(){
	std::cout << "print path:---------------------------\n";
	if(empty) return;
	for(int i=0;i<path_set.size();i++){
		std::cout << "-------path "<<i<<" : \n";
		std::cout<<"source: ";
		for(int j=0;j<path_set[i].source.size();j++){
			std::cout<<path_set[i].source[j]<<" ";
		}
		std::cout<<"dest: ";
		for(int j=0;j<path_set[i].dest.size();j++){
			std::cout<<path_set[i].dest[j]<<" ";
		}
		std::cout<<"type: "<<path_set[i].type<<" ";
		std::cout<<"min_len: "<<path_set[i].min_len<<" ";
		std::cout<<"cycle_len: "<<path_set[i].cycle_len<<"\n";
	}
}

void Fts::print_transition(){
	std::cout << "print path:---------------------------\n";
	if(empty) return;
	for(int i=0;i<transition_set.size();i++){
		std::cout << "-------transition "<<i<<" : \n";
		std::cout<<"source: ";
		for(int j=0;j<transition_set[i].source.size();j++){
			std::cout<<transition_set[i].source[j]<<" ";
		}
		std::cout<<"dest: ";
		for(int j=0;j<transition_set[i].dest.size();j++){
			std::cout<<transition_set[i].dest[j]<<" ";
		}
		std::cout<<"\n";
	}
}

