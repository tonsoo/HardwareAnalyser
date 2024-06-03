#include <string>
#include <winsock2.h>

namespace Http {

	class HttpHandler {
	public:
		std::string ResponseBody;
		int ResponseCode;

		std::string Fetch(const std::string& initial_url, const std::string& port);
		static std::string UrlEncode(const std::string& url);

	private:
		static bool IsReserved(char c);
		bool Initialize();
		SOCKET Connect(const std::string& host, const std::string& port);
	};
}