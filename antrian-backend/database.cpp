#include <pqxx/pqxx> 
#include <vector>
#include <string>
#include "headers/model.h"
#include "headers/ini.h"
#include <iostream>

Antrian get_antrian(pqxx::connection *C, std::string group_id){
    /* SQL */
    std::string sql = "SELECT group_id,queue,counter FROM nomor_antrian WHERE group_id = '"+group_id+"'";
    /* Create a non-transactional object. */
    pqxx::nontransaction N(*C);

    /* Execute SQL query */
    pqxx::result R( N.exec(sql));
    
    Antrian response;
    /* List down all the records */
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
        response.group_id = c[0].as<std::string>();
        response.queue = c[1].as<int>();
        response.counter = c[2].as<int>();
    }
    return response;
}

std::string next_antrian(pqxx::connection *C, std::string group_id, int queue){
    /* SQL */
    pqxx::work W(*C);

    /* update data */
    std::string s_queue = std::to_string(queue);
    std::string sql = "UPDATE nomor_antrian SET queue = '"+s_queue+"' WHERE group_id = '"+group_id+"'" ;

    W.exec(sql);
    W.commit();

    return group_id;
}

void reset_antrian(pqxx::connection *C, std::string group_id){
    /* update data */
    std::string sql = "UPDATE nomor_antrian SET queue = '0' WHERE group_id = '"+group_id+"'" ;
    /* SQL */
    pqxx::work W(*C);
    W.exec(sql);
    W.commit();
}