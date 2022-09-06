#ifndef MODEL_H
#define MODEL_H
#include <string>
#include <iostream>

struct Antrian {
  std::string group_id;
  int queue;
  int counter;
  std::string counter_id;
  std::string queue_id;
};

extern Antrian antrian;
#endif