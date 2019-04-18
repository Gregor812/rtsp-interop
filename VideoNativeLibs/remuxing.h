#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef void (*Callback)();

    int remux(Callback callback);
    void Log();

#ifdef __cplusplus
}
#endif
