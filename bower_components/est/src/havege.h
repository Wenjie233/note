/*
    havege.h -- Havege Random Support

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
#ifndef EST_HAVEGE_H
#define EST_HAVEGE_H

#define COLLECT_SIZE 1024

/**
   @brief          HAVEGE state structure
 */
typedef struct {
    int PT1, PT2, offset[2];
    int pool[COLLECT_SIZE];
    int WALK[8192];
} havege_state;

#ifdef __cplusplus
extern "C" {
#endif

    /**
       @brief          HAVEGE initialization
       @param hs       HAVEGE state to be initialized
     */
    PUBLIC void havege_init(havege_state *hs);

    /**
       @brief          HAVEGE rand function
       @param rng_st   points to an HAVEGE state
       @return         A random int
     */
    PUBLIC int havege_rand(void *p_rng);

#ifdef __cplusplus
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
