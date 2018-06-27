#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;

//
// This was an attempt to write the results of a HTTP Get to a file. The only issue was ApplicationData
// is not callable from Win32/Desktop, only from a UWP.
//
// The workaround is to use GetFolderFromPathAsync() with a hard-coded path
//

IAsyncAction RestGet()
{
    // Option 1: The AppData for the current UWP app (no wonder it does not work)
    //auto appData = ApplicationData::Current();

    // Option 2: The AppData for a user (same error, HRESULT 0x80073d54)
    //auto userList = co_await Windows::System::User::FindAllAsync();
    ///*for(auto&& user : userList)
    //{
    //    auto firstName = co_await user.GetPropertyAsync(Windows::System::KnownUserProperties::FirstName());
    //    wprintf(L"UserType: %d\n", user.Type());
    //}*/
    //auto user = userList.GetAt(0);
    //auto appData = co_await ApplicationData::GetForUserAsync(user);

    //auto folder = appData.LocalFolder(); // There are other folders

    auto folder = co_await StorageFolder::GetFolderFromPathAsync(L"C:\\");

    auto file = co_await folder.CreateFileAsync(L"results.html", CreationCollisionOption::ReplaceExisting);
    auto fileStream = co_await file.OpenAsync(FileAccessMode::ReadWrite);
    auto fileOutputStream = fileStream.GetOutputStreamAt(0);

    Uri uri(L"http://www.bing.com/search?q=cpprestsdk+github");
    HttpClient httpClient;
    auto responseStream = co_await httpClient.GetInputStreamAsync(uri);

    co_await RandomAccessStream::CopyAsync(responseStream, fileOutputStream);
    co_await fileOutputStream.FlushAsync();
}

int main()
{
    init_apartment();

    RestGet().get();
}
