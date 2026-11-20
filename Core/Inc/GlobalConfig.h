/**
 ********************************************************************************
 * @file    GlobalConfig.h
 * @author  Author
 * @date    Mon Oct 03 2022
 * @brief
 ********************************************************************************
 */

#ifndef __GLOBALCONFIG_H__
#define __GLOBALCONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/
/** Project config */
#define PROJECT_NAME "IP_IMU_MODULE"

/** Software version
 * aka Semantic versioning:
 * https://semver.org/spec/v2.0.0.html
 * MAJOR.MINOR.PATCH
 */
#define SOFTWARE_VERSION_MAJOR 1
#define SOFTWARE_VERSION_MINOR 0
#define SOFTWARE_VERSION_PATCH 0
#define SOFTWARE_TAG "aplha" // develop / release / alpha / beta /

#ifdef SOFTWARE_VERSION_MAJOR_GIT
#undef SOFTWARE_VERSION_MAJOR
#define SOFTWARE_VERSION_MAJOR SOFTWARE_VERSION_MAJOR_GIT
#endif
#ifdef SOFTWARE_VERSION_MINOR_GIT
#undef SOFTWARE_VERSION_MINOR
#define SOFTWARE_VERSION_MINOR SOFTWARE_VERSION_MINOR_GIT
#endif
#ifdef SOFTWARE_VERSION_PATCH_GIT
#undef SOFTWARE_VERSION_PATCH
#define SOFTWARE_VERSION_PATCH SOFTWARE_VERSION_PATCH_GIT
#endif
#ifdef SOFTWARE_TAG_GIT
#undef SOFTWARE_TAG
#define SOFTWARE_TAG SOFTWARE_TAG_GIT
#endif

#define SOFTWARE_COMMIT 0X78ecda6
#ifdef SOFTWARE_COMMIT_GIT
#undef SOFTWARE_COMMIT
#define SOFTWARE_COMMIT SOFTWARE_COMMIT_GIT
#endif

/* ──────────────────────────────────────────────────────────────────────────────── */
#define COMPILE_HOUR   (((__TIME__[0]-'0')*10) + (__TIME__[1]-'0'))
#define COMPILE_MINUTE (((__TIME__[3]-'0')*10) + (__TIME__[4]-'0'))
#define COMPILE_SECOND (((__TIME__[6]-'0')*10) + (__TIME__[7]-'0'))

#define COMPILE_YEAR  ((((__DATE__[7]-'0')*10+(__DATE__[8]-'0'))*10+(__DATE__[9]-'0'))*10+(__DATE__[10]-'0'))

#define COMPILE_MONTH   ((__DATE__[2] == 'n' ? (__DATE__ [1] == 'a'? 0 : 5) \
                        : __DATE__[2] == 'b' ? 1 \
                        : __DATE__[2] == 'r' ? (__DATE__ [0] == 'M'? 2 : 3) \
                        : __DATE__[2] == 'y' ? 4 \
                        : __DATE__[2] == 'l' ? 6 \
                        : __DATE__[2] == 'g' ? 7 \
                        : __DATE__[2] == 'p' ? 8 \
                        : __DATE__[2] == 't' ? 9 \
                        : __DATE__[2] == 'v' ? 10 : 11)+1)

#define COMPILE_DAY  ((__DATE__[4]==' ' ? 0 : __DATE__[4]-'0')*10+(__DATE__[5]-'0'))
// ─────────────────────────────────────────────────────────────────────────────
/** Debug config */

#ifndef SWO_DEBUG
	#define RETARGET_UART_DEBUG
#endif
// ─────────────────────────────────────────────────────────────────────────────
/** Sensor & algo config */


// #define __VFP_FP__ // In command line

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/


#ifdef __cplusplus
}
#endif

#endif
