#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <pplawait.h>

//
// This uses the cpprestsdk from vcpkg
// 
// Build/install vcpkg:
//   https://docs.microsoft.com/en-us/cpp/vcpkg
//   vcpkg integrate install  ; With this you can use packages 'automatically'
//
// Build/install the cpprestsdk package:
// vcpkg install cpprestsdk
//

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

concurrency::task<void> rest_get()
{
    auto fileStream = co_await fstream::open_ostream(U("results.html"));

    // Create http_client to send the request.
    http_client client(U("http://www.bing.com/"));

    // Build request URI and start the request.
    uri_builder builder(U("/search"));
    builder.append_query(U("q"), U("cpprestsdk github"));
    auto response = co_await client.request(methods::GET, builder.to_string());

    printf("Received response status code:%u\n", response.status_code());

    // Write response body into the file.
    auto fileSize = co_await response.body().read_to_end(fileStream.streambuf());
}

int main(int argc, char* argv[])
{
    try
    {
        auto task = rest_get();
        task.get();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }

    return 0;
}
