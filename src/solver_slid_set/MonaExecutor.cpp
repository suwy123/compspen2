#include "solver_slid_set/MonaExecutor.h"

#include <fstream>

/**
 * run mona, and get a model from output
 * @return true, if sat
 */
bool MonaExecutor::execute(std::map<std::string, std::string>& model) {
    std::string cmd = "mona ";
    cmd.append(m_args);
    cmd.append(" ").append(m_name);
    FILE  *fp=popen(cmd.c_str(), "r");
    std::string output;
    char c;
    while((c=fgetc(fp)) != EOF) {
        output.append(1, c);
    }
    pclose(fp);
    return parseModel(output, model);
}

/**
 * run mona
 */
void MonaExecutor::execute(std::string filename) {
    std::string cmd = "mona ";
    cmd.append(m_args);
    cmd.append(" ").append(m_name);
    FILE  *fp=popen(cmd.c_str(), "r");
    std::string output;
    char c;
    while((c=fgetc(fp)) != EOF) {
        output.append(1, c);
    }
    pclose(fp);

    std::ofstream out(filename);
    out << output;
    out.close();
}


bool MonaExecutor::parseModel(std::string &str,  std::map<std::string, std::string>& model) {
    int index = str.find("A satisfying example");
    if (index == -1) return false;
    index = str.find('\n', index)+1;
    int len = str.find(' ', index) - index;
    std::string key = str.substr(index, len);
    index = str.find(key.c_str(), index+len);
    std::string value;

    while(index < str.length()) {
        len = str.find(' ', index) - index;
        key = str.substr(index, len);
        index = str.find(' ', index+len+1) + 1;
        len = str.find('\n', index) - index;
        value = str.substr(index, len);
        index = index + len + 1;

        // cout << "key: " << key << " = " << value << endl;
        if(key.find("_minus") != -1) {
            getMinusVal(value);
        }
        model[key] = value;
    }
    return true;
}

void MonaExecutor::getMinusVal(std::string &value) {
    if (value.find('{') == -1) {
        if (value != "0") value.insert(0, 1, '-');
        return;
    }
    int index = value.find('{') + 1;
    int end = value.find('}');
    if (index < end) {
        reverse(value.begin()+1, value.end()-1);
    }

    while(index!=0 && index < end) {
        value.insert(index, 1, '-');
        end++;
        index = value.find(',', index)+1;
    }
}
