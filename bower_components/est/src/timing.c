/*
    timing.c -- Portable interface to the CPU cycle counter

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#include "est.h"

#if ME_EST_TIMING

#if WINDOWS
struct _hr_time {
    LARGE_INTEGER start;
};
#else
struct _hr_time {
    struct timeval start;
};
#endif

#if WINDOWS
ulong hardclock(void)
{
    LARGE_INTEGER  now;
    QueryPerformanceCounter(&now);
    return (ulong) (((uint64) now.HighPart) << 32) + now.LowPart;
}

#elif (defined(_MSC_VER) && defined(_M_IX86)) || defined(__WATCOMC__)

#elif WINDOWS
ulong hardclock(void)
{
    ulong tsc;
    __asm rdtsc 
    __asm mov[tsc], eax 
    return (tsc);
}

#elif defined(__GNUC__) && defined(__i386__)

ulong hardclock(void)
{
    ulong tsc;
    asm("rdtsc":"=a"(tsc));
    return (tsc);
}

#elif defined(__GNUC__) && (defined(__amd64__) || defined(__x86_64__))

ulong hardclock(void)
{
    ulong lo, hi;
    asm("rdtsc":"=a"(lo), "=d"(hi));
    return (lo | (hi << 32));
}

#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__ppc__))

ulong hardclock(void)
{
    ulong tbl, tbu0, tbu1;

    do {
        asm("mftbu %0":"=r"(tbu0));
        asm("mftb   %0":"=r"(tbl));
        asm("mftbu %0":"=r"(tbu1));
    } while (tbu0 != tbu1);
    return (tbl);
}

#elif defined(__GNUC__) && defined(__sparc__)

ulong hardclock(void)
{
    ulong tick;
    asm(".byte 0x83, 0x41, 0x00, 0x00");
    asm("mov    %%g1, %0":"=r"(tick));
    return (tick);
}

#elif defined(__GNUC__) && defined(__alpha__)

ulong hardclock(void)
{
    ulong cc;
    asm("rpcc %0":"=r"(cc));
    return (cc & 0xFFFFFFFF);
}

#elif defined(__GNUC__) && defined(__ia64__)

ulong hardclock(void)
{
    ulong itc;
    asm("mov %0 = ar.itc":"=r"(itc));
    return (itc);
}

#else

static int hardclock_init = 0;
static struct timeval tv_init;

ulong hardclock(void)
{
    struct timeval tv_cur;

    if (hardclock_init == 0) {
        gettimeofday(&tv_init, NULL);
        hardclock_init = 1;
    }
    gettimeofday(&tv_cur, NULL);
    return ((tv_cur.tv_sec - tv_init.tv_sec) * 1000000 + (tv_cur.tv_usec - tv_init.tv_usec));
}

#endif

int alarmed = 0;

#if WINDOWS

ulong get_timer(struct hr_time *val, int reset)
{
    ulong delta;
    LARGE_INTEGER offset, hfreq;
    struct _hr_time *t = (struct _hr_time *)val;

    QueryPerformanceCounter(&offset);
    QueryPerformanceFrequency(&hfreq);

    delta = (ulong)((1000 * (offset.QuadPart - t->start.QuadPart)) / hfreq.QuadPart);
    if (reset) {
        QueryPerformanceCounter(&t->start);
    }
    return (delta);
}


DWORD WINAPI TimerProc(LPVOID uElapse)
{
    Sleep((DWORD) uElapse);
    alarmed = 1;
    return (TRUE);
}


#if UNUSED
void set_alarm(int seconds)
{
    DWORD ThreadId;
    alarmed = 0;
    CloseHandle(CreateThread(NULL, 0, TimerProc, (LPVOID) (seconds * 1000), 0, &ThreadId));
}
#endif


void m_sleep(int milliseconds)
{
    Sleep(milliseconds);
}


PUBLIC int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    #if ME_WIN_LIKE
        FILETIME        fileTime;
        Time            now;
        static int      tzOnce;

        if (NULL != tv) {
            /* Convert from 100-nanosec units to microsectonds */
            GetSystemTimeAsFileTime(&fileTime);
            now = ((((Time) fileTime.dwHighDateTime) << BITS(uint)) + ((Time) fileTime.dwLowDateTime));
            now /= 10;
            now -= TIME_GENESIS;
            tv->tv_sec = (long) (now / 1000000);
            tv->tv_usec = (long) (now % 1000000);
        }
        if (NULL != tz) {
            TIME_ZONE_INFORMATION   zone;
            int                     rc, bias;
            rc = GetTimeZoneInformation(&zone);
            bias = (int) zone.Bias;
            if (rc == TIME_ZONE_ID_DAYLIGHT) {
                tz->tz_dsttime = 1;
            } else {
                tz->tz_dsttime = 0;
            }
            tz->tz_minuteswest = bias;
        }
        return 0;

    #elif VXWORKS
        struct tm       tm;
        struct timespec now;
        time_t          t;
        char            *tze, *p;
        int             rc;

        if ((rc = clock_gettime(CLOCK_REALTIME, &now)) == 0) {
            tv->tv_sec  = now.tv_sec;
            tv->tv_usec = (now.tv_nsec + 500) / MS_PER_SEC;
            if ((tze = getenv("TIMEZONE")) != 0) {
                if ((p = strchr(tze, ':')) != 0) {
                    if ((p = strchr(tze, ':')) != 0) {
                        tz->tz_minuteswest = stoi(++p);
                    }
                }
                t = tickGet();
                tz->tz_dsttime = (localtime_r(&t, &tm) == 0) ? tm.tm_isdst : 0;
            }
        }
        return rc;
    #endif
}

#else

ulong get_timer(struct hr_time *val, int reset)
{
    ulong delta;
    struct timeval offset;
    struct _hr_time *t = (struct _hr_time *)val;

    gettimeofday(&offset, NULL);
    delta = (offset.tv_sec - t->start.tv_sec) * 1000 + (offset.tv_usec - t->start.tv_usec) / 1000;
    if (reset) {
        t->start.tv_sec = offset.tv_sec;
        t->start.tv_usec = offset.tv_usec;
    }
    return (delta);
}


static void sighandler(int signum)
{
    alarmed = 1;
    signal(signum, sighandler);
}


#if UNUSED
void set_alarm(int seconds)
{
    alarmed = 0;
    signal(SIGALRM, sighandler);
    alarm(seconds);
}
#endif


void m_sleep(int milliseconds)
{
    struct timeval tv;

    tv.tv_sec = milliseconds / 1000;
    tv.tv_usec = milliseconds * 1000;
    select(0, NULL, NULL, NULL, &tv);
}

#endif

#endif

/*
    @copy   default

    Copyright (c) Embedthis Software. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a 
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
