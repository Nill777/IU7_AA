#ifndef PRE_UTILS_HPP
#define PRE_UTILS_HPP

#include <string>
#include <set>
#include <queue>
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>
#include <curl/curl.h>

#define URL "https://www.gotovim.ru/recepts/"
#define ROOT_URL "https://www.gotovim.ru"

#define ERROR_INIT_CURL 1
#define ERROR_CURL 2
#define ERROR_GET_CATEGORY 3
#define ERROR_GET_RECEPTS 4
#define ERROR_OPEN_FILE 5

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);
CURLcode perform_curl_request(CURL* curl, const std::string& url, std::ostringstream& readBuffer);
void extract_urls(const std::string& page_content, std::set<std::string>& urls, const std::string& mask);
int get_category_urls(std::set<std::string>& category_urls, const std::string& url);
int get_recepts_urls(std::set<std::string>& recept_urls, const std::string& url);
int pre_proc(std::queue<std::string>& full_url_queue);
int write_queue_to_file(const std::queue<std::string>& url_queue, const std::string& filename);
int read_queue_from_file(std::queue<std::string>& url_queue, const std::string& filename);

#endif
