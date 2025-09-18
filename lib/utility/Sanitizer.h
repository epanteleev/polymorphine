#pragma once

#ifdef USE_USAN
#define no_usan __attribute__((no_sanitize("undefined")))
#else
#define no_usan
#endif