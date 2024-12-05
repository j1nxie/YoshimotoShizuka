#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <unordered_map>

class HttpResponse {
private:
    int _status{200};
    std::string _body;
    std::unordered_map<std::string, std::string> _headers;

public:
    int status() const;

    std::string body() const;

    std::unordered_map<std::string, std::string> headers() const;

    void body(const std::string& content);

    void status(int code);
};

#endif
