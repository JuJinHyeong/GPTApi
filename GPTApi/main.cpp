#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    }
    catch (std::bad_alloc& e) {
        // handle memory problem
        return 0;
    }
}

int main() {
    std::setlocale(LC_ALL, "en_US.UTF-8"); // 콘솔 인코딩 설정
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        std::ifstream env("api_key.txt");
        std::string api_key;
        env >> api_key;

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth_header = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::ifstream file("jsons/test.json");
        json chatCompletion;
        file >> chatCompletion;
        std::string json_data = chatCompletion.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());

        std::string response_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        else
            std::cout << "Response: " << response_string << std::endl;

        curl_easy_cleanup(curl);
    }
    return 0;
}
