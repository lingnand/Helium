/*
 * Progress.h
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#ifndef PROGRESS_H_
#define PROGRESS_H_

struct Progress {
    float current;
    float cap;
    Progress(float start=0, float end=1): current(start), cap(end) {}
};

#endif /* PROGRESS_H_ */
