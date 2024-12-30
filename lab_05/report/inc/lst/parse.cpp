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

    extract_elem(this->page_content, url, R"(<link\s+rel=\"canonical\"\s+href=\"(.*?)\"\/>)");
    extract_elem(this->page_content, title, R"(<h1[^>]*>(.*?)<\/h1>)");
    extract_set(this->page_content, ingredients, R"(<li itemprop=\"recipeIngredient\">(.*?)<\/li>)");
    extract_set(this->page_content, steps, R"(<p itemprop=\"recipeInstructions\">(.*?)<\/p>)");
    extract_elem(this->page_content, image_url, R"(<img\s+itemprop=\"image\"\s+itemprop=\"[^"]+\"\s+width=\"[^"]+\"\s+height=\"[^"]+\"\s+title=\"[^"]+\"\s+alt=\"[^"]+\"\s+class=\"[^"]+\"\s+id=\"[^"]+\"[^>]+src=\"(.*?)\">)");

    this->url = url;
    this->title = title;
    this->ingredients = ingredients;
    this->steps = steps;
    this->image_url = image_url;
}

