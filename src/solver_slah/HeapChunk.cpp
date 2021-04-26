#include "solver_slah/HeapChunk.h"
#include "exception/SemanticException.h"


extern z3::context z3_ctx;

HeapChunk::HeapChunk(z3::func_decl fun, z3::expr_vector pars, z3::expr exp, int ex_size)
: m_fun(fun), m_pars(pars), m_data(z3_ctx), m_space(z3_ctx), 
  m_ex_size(ex_size),type(0),v_index(-1),min_bound(1),max_bound(2147483647){
		if (!exp.is_app() || exp.decl().name().str() != "and") {
		    throw SemanticException("'and' is excepted in the definition of hck");
		}
		// 1. data
		z3::expr_vector data_items(z3_ctx);
		int i=0;
		for (; i<exp.num_args(); i++) {
			assert(exp.arg(i).is_app());
			if (exp.arg(i).decl().name().str() != "sep") {
		        data_items.push_back(exp.arg(i));
			}else {
		        break;
		    }
		}
		if (i==exp.num_args()) {
		    throw SemanticException("'sep' is excepted in the definition of hck");
		}
		
		m_data = mk_and(data_items);
		
		z3::expr sep = exp.arg(i);
		
		//2. ptos,blks
		for (int i=0; i<sep.num_args(); i++) {
			if (sep.arg(i).decl().name().str() == "pto" || sep.arg(i).decl().name().str() == "blk") {
		        m_space.push_back(sep.arg(i));
		    }else {
		        throw SemanticException("'pto' or 'blk' is excepted in the definition of hck");
			}
		}
		
		if(m_space.size()==0){
			throw SemanticException("'pto' or 'blk' is excepted in the definition of hck");
		}
  }


void HeapChunk::make_fts(){
	m_fts.init(m_pars,m_data,m_z_size);
	m_fts.check_out_degree();
	//m_fts.print();
	m_fts.make_path_set();
	m_fts.make_transition_set();
	//m_fts.print_path();
	//m_fts.print_transition();
}

void HeapChunk::set_v(){
 	z3::expr y_x = m_pars[1+m_z_size] - m_pars[0];
 	
 	z3::expr data=m_data;
 	if(m_data.num_args()==1){
 		data=m_data.arg(0);//(and (...))->(...)
	}
    
    z3::expr first_con=data;
    if(data.decl().name().str()=="or"){
    	first_con=data.arg(0);//pix and piw and piz
    	
    	//获得所有在data中出现的v，后面matchP要用 
    	for(int i=1;i<data.num_args();i++){
    		z3::expr coni=data.arg(i);
    		if(coni.decl().name().str() == "and"){
    			if(coni.arg(1).arg(0).to_string() == y_x.to_string()&&coni.arg(1).decl().name().str()=="<=" && !coni.arg(1).arg(1).is_numeral()){
    				for(int j=0;j<m_pars.size();j++){
						if(coni.arg(1).arg(1).to_string()==m_pars[j].to_string()){
							v_index_set.push_back(j);
							break;
						}
					}
				}
			}
		}
	}
	
	min_bound=m_space.size();
	if(first_con.decl().name().str() == "and"){
		if(first_con.arg(0).arg(0).to_string() == y_x.to_string()){
			if(first_con.arg(0).decl().name().str()==">=" && first_con.arg(0).arg(1).is_numeral()){
				min_bound=std::max(min_bound,first_con.arg(0).arg(1).get_numeral_int());
			}else{
				throw SemanticException("the definition of hck is not well-formed(y-x>=n). \n");
			}
			
			if(first_con.arg(1).arg(0).to_string() == y_x.to_string()){
				if(first_con.arg(1).decl().name().str()=="<=" && first_con.arg(1).arg(1).is_numeral()){
					type=1;
					max_bound=std::min(max_bound,first_con.arg(1).arg(1).get_numeral_int());
				}else{
					type=2;
					for(int j=0;j<m_pars.size();j++){
						if(first_con.arg(1).arg(1).to_string()==m_pars[j].to_string()){
							v_index=j;
							v_index_set.push_back(j);
							break;
						}
					}
					if(v_index==-1) throw SemanticException("the definition of hck is not well-formed(y-x<=vj). \n");
				}	
			}
		}
	}else{
		if(first_con.arg(0).to_string()==y_x.to_string()){
			if(first_con.decl().name().str()==">=" && first_con.arg(1).is_numeral()){
				min_bound=std::max(min_bound,first_con.arg(1).get_numeral_int());
			}else{
				throw SemanticException("the definition of hck is not well-formed(y-x>=n). \n");
			}
		}
	}
}

void HeapChunk::get_summ(z3::expr &summ,int &k_index,z3::expr_vector args){//args is P's args
	//m_fts.print_path();
	std::string k_name = "[k,"+std::to_string(k_index)+"]";k_index++;
    z3::expr var_k = z3_ctx.constant(z3_ctx.str_symbol(k_name.c_str()),z3_ctx.int_sort()); //ki
    
/*
 * we request the first y-x constraint can be entailed by other y-x constraint.
 * so we just consider the first y-x constraint.
 *
 */
 	z3::expr y_x = m_pars[1+m_z_size] - m_pars[0];
	z3::expr arg_vj(z3_ctx);
	if(v_index!=-1) arg_vj=args[v_index];//实参 
	
    
    z3::expr fts_abs=z3_ctx.bool_val(false);//ki可以取得值 
    std::vector<path> path_set=m_fts.get_path_set();
	
	z3::expr arg_y_x = args[1+m_z_size] - args[0];
	if(type==0 || type==1){
		if(path_set.size()==0){//min_len=1,b=0,cycle_len=1
			fts_abs=z3_ctx.bool_val(true);
		}
		for(int i=0;i<path_set.size();i++){
	    	z3::expr path_abs=z3_ctx.bool_val(true);
	    	for(int j=0;j<m_z_size;j++){//z=source
	    		path_abs=path_abs && (args[1+j] == path_set[i].source[j]);
			}
			for(int j=0;j<m_z_size;j++){//u=dest
	    		path_abs=path_abs && (args[m_z_size+2+j] == path_set[i].dest[j]);
			}
	    	if(path_set[i].type==0){//k=n
	    		path_abs=path_abs && (var_k == path_set[i].min_len);
			}else{// k>=min_len and k % cycle_len = min_len % cycle_len
				int b = path_set[i].min_len % path_set[i].cycle_len;
				path_abs = path_abs && var_k >= path_set[i].min_len && mod(var_k,path_set[i].cycle_len) == b;
			}
			fts_abs = fts_abs || path_abs;
		}
		if(type==0){//min_bound<=y-x
			z3::expr body=(var_k > 0) && (arg_y_x >= (min_bound * var_k)) && fts_abs;
			summ=exists(var_k, body);
		}else{//min_bound <= y-x <= max_bound
			z3::expr body=(var_k > 0) && (arg_y_x >= (min_bound * var_k)) && (arg_y_x <= (max_bound * var_k)) && fts_abs;
			summ=exists(var_k, body);
		}
	}else{//min_bound <= y-x <= arg_vj
		if(path_set.size()==0){//min_len=1,b=0,cycle_len=1
			path emppath;
			emppath.cycle_len=1;
			emppath.min_len=1;
			emppath.type=1;
			path_set.push_back(emppath);
		}
		for(int i=0;i<path_set.size();i++){
	    	z3::expr path_abs=z3_ctx.bool_val(true);
	    	if(!m_fts.isempty()){
	    		for(int j=0;j<m_z_size;j++){//z=source
	    			path_abs=path_abs && (args[1+j] == path_set[i].source[j]);
				}
				for(int j=0;j<m_z_size;j++){//u=dest
		    		path_abs=path_abs && (args[m_z_size+2+j] == path_set[i].dest[j]);
				}
			}
			
	    	if(path_set[i].type==0){//k=n
	    		path_abs=path_abs && (arg_y_x >= (min_bound * path_set[i].min_len)) && (arg_y_x <= (arg_vj * path_set[i].min_len));
			}else{// k>=min_len and k % cycle_len = min_len % cycle_len
				int b = path_set[i].min_len % path_set[i].cycle_len;
				int k0=path_set[i].min_len;
				while(k0<min_bound*path_set[i].cycle_len) k0+=path_set[i].cycle_len;
				
				int tmp1=b*min_bound;
				int tmp2=min_bound*path_set[i].cycle_len;
				z3::expr case1=(arg_vj == min_bound) && (arg_y_x >= min_bound*path_set[i].min_len) && (mod(arg_y_x,tmp2)==tmp1);
				
				z3::expr intervals_abs=z3_ctx.bool_val(false);
				int s=path_set[i].min_len;;
				while(s<min_bound*path_set[i].cycle_len){
					intervals_abs=intervals_abs || ((arg_y_x >= min_bound*s) && (arg_y_x <= arg_vj*s));
					s+=path_set[i].cycle_len;
				}
				z3::expr case2=(arg_vj >= (min_bound+1)) && ((arg_y_x >= (k0*min_bound)) || intervals_abs);
				path_abs = path_abs && (case1 || case2);
			}
			fts_abs = fts_abs || path_abs;
		}
		summ = fts_abs;
	}
}

void HeapChunk::show() {
		std::cout << "name: " << get_name() << std::endl;
        std::cout << "pars: \n" << m_pars << std::endl;
        std::cout << "data: \n" << m_data << std::endl;
        std::cout << "space: \n" << m_space << std::endl;
}
