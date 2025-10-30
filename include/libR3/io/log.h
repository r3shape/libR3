/*
    libR3 — Logging API
    --------------------

    Overview
    ~~~~~~~~
    This module defines the lightweight logging interface used by
    the libR3 runtime kernel. It provides simple and structured
    console output utilities for debugging, tracing, and status
    reporting at various verbosity levels.

    The logging system is deliberately minimal — no dynamic
    allocation, no global state dependencies, and no file I/O.
    Its purpose is to provide a consistent runtime feedback layer
    for internal subsystems and user applications alike.

    Log Levels
    ~~~~~~~~~~
        R3_LOG_ERROR   — Fatal or critical errors (execution should halt)
        R3_LOG_WARN    — Recoverable issues or potential misuses
        R3_LOG_INFO    — High-level runtime status or diagnostics
        R3_LOG_DEV     — Developer-facing trace/debug information
        R3_LOG_OK      — Success and validation messages

    Design Notes
    ~~~~~~~~~~~~
    • Logging can be toggled on/off per level at runtime.
    • Supports both raw (`r3LogStdOut`) and formatted (`r3LogStdOutF`) output.
    • Uses ANSI escape codes for color and formatting when supported.
    • Intended for internal runtime feedback, not end-user output.

    Changelog
    ~~~~~~~~~
    [@zafflins 10/18/25 v1.0]
    ~ Initial implementation of the runtime logging interface.
      Added r3LogToggle(), r3LogStdOut(), r3LogStdOutF().

    [@zafflins 10/18/25 v1.0]
    Logging Example
    ```c
        r3LogStdOut(R3_LOG_INFO, "libR3 initialized successfully.\n");
        r3LogStdOutF(R3_LOG_DEV, "Allocated buffer at %p (%llu bytes)\n", ptr, size);
    ```
*/

#ifndef __R3_LOG_H__
#define __R3_LOG_H__

#include <include/libR3/r3def.h>

/* --------------------------------------------------------------------------
    Log Levels
-------------------------------------------------------------------------- */

typedef enum R3LogLevel {
    R3_LOG_ERROR,  // Critical failure
    R3_LOG_WARN,   // Potential issue
    R3_LOG_INFO,   // General runtime info
    R3_LOG_DEV,    // Developer/debug trace
    R3_LOG_OK      // Success messages
} R3LogLevel;

/* --------------------------------------------------------------------------
    Logging API
-------------------------------------------------------------------------- */

R3_PUBLIC_API none r3LogToggle(R3LogLevel level);
R3_PUBLIC_API none r3LogStdOut(R3LogLevel level, char* message);
R3_PUBLIC_API none r3LogStdOutF(R3LogLevel level, char* message, ...);

#endif /* __R3_LOG_H__ */
