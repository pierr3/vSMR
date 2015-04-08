#pragma once
#include <string>
#include <curl\curl.h>
#include <curl\easy.h>

class HttpHelper
{
private:
	static std::string downloadedContents;
	static size_t handle_data(void *ptr, size_t size, size_t nmemb, void *stream);

public:
	HttpHelper();
	std::string downloadStringFromURL(std::string url);
	~HttpHelper();

};
