// https://www.gotovim.ru
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <condition_variable>
#include <sys/time.h>
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <gnuplot-iostream.h>
#include "pre_proc.hpp"

#define INVALID_PAGE 6
#define ERROR_PRE_PROC 7
#define ERROR_DOWNLOAD_PAGE -1

#define NUM_TESTS 2
#define NUM_PAGES 50
#define FILE_LINKS "links.csv"
#define DATA_DIR "./data/"
#define HISTOGRAM_FILE "histogram.svg"
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

bool file_exists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
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
    while (true) {
        std::cout << "Введите кол-во потоков: ";
        std::cin >> threads; 
        if (threads > 0 && threads < max_threads)
            break;
        std::cout << "Некорректное число потоков возможно (0-" << max_threads << ")" << std::endl;
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

bool parse_page(const std::string& page_content, const std::string& url) {
    // Если есть состав
    std::regex div_regex(R"(sostav)");
    if (std::regex_search(page_content, div_regex))
        return true;
    return false;
}

int download_page(const std::string& url) {
    std::cout << downloaded_pages + 1 << ") " << url << " ";
    std::ostringstream readBuffer;
    int rc;

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Ошибка инициализации CURL" << std::endl;
        return ERROR_INIT_CURL;
    }
    CURLcode res = perform_curl_request(curl, url, readBuffer);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK && parse_page(readBuffer.str(), url)) {
        create_directory(DATA_DIR);
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
            if (done)
                break;
            else if (url_queue.empty()) {
                puts("Недостаточно страниц для загрузки");
                break;
            }
            url = url_queue.front();
            url_queue.pop();

            if (downloaded_pages == max_pages) {
                done = true;
                cv.notify_all();
                break;
            }
            if (!download_page(url)) {
                visited_urls.insert(url);
                downloaded_pages++;
                cv.notify_all();
            }
        }
        sleep(1);
    }
}

long long multi_threaded_download(int num_threads, const int max_pages) {
    long long start_time;
    std::vector<std::thread> threads;

    visited_urls.clear();
    start_time = time_now();

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_url, max_pages);
        // sleep(100);
    }
    for (std::thread &thread : threads)
        thread.join();

    return time_now() - start_time;
}

long long single_thread_download(const int max_pages) {
    long long start_time;
    std::string url;
    int download_rc;

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
        sleep(1);
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

void plot(std::queue<std::string> full_url_queue) {
    long long time, summ = 0;
    // Определение количества логических ядер
    int k = std::thread::hardware_concurrency();
    int max_threads = 4 * k;
    std::vector<int> thread_counts;
    std::vector<long long> times;

    for (int i = 0; i < NUM_TESTS; ++i) {
        downloaded_pages = 0;
        url_queue = full_url_queue;
        summ += single_thread_download(NUM_PAGES);
    }
    thread_counts.push_back(0);
    times.push_back(summ / NUM_TESTS);

    for (int num_threads = 1; num_threads <= max_threads; num_threads++) {
        summ = 0;
        for (int i = 0; i < NUM_TESTS; ++i) {
            done = false;
            downloaded_pages = 0;
            url_queue = full_url_queue;
            summ += multi_threaded_download(num_threads, NUM_PAGES);
        }
        thread_counts.push_back(num_threads);
        times.push_back(summ / NUM_TESTS);
    }

    save_histogram(thread_counts, times);
}

int main(void) {
    long long time;
    int max_pages = -1;
    int threads = -1;
    int command = -1;
    // Подготовка ссылок
    std::queue<std::string> full_url_queue;
    // if (pre_proc(full_url_queue))
    //     return ERROR_PRE_PROC;
    if (file_exists(FILE_LINKS)) {
        read_queue_from_file(full_url_queue, FILE_LINKS);
        std::cout << "URLs прочитаны из файла" << std::endl;
    } 
    else {
        if (pre_proc(full_url_queue))
            return ERROR_PRE_PROC;
        write_queue_to_file(full_url_queue, FILE_LINKS);
        std::cout << "URLs сохранены в файл" << std::endl;
    }

    while (command != 0)
    {
        std::cout << MENU;
        while ((fscanf(stdin, "%d", &command) != 1 || command < 0 || command > 4)) 
        {
            puts("Некорректная комманда");
            std::cout << ">";
        }
        switch (command)
        {
            case 1:
                get_max_pages(max_pages);
                break;
            case 2:
                downloaded_pages = 0;
                url_queue = full_url_queue;
                get_max_pages(max_pages);
                if ((time = single_thread_download(max_pages)) < 0)
                    puts("Произошла ошибка при загрузке страницы");
                else
                    print_time_prosess(0, time);
                break;
            case 3:
                done = false;
                downloaded_pages = 0;
                url_queue = full_url_queue;
                get_max_pages(max_pages);
                get_num_threads(threads);
                if ((time = multi_threaded_download(threads, max_pages)) < 0)
                    puts("Произошла ошибка при загрузке страницы");
                else
                    print_time_prosess(threads, time);
                break;
            case 4:
                url_queue = full_url_queue;
                plot(full_url_queue);
                break;
        }
    }
    
    return 0;
}
