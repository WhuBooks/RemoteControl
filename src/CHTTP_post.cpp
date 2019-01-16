
#include "CHTTP_post.h"

#include <curl/curl.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>


using namespace std;
#define BASE64_ENCODE_MAX_FILESIZE (1024 * 1024)

static string m_callback_result;
// 共用的 callback
//std::string UtfToGbk(const char* utf8)
//{
//	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
//	wchar_t* wstr = new wchar_t[len + 1];
//	memset(wstr, 0, len + 1);
//	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
//	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
//	char* str = new char[len + 1];
//	memset(str, 0, len + 1);
//	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
//	if (wstr) delete[] wstr;
//	return str;
//}
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    // 获取到的body存放在ptr中，先将其转换为string格式
	//auto a = UtfToGbk((char*)ptr);
	m_callback_result =  std::string((char *)ptr, size * nmemb);
    return size * nmemb;
}


bool CHTTP_post::xfuture_http_post_json(string url, string post, string &result, string &errmsg)
{
    m_callback_result = "";

    CURL *curl = NULL;
    CURLcode result_code;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");//
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
        struct timeval t1,t2;
        gettimeofday(&t1,NULL);
        result_code = curl_easy_perform(curl);
        gettimeofday(&t2,NULL);
        int64_t ts = (int64_t)(t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_usec-t1.tv_usec)/1000;
        //cout<<ts<<endl;
//        CURLM *curl_multi_init(void);                                            //初始化
//        CURLMcode curl_multi_add_handle(CURLM *multi_handle,
//                                        CURL *curl_handle);                                            //添加一个传输请求
//        CURLMcode curl_multi_perform(CURLM *multi_handle, int *running_handles); //执行传输
//        CURLMcode curl_multi_cleanup(CURLM *multi_handle);                       //清理
        if (result_code != CURLE_OK) {
            errmsg = string(curl_easy_strerror(result_code));
            cout<<"POST ERROR:"<<errmsg<<endl;
			curl_easy_cleanup(curl);
            return false;
        } else {
            result = m_callback_result;
			curl_easy_cleanup(curl);
            return true;
        }
    } else {
        errmsg = string("curl_easy_init() failed.");
        return false;
    }
}

bool CHTTP_post::xfuture_http_post_pgm(string url, string pgm, string &result, string &errmsg)
{
    m_callback_result = "";

    CURL *curl = NULL;
    CURLcode result_code;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type:image/x-portable-graymap");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pgm.c_str());
        result_code = curl_easy_perform(curl);
        if (result_code != CURLE_OK) {
            errmsg = string(curl_easy_strerror(result_code));
            return false;
        } else {
            result = m_callback_result;
            return true;
        }
    } else {
        errmsg = string("curl_easy_init() failed.");
        return false;
    }
}

bool CHTTP_post::xfuture_http_post_raw(string url, string raw, string &result, string &errmsg)
{
    m_callback_result = "";

    CURL *curl = NULL;
    CURLcode result_code;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-type:text/plain");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, raw.c_str());
        result_code = curl_easy_perform(curl);
        if (result_code != CURLE_OK) {
            errmsg = string(curl_easy_strerror(result_code));
            return false;
        } else {
            result = m_callback_result;
            return true;
        }
    } else {
        errmsg = string("curl_easy_init() failed.");
        return false;
    }
}


int CHTTP_post::xfuture_http_base64_encode_file(string file, string &out)
{
    unsigned char* buf = (unsigned char *)malloc(BASE64_ENCODE_MAX_FILESIZE);
    if (buf == NULL) {
        fprintf(stderr, "base64 encode file: cannot malloc buffer\n");
        return -1;
    }

    FILE *fp = fopen(file.c_str(), "rb");
    if (fp == NULL) {
        fprintf(stderr, "base64 encode file: cannot open file %s\n", file.c_str());
        return -1;
    }

    int res = 0;

    int rd = fread(buf, 1, BASE64_ENCODE_MAX_FILESIZE, fp);
    if (rd > 0) {
        //char* p = g_base64_encode(buf, rd);
        //out = string(p);
        //res = out.length();
        //g_free(p);
    } else {
        res = 0;
        out = "";
    }
    fclose(fp);
    return res;
}

