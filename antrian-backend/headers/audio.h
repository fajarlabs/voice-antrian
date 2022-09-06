// someheader.h
#ifndef AUDIO
#define AUDIO
#include <string>
#include <list>

void pemutar_suara(std::string audio_file);
void satuan_selector(int nomor, std::list<std::string> &voice_list);
void belasan_selector(int nomor, std::list<std::string> &voice_list);
std::list<std::string> suara_terbilang(int nomor);

#endif