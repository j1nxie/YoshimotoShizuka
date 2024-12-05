#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>

class HttpRequest {
private:
    std::string _method;
    std::string _path;
    std::string _body;

public:
    std::string method() const;

    std::string path() const;

    std::string body() const;

    void method(const std::string& method);

    void path(const std::string& path);

    void body(const std::string& body);
};

#endif
