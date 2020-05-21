/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "bsplinecurveevaluator.h"

std::vector<glm::vec2> BSplineCurveEvaluator::EvaluateCurve(const std::vector<glm::vec2> &ctrl_pts, int density) const {
    std::vector<glm::vec2> evaluated_pts;

    // REQUIREMENT:
    // Currently this function returns points for a Linear Evaluation.
    // Replace this code with code that returns evaluated points for a B-Spline
    // curve. Be sure to respect the extend_x_ and wrap_ flags.

    // triple endpoints to interpolate!

    if (density == 0) density = 100;

    if (ctrl_pts.size() < 3) {
        evaluated_pts = LinearEvaluate(ctrl_pts, density);
    } else {
        size_t i;
        glm::vec2 b0, b1, b2, b3, v0, v1, v2, v3;
        for (i = 0; i < ctrl_pts.size(); i++) {
            if (i == 0) {
                // triple first endpoint
                b0 = ctrl_pts[i], b1 = ctrl_pts[i], b2 = ctrl_pts[i], b3 = ctrl_pts[i+1];
            } else if (i == 1) {
                // double first endpoint
                b0 = ctrl_pts[i-1], b1 = ctrl_pts[i-1], b2 = ctrl_pts[i], b3 = ctrl_pts[i+1];
            } else if (i == ctrl_pts.size() - 2) {
                // double last endpoint
                b0 = ctrl_pts[i-1], b1 = ctrl_pts[i], b2 = ctrl_pts[i+1], b3 = ctrl_pts[i+1];
            } else if (i == ctrl_pts.size() - 1) {
                // triple last endpoint
                b0 = ctrl_pts[i-1], b1 = ctrl_pts[i], b2 = ctrl_pts[i], b3 = ctrl_pts[i];
            } else {
                // general case
                b0 = ctrl_pts[i-1], b1 = ctrl_pts[i], b2 = ctrl_pts[i+1], b3 = ctrl_pts[i+2];
            }
            v0 = ((1.f/6.f)*b0) + ((2.f/3.f)*b1) + ((1.f/6.f)*b2);
            v1 = ((2.f/3.f)*b1) + ((1.f/3.f)*b2);
            v2 = ((3.f/3.f)*b1) + ((2.f/3.f)*b2);
            v3 = ((1.f/6.f)*b1) + ((2.f/3.f)*b2) + ((1.f/6.f)*b3);
            AddBezier(evaluated_pts, density, v0, v1, v2, v3);
        }
    }
    evaluated_pts.push_back(ctrl_pts.back());
    if (extend_x_) ExtendX(evaluated_pts, ctrl_pts);
    // if (wrap_y_) -- extra credit
    return evaluated_pts;
}
