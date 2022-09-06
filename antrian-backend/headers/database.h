#ifndef DATABASE_H
#define DATABASE_H
#include <pqxx/pqxx> 
#include "model.h"
#include <vector>
#include <string>

Antrian get_antrian(pqxx::connection *c, std::string group_id);
std::string next_antrian(pqxx::connection *c, std::string group_id, int queue);
void reset_antrian(pqxx::connection *c, std::string group_id);

#endif