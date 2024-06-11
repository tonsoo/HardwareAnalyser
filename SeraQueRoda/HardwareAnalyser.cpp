#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <string>
#include <string.h>
#include <locale>
#include <codecvt>
#include "nvml.h"

#pragma comment(lib, "wbemuuid.lib")

#include "HardwareAnalyser.h"

wchar_t* toLPCWSTR(const char* charString) {
    int charLength = strlen(charString) + 1;

    int wideLength = MultiByteToWideChar(CP_ACP, 0, charString, charLength, NULL, 0);

    wchar_t* wideString = new wchar_t[wideLength];

    MultiByteToWideChar(CP_ACP, 0, charString, charLength, wideString, wideLength);

    return wideString;
}

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

    #define NVMLQUERY_DEFAULT_NVML_DLL_PATH "C:\\Program Files\\NVIDIA Corporation\\NVSMI\\NVML.DLL"
    #define NVMLQUERY_ALTERNATIVE_NVML_DLL_PATH "C:\\Windows\\System32\\nvml.dll"

    PlacaDeVideo* HardwareAnalyser::GPU_NVIDIA() {
        PlacaDeVideo* iRet = NULL;

        char* name{ new char[50] };
        char* fabricante{ new char[6] { 'N', 'V', 'I', 'D', 'I' , 'A' }};
        int cores = 0;
        int threads = 0;
        double clockSpeed = 0;
        double clockTurboSpeed = 0;
        double cache = 0;

        double vram = 0;

        nvmlReturn_t nvRetValue = NVML_ERROR_UNINITIALIZED;

        HINSTANCE hDLLhandle = NULL;
        hDLLhandle = LoadLibrary(toLPCWSTR(NVMLQUERY_DEFAULT_NVML_DLL_PATH));

        if (NULL == hDLLhandle)
        {
            hDLLhandle = LoadLibrary(toLPCWSTR(NVMLQUERY_ALTERNATIVE_NVML_DLL_PATH));
            if (NULL == hDLLhandle)
            {
                return iRet;
            }
        }

        typedef nvmlReturn_t(*PFNnvmlInit)(void);
        typedef nvmlReturn_t(*PFNnvmlShutdown)(void);
        typedef char* (*PFNnvmlErrorString)(nvmlReturn_t result);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetCount)(unsigned int* deviceCount);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetHandleByIndex)(unsigned int index, nvmlDevice_t* device);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetUtilizationRates)(nvmlDevice_t device, nvmlUtilization_t* utilization);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetEncoderUtilization)(nvmlDevice_t device, unsigned int* utilization, unsigned int* samplingPeriodUs);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetDecoderUtilization)(nvmlDevice_t device, unsigned int* utilization, unsigned int* samplingPeriodUs);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetMemoryInfo)(nvmlDevice_t device, nvmlMemory_t* memory);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetName)(nvmlDevice_t device, char* name, unsigned int length);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetNumGpuCores)(nvmlDevice_t device, unsigned int* numCores);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetClock)(nvmlDevice_t device, nvmlClockType_t clockType, nvmlClockId_t clockId, unsigned int* clockMHz);
        typedef nvmlReturn_t(*PFNnvmlDeviceGetMaxClockInfo)(nvmlDevice_t device, nvmlClockType_t type, unsigned int* clock);

        PFNnvmlInit pfn_nvmlInit = NULL;
        PFNnvmlShutdown pfn_nvmlShutdown = NULL;
        PFNnvmlErrorString pfn_nvmlErrorString = NULL;
        PFNnvmlDeviceGetCount pfn_nvmlDeviceGetCount = NULL;
        PFNnvmlDeviceGetHandleByIndex pfn_nvmlDeviceGetHandleByIndex = NULL;
        PFNnvmlDeviceGetUtilizationRates pfn_nvmlDeviceGetUtilizationRates = NULL;
        PFNnvmlDeviceGetEncoderUtilization pfn_nvmlDeviceGetEncoderUtilization = NULL;
        PFNnvmlDeviceGetDecoderUtilization pfn_nvmlDeviceGetDecoderUtilization = NULL;
        PFNnvmlDeviceGetMemoryInfo pfn_nvmlDeviceGetMemoryInfo = NULL;
        PFNnvmlDeviceGetName pfn_nvmlDeviceGetName = NULL;
        PFNnvmlDeviceGetNumGpuCores pfn_nvmlDeviceGetNumGpuCores = NULL;
        PFNnvmlDeviceGetClock pfn_nvmlDeviceGetClock = NULL;
        PFNnvmlDeviceGetMaxClockInfo pfn_nvmlDeviceGetMaxClockInfo = NULL;

        pfn_nvmlInit = (PFNnvmlInit)GetProcAddress(hDLLhandle, "nvmlInit");
        pfn_nvmlShutdown = (PFNnvmlShutdown)GetProcAddress(hDLLhandle, "nvmlShutdown");
        pfn_nvmlErrorString = (PFNnvmlErrorString)GetProcAddress(hDLLhandle, "nvmlErrorString");
        pfn_nvmlDeviceGetCount = (PFNnvmlDeviceGetCount)GetProcAddress(hDLLhandle, "nvmlDeviceGetCount");
        pfn_nvmlDeviceGetHandleByIndex = (PFNnvmlDeviceGetHandleByIndex)GetProcAddress(hDLLhandle, "nvmlDeviceGetHandleByIndex");
        pfn_nvmlDeviceGetName = (PFNnvmlDeviceGetName)GetProcAddress(hDLLhandle, "nvmlDeviceGetName");
        pfn_nvmlDeviceGetUtilizationRates = (PFNnvmlDeviceGetUtilizationRates)GetProcAddress(hDLLhandle, "nvmlDeviceGetUtilizationRates");
        pfn_nvmlDeviceGetEncoderUtilization = (PFNnvmlDeviceGetEncoderUtilization)GetProcAddress(hDLLhandle, "nvmlDeviceGetEncoderUtilization");
        pfn_nvmlDeviceGetDecoderUtilization = (PFNnvmlDeviceGetDecoderUtilization)GetProcAddress(hDLLhandle, "nvmlDeviceGetDecoderUtilization");
        pfn_nvmlDeviceGetMemoryInfo = (PFNnvmlDeviceGetMemoryInfo)GetProcAddress(hDLLhandle, "nvmlDeviceGetMemoryInfo");
        pfn_nvmlDeviceGetNumGpuCores = (PFNnvmlDeviceGetNumGpuCores)GetProcAddress(hDLLhandle, "nvmlDeviceGetNumGpuCores");
        pfn_nvmlDeviceGetClock = (PFNnvmlDeviceGetClock)GetProcAddress(hDLLhandle, "nvmlDeviceGetClock");
        pfn_nvmlDeviceGetMaxClockInfo = (PFNnvmlDeviceGetMaxClockInfo)GetProcAddress(hDLLhandle, "nvmlDeviceGetMaxClockInfo");

        nvRetValue = pfn_nvmlInit();

        if (NVML_SUCCESS != nvRetValue)
        {
            printf("[%s] error code :%d\r\n", "nvmlInit", nvRetValue);
            FreeLibrary(hDLLhandle);
            hDLLhandle = NULL;
            return iRet;
        }

        unsigned int uiNumGPUs = 0;
        nvRetValue = pfn_nvmlDeviceGetCount(&uiNumGPUs);

        if (NVML_SUCCESS != nvRetValue)
        {
            pfn_nvmlShutdown();
            FreeLibrary(hDLLhandle);
            hDLLhandle = NULL;
            return iRet;
        }

        if (0 == uiNumGPUs)
        {
            printf("No NVIDIA GPUs were detected.\r\n");
            pfn_nvmlShutdown();
            FreeLibrary(hDLLhandle);
            hDLLhandle = NULL;
            return iRet;
        }

        bool bGPUUtilSupported = true;
        bool bEncoderUtilSupported = true;
        bool bDecoderUtilSupported = true;

        for (unsigned int iDevIDX = 0; iDevIDX < uiNumGPUs; iDevIDX++)
        {
            nvmlDevice_t nvGPUDeviceHandle = NULL;
            nvRetValue = pfn_nvmlDeviceGetHandleByIndex(iDevIDX, &nvGPUDeviceHandle);

            if (NVML_SUCCESS != nvRetValue)
            {
                pfn_nvmlShutdown();
                FreeLibrary(hDLLhandle);
                hDLLhandle = NULL;
                return iRet;
            }

            name = new char[NVML_DEVICE_NAME_BUFFER_SIZE] { '\0' };
            nvRetValue = pfn_nvmlDeviceGetName(nvGPUDeviceHandle, name, NVML_DEVICE_NAME_BUFFER_SIZE);

            if (NVML_SUCCESS != nvRetValue)
            {
                pfn_nvmlShutdown();
                FreeLibrary(hDLLhandle);
                hDLLhandle = NULL;
                return iRet;
            }

            nvmlUtilization_t nvUtilData;
            nvRetValue = pfn_nvmlDeviceGetUtilizationRates(nvGPUDeviceHandle, &nvUtilData);
            if (NVML_SUCCESS != nvRetValue)
            {
                if (NVML_ERROR_NOT_SUPPORTED != nvRetValue)
                {
                    pfn_nvmlShutdown();
                    FreeLibrary(hDLLhandle);
                    hDLLhandle = NULL;
                    return iRet;
                }

                bGPUUtilSupported = false;
            }

            nvmlMemory_t GPUmemoryInfo;
            ZeroMemory(&GPUmemoryInfo, sizeof(GPUmemoryInfo));
            nvRetValue = pfn_nvmlDeviceGetMemoryInfo(nvGPUDeviceHandle, &GPUmemoryInfo);
            if (NVML_SUCCESS != nvRetValue)
            {
                pfn_nvmlShutdown();
                FreeLibrary(hDLLhandle);
                hDLLhandle = NULL;
                return iRet;
            }

            unsigned long long ullFrameBufferUsedBytes = 0L;
            ullFrameBufferUsedBytes = GPUmemoryInfo.total - GPUmemoryInfo.free;

            unsigned long ulFrameBufferTotalKBytes = 0L;
            unsigned long ulFrameBufferUsedKBytes = 0L;

            if (ULONG_MAX < GPUmemoryInfo.total)
            {
                GPUmemoryInfo.total = 10;
                //printf("ERROR: GPU memory size exceeds variable limit\r\n");
                //pfn_nvmlShutdown();
                //FreeLibrary(hDLLhandle);
                //hDLLhandle = NULL;
                //return iRetValue;
            }

            ulFrameBufferTotalKBytes = (unsigned long)(GPUmemoryInfo.total / 1024L);
            ulFrameBufferUsedKBytes = (unsigned long)(ulFrameBufferTotalKBytes - (GPUmemoryInfo.free / 1024L));

            std::cout << "vram: " << GPUmemoryInfo.total << "\n";
            vram = ulFrameBufferTotalKBytes;

            double dMemUtilzation = (((double)ulFrameBufferUsedKBytes / (double)ulFrameBufferTotalKBytes) * 100.0);

            unsigned int uiVidEncoderUtil = 0u;
            unsigned int uiVideEncoderLastSample = 0u;
            nvRetValue = pfn_nvmlDeviceGetEncoderUtilization(nvGPUDeviceHandle, &uiVidEncoderUtil, &uiVideEncoderLastSample);
            if (NVML_SUCCESS != nvRetValue)
            {
                if (NVML_ERROR_NOT_SUPPORTED != nvRetValue)
                {
                    pfn_nvmlShutdown();
                    FreeLibrary(hDLLhandle);
                    hDLLhandle = NULL;
                    return iRet;
                }

                bEncoderUtilSupported = false;
            }

            unsigned int uiVidDecoderUtil = 0u;
            unsigned int uiVidDecoderLastSample = 0u;
            nvRetValue = pfn_nvmlDeviceGetDecoderUtilization(nvGPUDeviceHandle, &uiVidDecoderUtil, &uiVidDecoderLastSample);
            if (NVML_SUCCESS != nvRetValue)
            {
                if (NVML_ERROR_NOT_SUPPORTED != nvRetValue)
                {
                    pfn_nvmlShutdown();
                    FreeLibrary(hDLLhandle);
                    hDLLhandle = NULL;
                    return iRet;
                }

                bDecoderUtilSupported = false;
            }

            nvmlMemory_t memory;
            nvRetValue = pfn_nvmlDeviceGetMemoryInfo(nvGPUDeviceHandle, &memory);
            if (nvRetValue != NVML_SUCCESS) {
                return iRet;
            }
            vram = (double)memory.total / 1e+6;

            unsigned int cores = 0u;
            nvRetValue = pfn_nvmlDeviceGetNumGpuCores(nvGPUDeviceHandle, &cores);
            if (nvRetValue != NVML_SUCCESS && NVML_ERROR_NOT_SUPPORTED != nvRetValue)
            {
                pfn_nvmlShutdown();
                FreeLibrary(hDLLhandle);
                hDLLhandle = NULL;
                return iRet;
            }

            std::cout << "Cores: " << cores << "\n";

            unsigned int _clock = 0u;
            nvRetValue = pfn_nvmlDeviceGetClock(nvGPUDeviceHandle, NVML_CLOCK_VIDEO, NVML_CLOCK_ID_CURRENT, &_clock);
            if (nvRetValue != NVML_SUCCESS && NVML_ERROR_NOT_SUPPORTED != nvRetValue)
            {
                pfn_nvmlShutdown();
                FreeLibrary(hDLLhandle);
                hDLLhandle = NULL;
                return iRet;
            }

            std::cout << "Clock: " << (int)_clock << "MHz\n";

            unsigned int _max_clock = 0u;
            nvRetValue = pfn_nvmlDeviceGetMaxClockInfo(nvGPUDeviceHandle, NVML_CLOCK_GRAPHICS, &_max_clock);
            if (nvRetValue != NVML_SUCCESS && NVML_ERROR_NOT_SUPPORTED != nvRetValue)
            {
                pfn_nvmlShutdown();
                FreeLibrary(hDLLhandle);
                hDLLhandle = NULL;
                return iRet;
            }

            std::cout << "Max Clock: " << _max_clock << "MHz\n";
        }

        nvRetValue = pfn_nvmlShutdown();

        FreeLibrary(hDLLhandle);
        hDLLhandle = NULL;

        /* WINDOWS QUERY METHOD */

        /*
        // Chamada para inicializar o enumerator dos componentes do Processador
        IEnumWbemClassObject* hardwareEnumerator = this->Scan("SELECT * FROM Win32_Processor");

        // Declaração das variaveis do Processador

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
        */

        return new PlacaDeVideo(vram, cores, threads, clockSpeed, clockTurboSpeed, cache, name, fabricante);
    }

    PlacaDeVideo* HardwareAnalyser::GPUInfo() {

        PlacaDeVideo* gpu = this->GPU_NVIDIA();

        return gpu;
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