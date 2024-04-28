#pragma once

#ifndef EAnalyserStatus_h
#define EAnalyserStatus_h

namespace SQR {
    enum EAnalyserStatus {
        NOT_INITIALIZED,
        INITIALIZED,
        OK,
        COM_LIBRARY_INIT_ERROR,
        COM_SECURITY_INIT_ERROR,
        IWBEMLOCATOR_CREATE_ERROR,
        WMI_CONNECT_ERROR,
        WMI_QUERY_ERROR
    };
}

#endif /* EAnalyserStatus_h */