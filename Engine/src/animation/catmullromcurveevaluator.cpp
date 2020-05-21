/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "catmullromcurveevaluator.h"

std::vector<glm::vec2> CatmullRomCurveEvaluator::EvaluateCurve(const std::vector<glm::vec2> &ctrl_pts, int density) const {
    std::vector<glm::vec2> evaluated_pts;

    // REQUIREMENT:
    // Currently this function returns points for a Linear Evaluation.
    // Replace this code with code that returns evaluated points for a Catmull-Rom
    // curve. Be sure to respect the extend_x_ and wrap_ flags; in particular,
    // the wrapped function should be C1 continuous like the rest of the curve.

    // double endpoints to interpolate!

    if (density == 0) density = 100;

    if (ctrl_pts.size() < 3) {
        evaluated_pts = LinearEvaluate(ctrl_pts, density);
    } else {
        size_t i;
        glm::vec2 p0, p1, p2, p3, v0, v1, v2, v3;
        for (i = 0; i < ctrl_pts.size() - 1; i++) {
            if (i == 0) {
                // double first endpoint
                p0 = ctrl_pts[i], p1 = ctrl_pts[i], p2 = ctrl_pts[i+1], p3 = ctrl_pts[i+2];
            } else if (i == ctrl_pts.size() - 2) {
                // double last endpoint
                p0 = ctrl_pts[i-1], p1 = ctrl_pts[i], p2 = ctrl_pts[i+1], p3 = ctrl_pts[i+1];
            } else {
                // general case
                p0 = ctrl_pts[i-1], p1 = ctrl_pts[i], p2 = ctrl_pts[i+1], p3 = ctrl_pts[i+2];
            }
            v0 = p1;
            v1 = p1 + ((1.f/6.f)*(p2 - p0));
            v2 = p2 - ((1.f/6.f)*(p3 - p1));
            v3 = p2;
            AddBezier(evaluated_pts, density, v0, v1, v2, v3);
        }
    }
    evaluated_pts.push_back(ctrl_pts.back());
    if (extend_x_) ExtendX(evaluated_pts, ctrl_pts);
    // if (wrap_y_) -- extra credit
    return evaluated_pts;
}
