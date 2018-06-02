#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

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

void rest_get()
{
    auto pFileStream = std::make_shared<ostream>();

    // Open stream to output file.
    auto requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
    {
        *pFileStream = outFile;

        // Create http_client to send the request.
        http_client client(U("http://www.bing.com/"));

        // Build request URI and start the request.
        uri_builder builder(U("/search"));
        builder.append_query(U("q"), U("cpprestsdk github"));
        return client.request(methods::GET, builder.to_string());
    })

    // Handle response headers arriving.
    .then([=](http_response response)
    {
        printf("Received response status code:%u\n", response.status_code());

        // Write response body into the file.
        return response.body().read_to_end(pFileStream->streambuf());
    })

    // Close the file stream.
    .then([=](size_t)
    {
        return pFileStream->close();
    });

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        requestTask.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }
}

int main(int argc, char* argv[])
{
    rest_get();

    return 0;
}
