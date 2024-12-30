#include <regex>
#include <set>
#include <codecvt>
#include <iostream>

#include "task.hpp"
#define EMPTY "empty"

// std::string clean_string(const std::string& str) {
//     std::string cleaned = std::regex_replace(str, std::regex(R"([^а-яА-Я0-9., -])"), "");  // чистим артефакты
//     cleaned = std::regex_replace(cleaned, std::regex(R"(\s{2,})"), " ");   // чистим более 2-х пробелов подряд
//     cleaned = std::regex_replace(cleaned, std::regex(R"(^\s+|\s+$)"), "");     // чистим пробелы в начале и в конце строки
//     return cleaned;
// }

std::wstring clean_string(const std::wstring& str) {
    std::wstring cleaned = std::regex_replace(str, std::wregex(L"[^а-яА-Я0-9., -]"), L"");  // чистим артефакты
    cleaned = std::regex_replace(cleaned, std::wregex(L"\\s{2,}"), L" ");       // чистим более 2-х пробелов подряд
    cleaned = std::regex_replace(cleaned, std::wregex(L"^\\s+|\\s+$"), L"");    // чистим пробелы в начале и в конце строки
    return cleaned;
}

void extract_elem(const std::string& page_content, std::string& a, const std::string& mask) {
    std::regex elem_regex(mask);
    std::smatch match;
    if (std::regex_search(page_content, match, elem_regex))
        a = match.str(1);
    else
        a = EMPTY;
}

void extract_set(const std::string& page_content, std::set<std::string>& a, const std::string& mask) {
    std::regex set_regex(mask);
    std::sregex_iterator begin = std::sregex_iterator(page_content.begin(), page_content.end(), set_regex);
    std::sregex_iterator end = std::sregex_iterator();

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    for (std::sregex_iterator i = begin; i != end; ++i)
        a.insert(converter.to_bytes(clean_string(converter.from_bytes(i->str()))));
}

void Task::parse_html() {
    std::string url;
    std::string title;
    std::set<std::string> ingredients;
    std::set<std::string> steps;
    std::string image_url;

    // <link rel="canonical" href="https://www.gotovim.ru/sbs/grillgovyadkefir.shtml"/>
    extract_elem(this->page_content, url, R"(<link\s+rel=\"canonical\"\s+href=\"(.*?)\"\/>)");
    // <h1 itemprop="name" alt="Молочный коктейль с мороженым, бананом и манго">Молочный коктейль с мороженым, бананом и манго</h1>
    extract_elem(this->page_content, title, R"(<h1[^>]*>(.*?)<\/h1>)");
    // <li itemprop="recipeIngredient">молоко (жирность любая) - 500 мл</li>
    extract_set(this->page_content, ingredients, R"(<li itemprop=\"recipeIngredient\">(.*?)<\/li>)");
    // <p itemprop="recipeInstructions">"Влейте маринад в ёмкость с ребрышками и хорошо перемешайте, чтобы каждый кусок был покрыт пряной смесью. Оставьте ребрышки мариноваться на ночь или минимум на 4 часа."</p>
    extract_set(this->page_content, steps, R"(<p itemprop=\"recipeInstructions\">(.*?)<\/p>)");
    // <img itemprop="image" width="700" height="525" title="Джем из мандаринов. Фотография рецепта" alt="Джем из мандаринов. Фотография рецепта" class="img2 astra" id="astra" src="//www.gotovim.ru/pics/sbs/dzhemanadrinov/rec.jpg">
    extract_elem(this->page_content, image_url, R"(<img\s+itemprop=\"image\"\s+itemprop=\"[^"]+\"\s+width=\"[^"]+\"\s+height=\"[^"]+\"\s+title=\"[^"]+\"\s+alt=\"[^"]+\"\s+class=\"[^"]+\"\s+id=\"[^"]+\"[^>]+src=\"(.*?)\">)");

    this->url = url;
    this->title = title;
    this->ingredients = ingredients;
    this->steps = steps;
    this->image_url = image_url;
}
