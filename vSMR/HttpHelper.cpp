#include "stdafx.h"
#include "HttpHelper.hpp"

//
// HttpHelper Class by Even Rognlien, used with permission
//

std::string HttpHelper::downloadedContents;

HttpHelper::HttpHelper()  {

}

// Used for downloading strings from web:
size_t HttpHelper::handle_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	int numbytes = size*nmemb;
	// The data is not null-terminated, so get the last character, and replace it with '\0'. 
	char lastchar = *((char *)ptr + numbytes - 1);
	*((char *)ptr + numbytes - 1) = '\0';
	downloadedContents.append((char *)ptr);
	downloadedContents.append(1, lastchar);
	*((char *)ptr + numbytes - 1) = lastchar;  // Might not be necessary. 
	return size*nmemb;
}

std::string HttpHelper::downloadStringFromURL(std::string url) {
	CURL *curl = curl_easy_init();
	if (curl)
	{
		downloadedContents = "";
		// Tell libcurl the URL 
		curl_easy_setopt(curl, CURLOPT_URL, url);
		// Tell libcurl what function to call when it has data 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpHelper::handle_data);
		// Do it! 
		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return downloadedContents;
	}
	return "";
}

HttpHelper::~HttpHelper() {

}