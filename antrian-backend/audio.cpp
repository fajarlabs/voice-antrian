#include <stdlib.h>
#include <string>
#include <cstring>
#include <list>

void satuan_selector(int nomor, std::list<std::string> &voice_list){
      if(nomor == 0) voice_list.push_back("audio/0.wav");
      if(nomor == 1) voice_list.push_back("audio/1.wav");
      if(nomor == 2) voice_list.push_back("audio/2.wav");
      if(nomor == 3) voice_list.push_back("audio/3.wav");
      if(nomor == 4) voice_list.push_back("audio/4.wav");
      if(nomor == 5) voice_list.push_back("audio/5.wav");
      if(nomor == 6) voice_list.push_back("audio/6.wav");
      if(nomor == 7) voice_list.push_back("audio/7.wav");
      if(nomor == 8) voice_list.push_back("audio/8.wav");
      if(nomor == 9) voice_list.push_back("audio/9.wav");
}

void belasan_selector(int nomor, std::list<std::string> &voice_list){
      if(nomor == 11) voice_list.push_back("audio/11.wav");
      if(nomor == 12) voice_list.push_back("audio/12.wav");
      if(nomor == 13) voice_list.push_back("audio/13.wav");
      if(nomor == 14) voice_list.push_back("audio/14.wav");
      if(nomor == 15) voice_list.push_back("audio/15.wav");
      if(nomor == 16) voice_list.push_back("audio/16.wav");
      if(nomor == 17) voice_list.push_back("audio/17.wav");
      if(nomor == 18) voice_list.push_back("audio/18.wav");
      if(nomor == 19) voice_list.push_back("audio/19.wav");
}

void puluhan_selector(int nomor, std::list<std::string> &voice_list){
      if(nomor > 19 && nomor < 30) {
         voice_list.push_back("audio/20.wav");
      } else if(nomor > 29 && nomor < 40) {
         voice_list.push_back("audio/30.wav");
      } else if(nomor > 39 && nomor < 50) {
         voice_list.push_back("audio/40.wav");
      } else if(nomor > 49 && nomor < 60) {
         voice_list.push_back("audio/50.wav");
      } else if(nomor > 59 && nomor < 70) {
         voice_list.push_back("audio/60.wav");
      } else if(nomor > 69 && nomor < 80) {
         voice_list.push_back("audio/70.wav");
      } else if(nomor > 79 && nomor < 90) {
         voice_list.push_back("audio/80.wav");
      } else if(nomor > 89 && nomor < 100) {
         voice_list.push_back("audio/90.wav");
      }   
}

void ratusan_selector(int nomor, std::list<std::string> &voice_list){
      if(nomor > 99 && nomor < 200) {
         voice_list.push_back("audio/100.wav");
      } else if(nomor > 199 && nomor < 300 ){
         voice_list.push_back("audio/200.wav");
      } else if(nomor > 299 && nomor < 400 ){
         voice_list.push_back("audio/300.wav");
      } else if(nomor > 399 && nomor < 500 ){
         voice_list.push_back("audio/400.wav");
      } else if(nomor > 499 && nomor < 600 ){
         voice_list.push_back("audio/500.wav");
      } else if(nomor > 599 && nomor < 700 ){
         voice_list.push_back("audio/600.wav");
      } else if(nomor > 699 && nomor < 800 ){
         voice_list.push_back("audio/700.wav");
      } else if(nomor > 799 && nomor < 900 ){
         voice_list.push_back("audio/800.wav");
      } else if(nomor > 899 && nomor < 1000 ){
         voice_list.push_back("audio/900.wav");
      }
}

std::list<std::string> suara_terbilang(int nomor){
   std::list<std::string> voice_list;
   if(nomor >= 0 && nomor < 10) {
      satuan_selector(nomor, voice_list);
   } else if(nomor > 9 && nomor < 11) {
      voice_list.push_back("audio/10.wav");
   } else if(nomor > 10 && nomor < 20 ) {
      belasan_selector(nomor, voice_list);
   } else if(nomor > 11 && nomor < 100) {
      // 10
      puluhan_selector(nomor, voice_list);
      
      // 1
      std::string konversi = std::to_string(nomor);
      char last_char = konversi.back();
      const char *_last_char = &last_char;
      int satuan = std::atoi(_last_char);
      if(satuan > 0) satuan_selector(satuan,voice_list);
   } else if(nomor > 99 && nomor < 1000) {
      // 100
      ratusan_selector(nomor, voice_list);
      
      // 10
      std::string konversi = std::to_string(nomor);
      std::string mid_char = konversi.substr(1);
      int puluhan = std::atoi(mid_char.c_str());
      if(puluhan > 0) puluhan_selector(puluhan,voice_list);
      
      // 1
      char last_char = konversi.back();
      const char *_last_char = &last_char;
      int satuan = std::atoi(_last_char);
      if(satuan > 0) satuan_selector(satuan,voice_list);
   }
   return voice_list;
}

void pemutar_suara(std::string audio_file){
   system("pkill -15 play");
   std::string _audio_file = "play ";
   std::string _conc_audio = _audio_file + audio_file;
   system(_conc_audio.c_str());
}
