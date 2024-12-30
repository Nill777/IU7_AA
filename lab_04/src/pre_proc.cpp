#include "pre_proc.hpp"

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::ostringstream*)userp)->write((char*)contents, size * nmemb);
    return size * nmemb;
}

CURLcode perform_curl_request(CURL* curl, const std::string& url, std::ostringstream& readBuffer) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    return curl_easy_perform(curl);
}

void extract_urls(const std::string& page_content, std::set<std::string>& urls, const std::string& mask) {
    std::regex link_regex(mask);
    std::sregex_iterator links_begin = std::sregex_iterator(page_content.begin(), page_content.end(), link_regex);
    std::sregex_iterator links_end = std::sregex_iterator();
    std::string new_link;
    for (std::sregex_iterator i = links_begin; i != links_end; ++i) {
        new_link = ROOT_URL + i->str();
        // std::cout << new_link << std::endl;
        urls.insert(new_link);
    }
}

int get_category_urls(std::set<std::string>& category_urls, const std::string& url) {
    std::ostringstream readBuffer;
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Ошибка инициализации CURL" << std::endl;
        return ERROR_INIT_CURL;
    }
    CURLcode res = perform_curl_request(curl, url, readBuffer);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK)
        extract_urls(readBuffer.str(), category_urls, R"(\/recepts\/[a-z]+\/[a-z]+\/)");
    else {
        std::cerr << "Ошибка CURL: " << curl_easy_strerror(res) << std::endl;
        return ERROR_CURL;
    }

    return EXIT_SUCCESS;
}

int get_recepts_urls(std::set<std::string>& recept_urls, const std::string& url) {
    std::ostringstream readBuffer;
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Ошибка инициализации CURL" << std::endl;
        return ERROR_INIT_CURL;
    }
    CURLcode res = perform_curl_request(curl, url, readBuffer);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK)
        extract_urls(readBuffer.str(), recept_urls, R"(\/sbs\/[a-z]+\.shtml)");
    else {
        std::cerr << "Ошибка CURL: " << curl_easy_strerror(res) << std::endl;
        return ERROR_CURL;
    }

    return EXIT_SUCCESS;
}

int pre_proc(std::queue<std::string>& full_url_queue) {
    std::set<std::string> category_urls;
    std::set<std::string> recept_urls;
    std::queue<std::string> tmp_url_queue;
    std::string url;

    if (get_category_urls(category_urls, URL))
        return ERROR_GET_CATEGORY;
    for (const std::string& link : category_urls)
        tmp_url_queue.push(link);

    while (!tmp_url_queue.empty()) {
        url = tmp_url_queue.front();
        std::cout << url << std::endl;
        tmp_url_queue.pop();
        if (get_recepts_urls(recept_urls, url))
            return ERROR_GET_RECEPTS;
    }

    for (const std::string& link : recept_urls)
        full_url_queue.push(link);

    return EXIT_SUCCESS;
}

int write_queue_to_file(const std::queue<std::string>& url_queue, const std::string& filename) {
    std::ofstream outfile(filename);

    if (!outfile.is_open()) {
        std::cerr << "Ошибка открытия файла для записи: " << filename << std::endl;
        return ERROR_OPEN_FILE;
    }
    std::queue<std::string> temp_queue = url_queue;
    while (!temp_queue.empty()) {
        outfile << temp_queue.front() << std::endl;
        temp_queue.pop();
    }

    outfile.close();
    return EXIT_SUCCESS;
}

int read_queue_from_file(std::queue<std::string>& url_queue, const std::string& filename) {
    std::ifstream infile(filename);

    if (!infile.is_open()) {
        std::cerr << "Ошибка открытия файла для чтения: " << filename << std::endl;
        return ERROR_OPEN_FILE;
    }
    std::string line;
    while (std::getline(infile, line)) {
        url_queue.push(line);
    }

    infile.close();
    return EXIT_SUCCESS;
}