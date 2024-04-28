#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <string>
#include <string.h>
#include <locale>
#include <codecvt>
#include <nvml.h>

#pragma comment(lib, "wbemuuid.lib")

#include "HardwareAnalyser.h"

namespace SQR {

    HardwareAnalyser::HardwareAnalyser() {
        this->SetStatus(EAnalyserStatus::INITIALIZED);
        this->hardwareService = nullptr;

        // Inicialização do COM
        this->hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) {
            this->SetStatus(EAnalyserStatus::COM_LIBRARY_INIT_ERROR);
            return;
        }

        // Inicialização do COM Security
        this->hres = CoInitializeSecurity(NULL, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
        if (FAILED(hres) && hres != RPC_E_TOO_LATE) {
            this->SetStatus(EAnalyserStatus::COM_SECURITY_INIT_ERROR);
            CoUninitialize();
            return;
        }

        // Inicialização do namespace WMI
        IWbemLocator* hardwareLoc = nullptr;
        this->hres = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&hardwareLoc));
        if (FAILED(hres)) {
            this->SetStatus(EAnalyserStatus::IWBEMLOCATOR_CREATE_ERROR);
            CoUninitialize();
            return;
        }

        // Inicialização da conexção do namespace WMI
        this->hres = hardwareLoc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"),                // Namespace
            nullptr,                                // User name
            nullptr,                                // User password
            nullptr,                                // Locale
            0,                                      // Security flags
            nullptr,                                // Authority
            nullptr,                                // Context object
            &this->hardwareService                  // IWbemServices proxy
        );

        if (FAILED(hres)) {
            this->SetStatus(EAnalyserStatus::WMI_CONNECT_ERROR);
            hardwareLoc->Release();
            CoUninitialize();
            return;
        }
    }

    IEnumWbemClassObject* HardwareAnalyser::Scan(const char* query) {

        // Definir consulta WMI
        IEnumWbemClassObject* hardwareEnumerator = NULL;
        this->hres = this->hardwareService->ExecQuery(bstr_t("WQL"), bstr_t(query),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &hardwareEnumerator);
        if (FAILED(hres)) {
            this->SetStatus(EAnalyserStatus::WMI_QUERY_ERROR);
            return NULL;
        }

        // Define status OK, indicando que a inicialização foi concluida com sucesso
        this->SetStatus(EAnalyserStatus::OK);

        return hardwareEnumerator;
    }

    void HardwareAnalyser::Release() {

        CoUninitialize();
    }

    void HardwareAnalyser::SetStatus(EAnalyserStatus status) {

        bool error = true;
        switch (status) {
        case EAnalyserStatus::NOT_INITIALIZED:
            std::cerr << "Hardware Analyser has not been initialized.";
            break;
        case EAnalyserStatus::COM_LIBRARY_INIT_ERROR:
            std::cerr << "Failed to initialize COM library.";
            break;
        case EAnalyserStatus::COM_SECURITY_INIT_ERROR:
            std::cout << "Failed to initialize COM security.";
            break;
        case EAnalyserStatus::IWBEMLOCATOR_CREATE_ERROR:
            std::cerr << "Failed to create IWbemLocator object";
            break;
        case EAnalyserStatus::WMI_CONNECT_ERROR:
            std::cerr << "Failed to connect to WMI namespace.";
            break;
        case EAnalyserStatus::WMI_QUERY_ERROR:
            std::cerr << "Failed to execute WMI query.";
            break;
        case EAnalyserStatus::NVML_INIT_ERROR:
            std::cerr << "Failed to initialize NVML.";
            break;
        case EAnalyserStatus::GPU_HANDLER_ERROR:
            std::cerr << "Failed to get NVML handler.";
            break;
        case EAnalyserStatus::NVML_GET_ERROR:
            std::cerr << "Failed to retrieve information from NVML.";
            break;
        case EAnalyserStatus::OK:
        default:
            error = false;
            break;
        }
        
        if (error) {
            std::cerr << " Error code: 0x" << std::hex;
            HardwareAnalyser::Release();
        }

        std::cout << std::endl;

        this->AnalyserStatus = status;
    }

    Processador* HardwareAnalyser::ProcessorInfo() {

        // Chamada para inicializar o enumerator dos componentes do Processador
        IEnumWbemClassObject* hardwareEnumerator = this->Scan("SELECT * FROM Win32_Processor");

        // Declaração das variaveis do Processador
        char* name{ new char[50] };
        char* fabricante{ new char[50] };
        int cores = 0;
        int threads = 0;
        double clockSpeed = 0;
        double clockTurboSpeed = 0;
        double cache = 0;

        IWbemClassObject* filterObj;
        ULONG uReturn = 0;
        while (hardwareEnumerator) {
            HRESULT hr = hardwareEnumerator->Next(WBEM_INFINITE, 1, &filterObj, &uReturn);
            if (uReturn == 0) {
                break;
            }

            // Filtra a velocidade atual de clock do processador
            this->Filter(L"CurrentClockSpeed", &hr, *filterObj, [&clockSpeed](VARIANT& v) -> void {
                clockSpeed = v.uintVal;
                });

            // Filtra a velocidade maxima de clock do processador
            this->Filter(L"MaxClockSpeed", &hr, *filterObj, [&clockTurboSpeed](VARIANT& v) -> void {
                clockTurboSpeed = v.uintVal;
                });

            // Filtra a quantidade de nucloes do Processador
            this->Filter(L"NumberOfCores", &hr, *filterObj, [&cores](VARIANT& v) -> void {
                cores = v.uintVal;
                });

            // Filtra a quantidade de Threads do processador
            this->Filter(L"NumberOfLogicalProcessors", &hr, *filterObj, [&threads](VARIANT& v) -> void {
                threads = v.uintVal;
                });

            // Filtra as informações da quantidade de memoria cache do processador
            auto cacheFunc = [&cache](VARIANT& v) -> void {
                cache += v.uintVal;
                };
            this->Filter(L"L2CacheSize", &hr, *filterObj, cacheFunc);
            this->Filter(L"L3CacheSize", &hr, *filterObj, cacheFunc);

            // Filtra as informações do nome da peça
            this->Filter(L"Name", &hr, *filterObj, [&name](VARIANT& v) -> void {
                std::wstring tmp = v.bstrVal;
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::string str = converter.to_bytes(tmp);

                strcpy_s(name, 50, str.c_str());
                });

            // Filtra as informações do fabricante da peça
            this->Filter(L"Manufacturer", &hr, *filterObj, [&fabricante](VARIANT& v) -> void {
                std::wstring tmp = v.bstrVal;
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::string str = converter.to_bytes(tmp);

                strcpy_s(fabricante, 50, str.c_str());
                });

            filterObj->Release();
        }

        return new Processador(cores, threads, clockSpeed, clockTurboSpeed, cache / 1024, name, fabricante);
    }

    PlacaDeVideo* HardwareAnalyser::GPUInfo() {
        // Chamada para inicializar o enumerator dos componentes do Processador
        IEnumWbemClassObject* hardwareEnumerator = this->Scan("SELECT * FROM Win32_Processor");

        // Declaração das variaveis do Processador
        char* name{ new char[50] };
        char* fabricante{ new char[50] };
        int cores = 0;
        int threads = 0;
        double clockSpeed = 0;
        double clockTurboSpeed = 0;
        double cache = 0;

        double vram = 0;

        nvmlReturn_t result;

        // Inicialize a NVML
        result = nvmlInit();
        if (result != NVML_SUCCESS) {
            this->SetStatus(EAnalyserStatus::NVML_INIT_ERROR);
            return nullptr;
        }

        // Pegue o handle do dispositivo
        nvmlDevice_t device;
        result = nvmlDeviceGetHandleByIndex(0, &device); // Index 0 representa a primeira GPU
        if (result != NVML_SUCCESS) {
            this->SetStatus(EAnalyserStatus::GPU_HANDLER_ERROR);
            nvmlShutdown();
            return nullptr;
        }

        // Pegue o nome do dispositivo
        result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);

        //// Pegue o número de cores
        //result = nvmlcount(device, NVML_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, &numCores); // 0 representa o índice da GPU
        //if (result != NVML_SUCCESS) {
        //    std::cerr << "Falha ao obter o número de cores. Código de erro: " << nvmlErrorString(result) << std::endl;
        //    nvmlShutdown();
        //    return nullptr;
        //}

        //// Pegue o número de threads
        //unsigned int numThreads;
        //result = nvmlDeviceGetCudaEnabledDeviceCount(&numThreads);
        //if (result != NVML_SUCCESS) {
        //    std::cerr << "Falha ao obter o número de threads. Código de erro: " << nvmlErrorString(result) << std::endl;
        //    nvmlShutdown();
        //    return nullptr;
        //}
        //std::cout << "Número de threads: " << numThreads << std::endl;

        // Pegue a velocidade do clock
        unsigned int clock;
        result = nvmlDeviceGetMaxClockInfo(device, NVML_CLOCK_SM, &clock);
        if (result == NVML_SUCCESS) {
            clockSpeed = (double)clock / pow(10, 1e+6);
        }

        nvmlMemory_t memory;
        result = nvmlDeviceGetMemoryInfo(device, &memory);
        if (result == NVML_SUCCESS) {
            vram = memory.total / 1e+6;
        }

        nvmlShutdown();

        return new PlacaDeVideo(vram, cores, threads, clockSpeed, clockTurboSpeed, cache / 1024, name, fabricante);
    }

    void HardwareAnalyser::Filter(LPCWSTR query, HRESULT* hr, IWbemClassObject& filterObject, const std::function<void(VARIANT&)>& func) {
        VARIANT filterVariant;
        *hr = filterObject.Get(query, 0, &filterVariant, 0, 0);
        if (SUCCEEDED(*hr)) {
            func(filterVariant);
            VariantClear(&filterVariant);
        }
    }
}