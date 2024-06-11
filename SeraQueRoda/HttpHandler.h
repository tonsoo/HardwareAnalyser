#include <string>
#include <winsock2.h>

namespace Http {

	class HttpHandler {
	public:
		std::string ResponseBody;
		int ResponseCode;

		void AddParameter(const std::string& name, const std::string& value);
		void AddParameter(const std::string& name, const double& value);
		void AddParameter(const std::string& name, const int& value);
		void AddParameter(const std::string& name, const char& value);

		std::string Fetch(const std::string& initial_url, const std::string& port);
		static std::string UrlEncode(const std::string& url);

	private:
		std::string query;
		static bool IsReserved(char c);
		bool Initialize();
		SOCKET Connect(const std::string& host, const std::string& port);
	};
}