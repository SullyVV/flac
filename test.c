#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
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


void parse_object(cJSON *root) {
  cJSON* title = NULL;
  cJSON* genre = NULL;
  
  cJSON* items = cJSON_CreateArray();
  items = cJSON_GetObjectItem(root, "files");
  //char * rendered = cJSON_Print(items);                                                                                                                                
  //printf("%s\n", rendered);  
/*
  cJSON * files = NULL;
  files = cJSON_GetObjectItem(root,"files");
  
  if(files != NULL) {
    printf("not null");
    cJSON *file = files->child;
    if (file) {
      printf("hello\n");
    }
    while(file != NULL) {
      // get and print key
      //char * rendered = cJSON_Print(root);
      //printf("%s\n", rendered);
      file = file->next;
      
    }
  }  
  */
  int sz = (int)cJSON_GetArraySize(items);
  printf("sz:%d", sz);
  int i;
  for (i = 0 ; i < cJSON_GetArraySize(items) ; i++)
  {
    cJSON * subitem = cJSON_GetArrayItem(items, i);
    title = cJSON_GetObjectItem(subitem, "title");
    genre = cJSON_GetObjectItem(subitem, "genre");
    printf("%s\n", title->valuestring);
    printf("%s\n", genre->valuestring);
    
  }
  /*
  while (files) {
    printf("hello");
    char * title = cJSON_GetObjectItem(files, "title")->string;
    char * genre = cJSON_GetObjectItem(files, "genre")->string;
    printf("title:%s , genre: %s\n", title, genre);
    files = files->next;
  }
  */
}

int main(int argc, char *argv[]) {
  CURLcode ret;
  CURL *hnd;
  struct curl_slist *slist1;
  struct MemoryStruct json;
  json.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  json.size = 0;    /* no data at this point */ 
  
  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "x-api-key:UJWr37dxHD4VTmEVZYPpa9U3hNJTr7wgas1Uhvsf");
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, "https://x24cx5vto4.execute-api.us-east-1.amazonaws.com/prod");
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.35.0");
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&json);
  ret = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);
  
  //printf("the recived json list is :\n %s", json.memory); 
  
  cJSON* request_json = NULL;
  request_json = cJSON_Parse(json.memory);
  parse_object(request_json);


  free(json.memory);
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;
  
  curl_global_cleanup();

  return (int)ret;
}
