#ifndef MONA_EXECUTOR_H
#define MONA_EXECUTOR_H
#include <iostream>
#include <map>
#include <algorithm>

class MonaExecutor {
private:
    std::string m_name;
    std::string m_args;

private:
    void getMinusVal(std::string& value);
public:
    MonaExecutor(){}
    void setName(std::string name) {m_name = name;}
    void setArgs(std::string args) {m_args = args;}
    bool execute(std::map<std::string, std::string>& model);
    void execute(std::string filename);
    bool parseModel(std::string& output,  std::map<std::string, std::string>& model);

};


#endif /* MONA_EXECUTOR_H */
