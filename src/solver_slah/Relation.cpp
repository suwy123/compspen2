#include "solver_slah/Relation.h"

//extern z3::context z3_ctx;

Relation::Relation(z3::context& z3_ctx):sol(z3_ctx){}

void Relation::init(z3::expr_vector adds,z3::expr cons){
	size = adds.size();
	if(size>0){
		std::vector<int> row;
		for(int i = 0; i<adds.size();i++){
			row.push_back(0);
		}
		for(int i = 0; i<adds.size();i++){
			rel_table.push_back(row);
		}
		
		for(int i = 0; i < adds.size(); i++){
			add_table.insert(std::pair<std::string,int>(adds[i].to_string(),i));
			for(int j = i-1; j >= 0; j--){
				if(rel_table[i][j] == 0){
					sol.reset();
    				sol.add(cons && (!(adds[i] == adds[j])));
    				if(sol.check() == z3::unsat){
    					rel_table[i][j] = 1;
    					rel_table[j][i] = 1;
    					tran(i,j,1);
    					continue;
					}
					sol.reset();
    				sol.add(cons && (!(adds[i] < adds[j])));
    				if(sol.check() == z3::unsat){
    					rel_table[i][j] = 2;
    					rel_table[j][i] = 4;
    					tran(i,j,2);
    					continue;
					}
					sol.reset();
    				sol.add(cons && (!(adds[i] > adds[j])));
    				if(sol.check() == z3::unsat){
    					rel_table[i][j] = 4;
    					rel_table[j][i] = 2;
    					tran(i,j,4);
    					continue;
					}
					sol.reset();
    				sol.add(cons && (!(adds[i] <= adds[j])));
    				if(sol.check() == z3::unsat){
    					rel_table[i][j] = 3;
    					rel_table[j][i] = 5;
    					tran(i,j,3);
    					continue;
					}
					sol.reset();
    				sol.add(cons && (!(adds[i] >= adds[j])));
    				if(sol.check() == z3::unsat){
    					rel_table[i][j] = 5;
    					rel_table[j][i] = 3;
    					tran(i,j,5);
    					continue;
					}	
				}
			}
			rel_table[i][i] = 1;		
		}
	}
}

void Relation::init(z3::expr_vector adds){
	size = adds.size();
	if(size>0){
		std::vector<int> row;
		for(int i = 0; i<adds.size();i++){
			row.push_back(0);
		}
		for(int i = 0; i<adds.size();i++){
			rel_table.push_back(row);
		}
		for(int i = 0; i < adds.size(); i++){
			add_table.insert(std::pair<std::string,int>(adds[i].to_string(),i));
		}
	}
}

void Relation::tran(int i,int j,int value){
	if(value == 1){//a == b ? c  --> a ? c
		for(int k = j-1; k >=0; k--){
			rel_table[i][k] = rel_table[j][k];
			rel_table[k][i] = rel_table[k][j];
		}
	}else if(value == 2){//a < b ? c  --> a < c where ? is == or < or <=
		for(int k = j-1; k >= 0; k--){
			if(rel_table[j][k] == 1 ||rel_table[j][k] == 2 || rel_table[j][k] == 3){
				rel_table[i][k] = 2;
				rel_table[k][i] = 4;
			}
		}
	}else if(value == 3){//a <= b ? c  --> a <= c where ? is == or <=		a < c where ? is <
		for(int k = j-1; k >=0; k--){
			if(rel_table[j][k] == 1 || rel_table[j][k] == 3){
				rel_table[i][k] = 3;
				rel_table[k][i] = 5;
			}else if(rel_table[j][k] == 2){
				rel_table[i][k] = 2;
				rel_table[k][i] = 4;
			} 
		}
	}else if(value == 4){//a > b ? c  --> a > c where ? is == or > or >=
		for(int k = j-1; k >=0; k--){
			if(rel_table[j][k] == 1 ||rel_table[j][k] == 4 || rel_table[j][k] == 5){
				rel_table[i][k] = 4;
				rel_table[k][i] = 2;
			}
		}
	}else if(value == 5){//a >= b ? c  --> a >= c where ? is == or >=		a > c where ? is >
		for(int k = j-1; k >=0; k--){
			if(rel_table[j][k] == 1 || rel_table[j][k] == 5){
				rel_table[i][k] = 5;
				rel_table[k][i] = 3;
			}else if(rel_table[j][k] == 4){
				rel_table[i][k] = 4;
				rel_table[k][i] = 2;
			} 
		}
	}
}

int Relation::get_relation(z3::expr add1,z3::expr add2){
	if(add_table.find(add1.to_string()) != add_table.end() && add_table.find(add2.to_string()) != add_table.end()){
		int i = add_table[add1.to_string()];int j = add_table[add2.to_string()];
		return rel_table[i][j];
	}
	//std::cout<<"address is not in rel_table"<<std::endl;
	return -1;
}

void Relation::set_relation(z3::expr add1,z3::expr add2,int rel){
	if(add_table.find(add1.to_string()) != add_table.end() && add_table.find(add2.to_string()) != add_table.end()){
		int i = add_table[add1.to_string()];int j = add_table[add2.to_string()];
		switch(rel){
			case 1:
				rel_table[i][j] = 1;rel_table[j][i] = 1;return;
			case 2:
				rel_table[i][j] = 2;rel_table[j][i] = 4;return;
			case 3:
				rel_table[i][j] = 3;rel_table[j][i] = 5;return;
		}
	}
	std::cout<<"address is not in rel_table"<<std::endl;
}

int Relation::get_degree(z3::expr add){//numof > or >=
	if(add_table.find(add.to_string())!=add_table.end()){
		int num = 0;
		int i = add_table[add.to_string()];
		for(int j = 0;j<size;j++){
			if(rel_table[i][j] == 4 || rel_table[i][j] == 5) num++;
		}
		return num;
	}else{
		std::cout<<"address is not in rel_table when get_degree"<<std::endl;
		return -1;
	}
	return 0;
}

void Relation::print(){
	std::cout<<"add_table:"<<std::endl;
	for (std::map<std::string,int>::iterator iter = add_table.begin();iter != add_table.end();++iter) {
    	std::cout <<iter->first<<"----------------"<<iter->second<< std::endl;
	}
	std::cout<<"rel_table:"<<std::endl;
	for(int i = 0;i<size;i++){
		for(int j = 0;j<size;j++){
			std::cout<<rel_table[i][j]<<" ";
		}
		std::cout<<std::endl;
	}
}


