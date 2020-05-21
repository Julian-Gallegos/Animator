/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "beziercurveevaluator.h"

std::vector<glm::vec2> BezierCurveEvaluator::EvaluateCurve(const std::vector<glm::vec2> &ctrl_pts, int density) const {
    std::vector<glm::vec2> evaluated_pts;

    // REQUIREMENT:
    // Currently this function returns points for a Linear Evaluation.
    // Replace this code with code that returns evaluated points for a Bezier
    // Spline curve. Be sure to respect the extend_x_ and wrap_ flags in a
    // a reasonable way.

    // use max_x_ with density??

    if (density == 0) density = 100;

    if (ctrl_pts.size() < 4) {
        evaluated_pts = LinearEvaluate(ctrl_pts, density);
    } else {
        size_t i, j;
        glm::vec2 v0, v1, v2, v3;
        for (i = 0; i < ctrl_pts.size() - 1; i+= 3) {
            if (i + 3 >= ctrl_pts.size()) {
                std::vector<glm::vec2> last_few_ctrl, last_few_eval;
                for (j = i; j < ctrl_pts.size(); j++) {
                    last_few_ctrl.push_back(ctrl_pts[j]);
                }
                last_few_eval = LinearEvaluate(last_few_ctrl, density);
                for (auto e : last_few_eval)
                    evaluated_pts.push_back(e);
            } else {
                v0 = ctrl_pts[i], v1 = ctrl_pts[i+1], v2 = ctrl_pts[i+2], v3 = ctrl_pts[i+3];
                AddBezier(evaluated_pts, density, v0, v1, v2, v3);
            }
        }
    }
    evaluated_pts.push_back(ctrl_pts.back());
    if (extend_x_)
        ExtendX(evaluated_pts, ctrl_pts);
    // if (wrap_y_) -- extra credit
    return evaluated_pts;
}
