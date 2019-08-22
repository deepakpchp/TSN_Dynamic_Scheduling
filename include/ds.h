#include <iostream>
#define SUCCESS 0
#define FAILURE 1

#define INFO(X) std::cout<<"Info: "<<X<<std::endl
#define LOG(X) std::cout<<"Log! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl
#define ERROR(X) std::cerr<<"Error!! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl
#define FATAL(X) std::cerr<<"Fatal Error!!! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl<<std::endl; exit(0)
