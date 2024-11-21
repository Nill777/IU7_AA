// gotovim.ru

#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <queue>

#include <regex>
#include <condition_variable>
#include <sys/time.h>
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <gnuplot-iostream.h>

#define ERROR_INIT_CURL 1
#define ERROR_OPEN_FILE 2
#define ERROR_CURL 3
#define INVALID_PAGE 4
#define ERROR_DOWNLOAD_PAGE -1
#define ERROR_GET_CATEGORY -2
#define ERROR_GET_RECEPTS 5
#define ERROR_PRE_PROC 6

#define NUM_PAGES 20
#define DATA_DIR "./data/"
#define HISTOGRAM_FILE "histogram.svg"
#define URL "https://www.gotovim.ru/recepts/"
#define ROOT_URL "https://www.gotovim.ru"
#define MENU "\
\nКомманды: \
\n1 - Задать максимальное количество страниц\
\n2 - Выгрузка в последовательном режиме\
\n3 - Выгрузка в параллельном режиме\
\n4 - Построить график производительности от количества дополнительных потоков\
\n0 - Завершить работу программы\n\
>"

int downloaded_pages = 0;
std::set<std::string> visited_urls;
std::mutex mtx;
std::queue<std::string> url_queue;
std::condition_variable cv;
bool done = false;

long long time_now() {    
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    return current_time.tv_sec * 1000.0 + current_time.tv_usec / 1000.0;
}

void clean_stdin(void) {
    char c;
    while ((c = getchar()) != '\n');
}

void print_time_prosess(const int thread, const long long time) {
    std::cout << "Threads: " << thread <<
        ", time: " << time << " milliseconds" << std::endl;
}
void get_max_pages(int &max_pages) {
    if (max_pages != -1) {
        std::cout << "Текущее число страниц: " << max_pages << ", изменить [N/y]? ";
        char action;
        std::cin >> action;
        if (action == 'y' || action == 'Y') {
            while (true) {
                std::cout << "Введите кол-во выгружаемых страниц: ";
                std::cin >> max_pages;
                if (max_pages > 0) {
                    break;
                } else {
                    std::cout << "Некорректное число страниц (должно быть > 0)" << std::endl;
                }
            }
        }
        return;
    }
    while (true) {
        std::cout << "Введите кол-во выгружаемых страниц: ";
        std::cin >> max_pages;
        if (max_pages > 0) {
            break;
        } else {
            std::cout << "Некорректное число страниц (должно быть > 0)" << std::endl;
        }
    }
}


void get_num_threads(int &threads) {
    int k = std::thread::hardware_concurrency();
    int max_threads = 4 * k;
    while ((fscanf(stdin, "Введите кол-во потоков: %d", threads) != 1 || threads < max_threads)) {
        printf("Некорректное число потоков возможно (0-%d)", max_threads);
        clean_stdin();
    }
}

bool directory_exists(const std::string& dir) {
    struct stat info;
    return (stat(dir.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

void create_directory(const std::string& dir) {
    if (!directory_exists(dir)) {
        mkdir(dir.c_str(), 0777);
    }
}

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::ostringstream*)userp)->write((char*)contents, size * nmemb);
    return size * nmemb;
}

bool parse_page(const std::string& page_content, const std::string& url) {
    // Извлечение новых ссылок
    // <a href="/recepts/salad/meat/">Салаты мясные</a>
    // <a href="https://www.gotovim.ru/sbs/glintweinkognak.shtml">Глинтвейн из красного вина с коньяком</a>
    std::regex link_regex(R"(https:\/\/www\.gotovim\.ru\/sbs\/[a-z]+\.shtml)");
    std::sregex_iterator links_begin = std::sregex_iterator(page_content.begin(), page_content.end(), link_regex);
    std::sregex_iterator links_end = std::sregex_iterator();

    for (std::sregex_iterator i = links_begin; i != links_end; ++i) {
        std::string new_link = (*i).str();
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (visited_urls.find(new_link) == visited_urls.end()) {
                url_queue.push(new_link);
            }
        }
    }
    // Если есть состав
    std::regex div_regex(R"(sostav)");
    if (std::regex_search(page_content, div_regex))
        return true;
    return false;
}

int download_page(const std::string& url) {
    std::cout << "URL: " << url << " ";
    int rc;
    CURLcode res;
    std::ostringstream readBuffer;
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Ошибка инициализации CURL" << std::endl;
        return ERROR_INIT_CURL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    // printf("CUR_CURL3 %d ", res);
    if (res == CURLE_OK && parse_page(readBuffer.str(), url)) {
        // puts("wertyuio2");
        create_directory(DATA_DIR);
        // puts("wertyuio");
        std::ofstream file(DATA_DIR + std::to_string(std::hash<std::string>{}(url)) + ".html");
        if (file) {
            file << readBuffer.str();
            rc = EXIT_SUCCESS;
            puts("download");
        } 
        else {
            std::cerr << "Ошибка открытия файла для записи" << std::endl;
            rc = ERROR_OPEN_FILE;
        }
    } 
    else if (res != CURLE_OK) {
        std::cerr << "Ошибка CURL: " << curl_easy_strerror(res) << std::endl;
        rc = ERROR_CURL;
    }
    else {
        rc = INVALID_PAGE;
        puts("skip");
    }
    return rc;
}

void process_url(const int max_pages) {
    while (true) {
        std::string url;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return !url_queue.empty() || done; });
            if (done && url_queue.empty())
                break;
            else if (url_queue.empty()) {
                puts("Недостаточно страниц для загрузки");
                break;
            }
            url = url_queue.front();
            url_queue.pop();
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (downloaded_pages == max_pages) {
                done = true;
                cv.notify_all();
                break;
            }
        }
        if (!download_page(url)) {
            std::lock_guard<std::mutex> lock(mtx);
            visited_urls.insert(url);
            downloaded_pages++;
        }
    }
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
    for (std::sregex_iterator i = links_begin; i != links_end; ++i) {
        std::string new_link = ROOT_URL + i->str();
        std::cout << new_link << std::endl;
        urls.insert(new_link);
    }
}

int get_category_urls(std::set<std::string>& category_urls, const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Ошибка инициализации CURL" << std::endl;
        return ERROR_INIT_CURL;
    }

    std::ostringstream readBuffer;
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

long long multi_threaded_download(int num_threads, const std::string &start_url, const int max_pages) {
    std::vector<std::thread> threads;
    long long start_time;
    std::set<std::string> category_urls;
    downloaded_pages = 0;

    if (get_category_urls(category_urls, start_url))
        return ERROR_GET_CATEGORY;
    for (const std::string& link : category_urls)
        url_queue.push(link);

    visited_urls.clear();
    start_time = time_now();

    for (int i = 0; i < num_threads; ++i)
        threads.emplace_back(process_url, max_pages);
    for (std::thread &thread : threads)
        thread.join();

    return time_now() - start_time;
}

long long single_thread_download(const std::string &start_url, const int max_pages) {
    long long start_time;
    std::string url;
    int download_rc;
    downloaded_pages = 0;

    std::set<std::string> category_urls;
    if (get_category_urls(category_urls, start_url))
        return ERROR_GET_CATEGORY;
    for (const std::string& link : category_urls)
        url_queue.push(link);
    // std::cout << url_queue << std::endl;
    visited_urls.clear();
    start_time = time_now();

    while (downloaded_pages < max_pages) {
        if (url_queue.empty()) {
            puts("Недостаточно страниц для загрузки");
            break;
        }
        url = url_queue.front();
        url_queue.pop();
        download_rc = download_page(url);
        printf("rc = %d\n", download_rc);
        if (!download_rc) {
            visited_urls.insert(url);
            downloaded_pages++;
        }
        else if (download_rc == INVALID_PAGE)
            continue;
        else
            return ERROR_DOWNLOAD_PAGE;
    }
    return time_now() - start_time;
}

void save_histogram(const std::vector<int>& thread_counts, const std::vector<long long>& times) {
    Gnuplot gp;
    gp << "reset\n";
    gp << "NO_ANIMATION = 1\n";
    gp << "set terminal svg size 1920, 1080\n";
    gp << "set size ratio 0.61803\n";
    gp << "set grid\n";
    gp << "set title 'Зависимость времени выгрузки от количества потоков'\n";
    gp << "set xlabel 'Количество потоков (шт)'\n";
    gp << "set ylabel 'Время (с)'\n";
    gp << "set output '" << HISTOGRAM_FILE << "'\n";

    // Подготовка данных для gnuplot
    std::vector<std::pair<int, long long>> data;
    for (size_t i = 0; i < thread_counts.size(); ++i) {
        data.emplace_back(thread_counts[i], times[i]);
    }

    gp << "plot '-' with linespoints title 'Время выгрузки'\n";
    gp.send1d(data);
}

void plot() {
    // Определение количества логических ядер
    int k = std::thread::hardware_concurrency();
    int max_threads = 4 * k;

    std::vector<int> thread_counts = {1, 2, 4, 8, 16};
    std::vector<long long> times = {1000, 800, 600, 400, 200}; 
    // std::vector<int> thread_counts;
    // std::vector<long long> times;

    // thread_counts.push_back(0);
    // times.push_back(single_thread_download(URL, NUM_PAGES));
    // for (int num_threads = 1; num_threads <= max_threads; num_threads *= 2) {
    //     long long time_taken = multi_threaded_download(num_threads, URL, NUM_PAGES);
    //     thread_counts.push_back(num_threads);
    //     times.push_back(time_taken);
    // }

    save_histogram(thread_counts, times);
}

int get_recepts_urls(std::set<std::string>& recept_urls, const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Ошибка инициализации CURL" << std::endl;
        return ERROR_INIT_CURL;
    }

    std::ostringstream readBuffer;
    CURLcode res = perform_curl_request(curl, url, readBuffer);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK)
        extract_urls(readBuffer.str(), recept_urls, R"(https:\/\/www\.gotovim\.ru\/sbs\/[a-z]+\.shtml)");
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
        tmp_url_queue.pop();
        if (get_recepts_urls(recept_urls, URL))
            return ERROR_GET_RECEPTS;
        for (const std::string& link : recept_urls)
            full_url_queue.push(link);
    }
    return EXIT_SUCCESS;
}

int main(void) {
    long long time;
    int max_pages = -1;
    int threads = -1;
    int command = -1;
    // Подготовка ссылок
    std::queue<std::string> full_url_queue;
    if (pre_proc(full_url_queue))
        return ERROR_PRE_PROC;

    while (command != 0)
    {
        printf(MENU);
        while ((fscanf(stdin, "%d", &command) != 1 || command < 0 || command > 4)) 
        {
            puts("Некорректная комманда");
            printf(">");
            clean_stdin();
        }
        clean_stdin();
        switch (command)
        {
            case 1:
                get_max_pages(max_pages);
                break;
            case 2:
                url_queue = full_url_queue;
                get_max_pages(max_pages);
                if ((time = single_thread_download(URL, max_pages)) < 0)
                    puts("Произошла ошибка при загрузке страницы");
                else
                    print_time_prosess(0, time);
                break;
            case 3:
                url_queue = full_url_queue;
                get_max_pages(max_pages);
                get_num_threads(threads);
                if ((time = multi_threaded_download(threads, URL, max_pages)) < 0)
                    puts("Произошла ошибка при загрузке страницы");
                else
                    print_time_prosess(threads, time);
                break;
            case 4:
                url_queue = full_url_queue;
                plot();
                break;
        }
    }
    
    return 0;
}
