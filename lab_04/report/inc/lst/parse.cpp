void extract_urls(const std::string& page_content, std::set<std::string>& urls, const std::string& mask) {
    std::regex link_regex(mask);
    std::sregex_iterator links_begin = std::sregex_iterator(page_content.begin(), page_content.end(), link_regex);
    std::sregex_iterator links_end = std::sregex_iterator();
    std::string new_link;
    for (std::sregex_iterator i = links_begin; i != links_end; ++i) {
        new_link = ROOT_URL + i->str();
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