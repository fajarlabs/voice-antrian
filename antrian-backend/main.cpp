#include "headers/crow_all.h"
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <typeinfo>
#include <thread>
#include <list>
#include <iostream>
#include <pqxx/pqxx> 
#include <vector>
#include "headers/audio.h"
#include "headers/model.h"
#include "headers/database.h"
#include "headers/ini.h"
#include <unordered_set>
#include <mutex>
#include <unistd.h>

/*
GET /
GET /pendaftaran/execution/next
GET /pendaftaran/execution/rcal
GET /pendaftaran/execution/prev
GET /pendaftaran/execution/*123
GET /reset
GET /terbilang/?queue=1&counter=1
*/

std::string _GROUP_ID;
std::string _COUNTER_ID;
std::string _QUEUE_ID;
Antrian _CURRENT_DATA;
std::string _CONNECTION_STRING;
pqxx::connection* _CONN;
bool _IS_PLAY = true;
int _IS_REQUEST = 0;


void putar_suara(int queue, int counter = 0){

    if (_IS_PLAY){
        // notification
        pemutar_suara("audio/simple_notification.wav");
    }

    if(_IS_PLAY){
        // suara awal antrian
        pemutar_suara("audio/antrian.wav");
    }
    std::list<std::string> list_terbilang = suara_terbilang(queue);

    if(_IS_PLAY && _QUEUE_ID != ""){
        pemutar_suara("audio/"+_QUEUE_ID+".wav");
    }

    // list suara terbilang
    for (auto word: list_terbilang) {
        std::cout << word << std::endl;

        if(_IS_PLAY) {
            pemutar_suara(word);
        }
    }

    if(counter > 0) {
        // suara counter / loket
        if(_IS_PLAY){
            pemutar_suara("audio/counter.wav");
        }
        std::list<std::string> list_loket = suara_terbilang(counter);

        if(_IS_PLAY && _COUNTER_ID != ""){
            pemutar_suara("audio/"+_COUNTER_ID+".wav");
        }

        // suara terbilang loket
        for (auto word: list_loket) {
            std::cout << word << std::endl;
            if(_IS_PLAY){
                pemutar_suara(word);
            }
        }
    }
}

void reconnect(){
    try {
        std::cout << "Reconnect database" << std::endl;
        if(_CONN != nullptr){
            std::cout << "Delete pointer" << std::endl;
            delete _CONN;
        }
        _CONN = new pqxx::connection(_CONNECTION_STRING);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        _CONN = nullptr;
    }
}

void connect() {
    try {
        _CONN = new pqxx::connection(_CONNECTION_STRING);
        if (_CONN) {
            std::cout << "Opened database successfully: " << _CONN->dbname() << std::endl;
        } else {
            std::cout << "Can't open database" << std::endl;  
            reconnect();
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

int _LOOP = true;
std::vector<Antrian> _LIST_VOICE;

void loop_audio(){
    while (_LOOP) {
        //unsigned int microsecond = 1000000;
        if(_LIST_VOICE.size() > 0){
            for(Antrian i : _LIST_VOICE){
                std::cout << "Antrian :" << i.queue << std::endl;
                putar_suara(i.queue, i.counter);
                _LIST_VOICE.erase(_LIST_VOICE.begin());
            }
        }
        // reset flag play
        _IS_PLAY = true;
        //usleep(1 * microsecond);//sleeps for 3 second
    }
}

int main() {

    std::thread th1(loop_audio);
    
    // first, create a file instance
    mINI::INIFile file("setting.ini");
    // next, create a structure that will hold data
    mINI::INIStructure ini;
    // now we can read the file
    file.read(ini);
    // database read value
    _CONNECTION_STRING = "dbname="+ini["database"]["dbname"]+" user="+ini["database"]["user"]+" password="+ini["database"]["password"]+" hostaddr="+ini["database"]["hostaddr"]+" port="+ini["database"]["port"];
    // app read valuepemutar_suara("audio/simple_notification.wav");
    _GROUP_ID = ini["voice"]["group_id"];
    _COUNTER_ID = ini["voice"]["counter_id"];
    _QUEUE_ID = ini["voice"]["queue_id"];
    _CURRENT_DATA.queue_id = _QUEUE_ID;
    _CURRENT_DATA.counter_id = _COUNTER_ID;
    // connect database
    connect();

    crow::SimpleApp app;
    std::cout << "Web restapi started" << std::endl;

    std::mutex mtx;
    std::unordered_set<crow::websocket::connection*> users;

    CROW_ROUTE(app, "/ws").websocket()
        .onopen([&](crow::websocket::connection& conn){
                CROW_LOG_INFO << "new websocket connection";
                std::lock_guard<std::mutex> _(mtx);
                users.insert(&conn);
                })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                CROW_LOG_INFO << "websocket connection closed: " << reason;
                std::lock_guard<std::mutex> _(mtx);
                users.erase(&conn);
                })
        .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary){
                std::lock_guard<std::mutex> _(mtx);
                for(auto u:users)
                    if (is_binary)
                        u->send_binary(data);
                    else {
                        if(data == "data/current"){
                            std::string counter = std::to_string(_CURRENT_DATA.counter);
                            std::string queue = std::to_string(_CURRENT_DATA.queue);
                            std::string is_request = std::to_string(_IS_REQUEST);

                            // to uppercase string
                            std::string format_data = "c:"+counter+".q:"+queue+".gid:"+_GROUP_ID+".cid:"+_COUNTER_ID+".qid:"+_QUEUE_ID+".ir:"+is_request;
                            u->send_text(format_data);
                            _IS_REQUEST = 0;                           
                        }
                    }
                });

    CROW_ROUTE(app, "/")([](){
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
	
        auto page = crow::mustache::load("index.html");
        return page.render(x);      
    });

    CROW_ROUTE(app, "/pendaftaran/execution/rcal")([](){
        crow::json::wvalue response_json;

        // stop all play
        _IS_PLAY = false;
        _IS_REQUEST = 1;

        try {
            if (_CONN) {
                Antrian result = get_antrian(_CONN, _GROUP_ID);
   
                if (result.queue > 0) { 
                    response_json["group_id"] = result.group_id;
                    response_json["queue"] = result.queue;
                    response_json["counter"] = result.counter;
                    response_json["code"] =  200;

                    _CURRENT_DATA.group_id = result.group_id;
                    _CURRENT_DATA.queue = result.queue;
                    _CURRENT_DATA.counter = result.counter;

                    _LIST_VOICE.push_back(_CURRENT_DATA);
                    //putar_suara(result.queue, result.counter);
                } else {
                    response_json["code"] =  200;
                    response_json["description"] = "Sorry, your QUEUE is ZERO";
                }
            } else {
                std::cout << "Can't open database" << std::endl;
                response_json["code"] =  200;
                response_json["description"] = "Can't open database";  
                reconnect();  
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            response_json["code"] =  200;
            response_json["description"] = "Can't open database"; 
            reconnect();
        }
        return crow::response(std::move(response_json));
    });

    CROW_ROUTE(app, "/pendaftaran/execution/prev")([](){
        crow::json::wvalue response_json;

        // stop all play
        _IS_PLAY = false;
        _IS_REQUEST = 1;

        try {
            if (_CONN) {
                Antrian result = get_antrian(_CONN, _GROUP_ID);

                if (result.queue > 1) {
                    int queue = result.queue < 1 ? 0 : result.queue - 1;
                    response_json["group_id"] = result.group_id;
                    response_json["queue"] = queue;
                    response_json["counter"] = result.counter;
                    response_json["code"] =  200;

                    _CURRENT_DATA.group_id = result.group_id;
                    _CURRENT_DATA.queue = queue;
                    _CURRENT_DATA.counter = result.counter;

                    _LIST_VOICE.push_back(_CURRENT_DATA);
                } else {
                    response_json["code"] =  200;
                    response_json["description"] =  "Sorry, your QUEUE data must have more than 1";
                }
            } else {
                std::cout << "Can't open database" << std::endl;
                response_json["code"] =  200;
                response_json["description"] = "Can't open database";    
                reconnect();
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            response_json["code"] =  200;
            response_json["description"] = "Can't open database";   
            reconnect();
        }
        return crow::response(std::move(response_json));
    });

    CROW_ROUTE(app, "/pendaftaran/execution/next")([](){
        crow::json::wvalue response_json;

        // stop all play
        _IS_PLAY = false;
        _IS_REQUEST = 1;

        try {
            if (_CONN) {
                Antrian result_first = get_antrian(_CONN, _GROUP_ID);
                int queue = result_first.queue + 1;
                next_antrian(_CONN, _GROUP_ID, queue );
                Antrian result = get_antrian(_CONN, _GROUP_ID);
                response_json["code"] = 200;
                response_json["group_id"] = result.group_id;
                response_json["queue"] = result.queue;
                response_json["counter"] = result.counter;

                _CURRENT_DATA.group_id = result.group_id;
                _CURRENT_DATA.queue = queue;
                _CURRENT_DATA.counter = result.counter;

                _LIST_VOICE.push_back(_CURRENT_DATA);
            } else {
                std::cout << "Can't open database" << std::endl;
                response_json["code"] =  200;
                response_json["description"] = "Can't open database";   
                reconnect();
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            response_json["code"] =  200;
            response_json["description"] = "Can't open database"; 
            reconnect();
        }
        return crow::response(std::move(response_json));
    });

    CROW_ROUTE(app, "/current")([](){
        crow::json::wvalue response_json;

        // stop all play
        _IS_PLAY = false;
        _IS_REQUEST = 1;

        try {
            if (_CONN) {
                Antrian result = get_antrian(_CONN, _GROUP_ID);
                response_json["code"] =  200;
                response_json["group_id"] = result.group_id;
                response_json["queue"] = result.queue;
                response_json["counter"] = result.counter;

                _CURRENT_DATA.group_id = result.group_id;
                _CURRENT_DATA.queue = result.queue;
                _CURRENT_DATA.counter = result.counter;

                _LIST_VOICE.push_back(_CURRENT_DATA);
            } else {
                std::cout << "Can't open database" << std::endl;
                response_json["code"] =  200;
                response_json["description"] = "Can't open database";   
                reconnect(); 
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            response_json["code"] =  200;
            response_json["description"] = "Can't open database";   
            reconnect(); 
        }
        return crow::response(std::move(response_json));
    });

    CROW_ROUTE(app, "/pendaftaran/execution/<string>")([](std::string id){
        crow::json::wvalue response_json;

        // stop all play
        _IS_PLAY = false;
        _IS_REQUEST = 1;

        response_json["code"] =  200;
        if (id.find("*") != std::string::npos) {
            std::string ch_number = id.substr(1);
            int conv_number = std::atoi(ch_number.c_str());
            response_json["queue"] = conv_number;

            _CURRENT_DATA.queue = conv_number;

            _LIST_VOICE.push_back(_CURRENT_DATA);
        } else {
            response_json["description"] = "Error formatting data";
        }
        return crow::response(std::move(response_json));
    });

    CROW_ROUTE(app, "/reset")([](){
        crow::json::wvalue response_json;
        try {
            if (_CONN) {
                reset_antrian(_CONN, _GROUP_ID);
                response_json["code"] =  200;
                response_json["description"] = "Reset successfully";

                _CURRENT_DATA.counter = 0;
                _CURRENT_DATA.queue = 0;
    
            } else {
                std::cout << "Can't open database" << std::endl;
                response_json["code"] =  200;
                response_json["description"] = "Can't open database";    
                reconnect();               
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            reconnect();
        }
        return crow::response(std::move(response_json));
    });

    CROW_ROUTE(app, "/voice")([](const crow::request& req){
        int queue = atoi(req.url_params.get("queue"));
        int counter = atoi(req.url_params.get("counter"));

        _CURRENT_DATA.queue = queue;
        _CURRENT_DATA.counter = counter;

        _LIST_VOICE.push_back(_CURRENT_DATA);

        crow::json::wvalue response_json;
        response_json["status"] = "ok";
        response_json["queue"] = queue;
        response_json["counter"] = counter;

        return crow::response(std::move(response_json));
    });

    std::string s_port = ini["web"]["port"];
    int port = std::atoi(s_port.c_str());
    app.bindaddr("127.0.0.1").port(port).multithreaded().run();
}
