#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <pplawait.h>

//
// This is set to compile in Debug & Release for x86
// It uses the cpprest library I got via vcpkg
//
// To get it to run it needs DLLs from [Repos]\vcpkg\installed\x86-windows\[debug]\bin
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
