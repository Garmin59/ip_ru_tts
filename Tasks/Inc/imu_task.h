/**************************************************************
 * @file imu_task.h
 * @author Ilya Petrukhin ()
 * @brief 
 * @version 0.1
 * @date 30-04-2025
 * 
 * @copyright Copyright (c) 2025
 * 
**************************************************************/

#pragma once

void imu_task (void *param);
void reset_course (void);
void reset_course_dev (uint8_t dev);
