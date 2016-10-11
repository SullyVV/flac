#define FUSE_USE_VERSION 26
#include "fuse_wrap.hpp"
#include <string>
#include <curl/curl.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
//#include <unordered_map>
//#include <string>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


const char *flacjacket_str = "Flacjacket World!\n";
const char *flacjacket_path = "/flacjacket";
//static const char *rock_str = "rock!\n";
//static const char *rock_path = "/rock";
const char *music_str = "music\n";
const char *music_path = "/music";

//static const char *beat_str = "beat it!";
 const char *beat_path = "beat it";

cJSON* request_json = NULL;
//std::unordered_map<std::string, vector<string> > genreMap;
//std::unordered_map<std::string, vector<string> > artistMap;



size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  MemoryStruct *mem = (MemoryStruct *)userp;
  mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

MemoryStruct getMetadata(MemoryStruct data) {
  CURLcode ret;
  CURL *hnd;
  struct curl_slist *slist1;
  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "x-api-key:UJWr37dxHD4VTmEVZYPpa9U3hNJTr7wgas1Uhvsf");
  //curl set up
  curl_global_init(CURL_GLOBAL_ALL);
  hnd = curl_easy_init();
  if (request_json == NULL) {
    curl_easy_setopt(hnd, CURLOPT_URL, "https://x24cx5vto4.execute-api.us-east-1.amazonaws.com/prod");
  }
  else {
    char temp[256] = "https://x24cx5vto4.execute-api.us-east-1.amazonaws.com/prod?version=";
    char *version = cJSON_GetObjectItem(request_json, "version")->valuestring; 
    strcat(temp, version);
    curl_easy_setopt(hnd, CURLOPT_URL, temp);
  }
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.35.0");
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA,(void*)&data);
  ret = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);
  
  if ((int)ret != 0) {
    printf("Failed to get metadata\n");
  }
  
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;
  curl_global_cleanup();
  return data;
}


int flacjacket_getattr(const char *path, struct stat *stbuf) {
  //printf( "[getattr] Called\n" );
  //printf( "\tAttributes of %s requested\n", path );
	stbuf->st_uid = getuid(); 
  stbuf->st_gid = getgid();
  stbuf->st_atime = time(NULL);
  stbuf->st_mtime = time(NULL);
  
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | S_IRWXU;
		stbuf->st_nlink = 2;
	} 
  else {
    stbuf->st_mode = S_IFREG | S_IRWXU;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1024;
	} 

	return 0;
}

int flacjacket_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi) {
  (void) offset;
  (void) fi;
  MemoryStruct json;
  json.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */ 
  json.size = 0;    /* no data at this point */ 
  json = getMetadata(json);
  
  if (strcmp(json.memory, "\"not modified\"") != 0) {
    cJSON_Delete(request_json);
    request_json = cJSON_Parse(json.memory);
  } 
  
  cJSON* musics = cJSON_GetObjectItem(request_json, "files");
  
  if (strcmp(path, "/") == 0) {
    cJSON* title = NULL;
    //cJSON* genre = NULL;
    //cJSON* artist = NULL;
    int sz = (int)cJSON_GetArraySize(musics);
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    for (int i = 0 ; i < sz; i++) {
      cJSON * music = cJSON_GetArrayItem(musics, i);
      title = cJSON_GetObjectItem(music, "title");
      std::string mp3(title->valuestring);
      mp3 += ".mp3";
      filler(buf, mp3.c_str(), NULL, 0);
      //genre = cJSON_GetObjectItem(music, "genre");
      //artist = cJSON_GetObjectItem(music, "artist");
      //printf("music %d: title: %s,  genre: %s, artist: %s \n", i, title->valuestring, genre->valuestring, artist->valuestring);
    }
  }
  else if (strcmp(path, "rock") == 0) {
    filler(buf, beat_path, NULL, 0);
  }
  
  //clean up
  free(json.memory);
  return 0;
}

int flacjacket_open(const char *path, struct fuse_file_info *fi) {
	if (strcmp(path, flacjacket_path) == 0)
		return 0;
  
  if (strcmp(path, music_path) == 0)
		return 0;
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

int flacjacket_read(const char *path, char *buf, size_t size, off_t offset,
                           struct fuse_file_info *fi) {
	size_t len;
	if(strcmp(path, flacjacket_path) == 0) {
		len = strlen(flacjacket_str);
    if ((size_t)offset < len) {
      if (offset + size > len)
        size = len - offset;
      memcpy(buf, flacjacket_str + offset, size);
    } 
  } else if (strcmp(path, music_path) == 0) {
    len = strlen(music_str);
    if ((size_t)offset < len) {
      if (offset + size > len)
        size = len - offset;
      memcpy(buf, music_str + offset, size);
    }
  }
  else
    size = 0;

	return size;
}
