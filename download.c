/********* Sample code generated by the curl command line tool **********
 * All curl_easy_setopt() options are documented at:
 * http://curl.haxx.se/libcurl/c/curl_easy_setopt.html
 ************************************************************************/
#include <unistd.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
int WriteMusicCallback(void *contents, int size, int nmemb, void *userp) {
    printf("in write call back\n");
    FILE* fd = fopen("./buf.mp3", "w");
    fprintf(fd, "%s", (char*)contents);
    fclose(fd);
    return 0;
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
int main(int argc, char *argv[])
{
    CURLcode ret;
    CURL *hnd;
    FILE* fp;
    struct curl_slist *slist1;
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "api-key: s00per_seekrit");
    char outfilename[100] = "./buf.mp3";
    hnd = curl_easy_init();
    if (hnd) {
        fp = fopen(outfilename,"wb");
        curl_easy_setopt(hnd, CURLOPT_URL, "https://www.exoatmospherics.com/transcoder");
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "Four Tet - Randoms - 01 Moma.flac");
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)33);
        curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.35.0");
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
        curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
        //curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMusicCallback);
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, fp);
        ret = curl_easy_perform(hnd);
        /***************clean up********************/
        curl_easy_cleanup(hnd);
        hnd = NULL;
        curl_slist_free_all(slist1);
        slist1 = NULL;
        fclose(fp);
        /*******************************************/
        return 0;
    } else {
        return -1;
    }
}
/**** End of sample code ****/
