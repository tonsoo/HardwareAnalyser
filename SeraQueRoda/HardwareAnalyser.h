#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <string>
#include <string.h>
#include <locale>
#include <codecvt>
#include <functional>

#pragma comment(lib, "wbemuuid.lib")

#include "Processador.h"
#include "EAnalyserStatus.h"

#ifndef HardwareAnalyser_h
#define HardwareAnalyser_h

namespace SQR {

    class HardwareAnalyser {
    public:
        HardwareAnalyser();
        IEnumWbemClassObject* Scan(const char* query);
        void Release();

        Processador* ProcessorInfo();
    private:
        void SetStatus(EAnalyserStatus status);
        EAnalyserStatus AnalyserStatus;
        void Filter(LPCWSTR query, HRESULT* hr, IWbemClassObject& filterObject, const std::function<void(VARIANT&)>& func);

        HRESULT hres;
        IWbemServices* hardwareService;
    };
}
#endif