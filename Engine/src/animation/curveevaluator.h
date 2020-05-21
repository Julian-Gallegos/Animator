/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#ifndef CURVEEVALUATOR_H
#define CURVEEVALUATOR_H

#include <vectors.h>

class CurveEvaluator {
public:
    CurveEvaluator() : wrap_y_(false), extend_x_(false), max_x_(0.f) {}
	CurveEvaluator(float animation_length, bool wrap) : wrap_y_(wrap), extend_x_(true), max_x_(animation_length) {}
    virtual std::vector<glm::vec2> EvaluateCurve(const std::vector<glm::vec2>& ctrl_pts, int density) const = 0;
    virtual ~CurveEvaluator() {}
    void Wrap(bool wrap = true) { wrap_y_ = wrap; }
protected:
    void ExtendX(std::vector<glm::vec2>& out_pts, const std::vector<glm::vec2>& in_pts) const {
        if (wrap_y_) {
            // if wrapping is on, linearly interpolate the y value at xmin and
            // xmax so that the slopes of the lines adjacent to the
            // wraparound are equal.
            float dx1 = in_pts[0].x;
            float dx2 = max_x_ - in_pts.back().x;
            float t = dx2/(dx1 + dx2);
            float y = t*in_pts[0].y + (1-t)*in_pts.back().y;
            out_pts.insert(out_pts.begin(), glm::vec2(0, y));
            out_pts.push_back(glm::vec2(max_x_, y));
        } else {
	        if (in_pts.back().x < max_x_)
	            out_pts.push_back(glm::vec2(max_x_, in_pts.back().y));
	        if (in_pts[0].x > 0)
                out_pts.insert(out_pts.begin(), glm::vec2(0, in_pts[0].y));
        }
    }
    std::vector<glm::vec2> LinearEvaluate(const std::vector<glm::vec2> &ctrl_pts, int density) const {
        std::vector<glm::vec2> evaluated_pts;

        if (density == 0) density = 100;
        for (size_t i = 0; i < ctrl_pts.size()-1; i++) {
            for (int j = 0; j < density; j++) {
                float t = j/(float) density;
                glm::vec2 p = t*ctrl_pts[i+1] + (1-t)*ctrl_pts[i];
                evaluated_pts.push_back(p);
            }
        }
        return evaluated_pts;
    }
    std::vector<glm::vec2> LinearEvaluatePair(int density, glm::vec2 p0, glm::vec2 p1) const {
        std::vector<glm::vec2> evaluated_pts;
        for (int j = 0; j < density; j++) {
            float t = j/(float) density;
            glm::vec2 p = t*p1 + (1-t)*p0;
            evaluated_pts.push_back(p);
        }
        return evaluated_pts;
    }
    void AddBezier(std::vector<glm::vec2> &add_to, int density, glm::vec2 v0,
                   glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) const {
        size_t j;
        double u;
        glm::vec2 next;
        for (j = 0; j < static_cast<size_t>(density); j++) {
            u = static_cast<double>(j)/static_cast<double>(density); // j/density
            next = (static_cast<float>(pow(1.0-u, 3.0)) * v0) +
                   (static_cast<float>(3.0*u*pow(1.0-u, 2.0)) * v1) +
                   (static_cast<float>(3.0*pow(u, 2.0)*(1-u)) * v2) +
                   (static_cast<float>(pow(u, 3.0)) * v3);
            add_to.push_back(next);
        }
    }
    bool wrap_y_;
    bool extend_x_;
    float max_x_;
};

#endif // CURVEEVALUATOR_H
