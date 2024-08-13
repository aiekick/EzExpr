#include "EzExprManager.h"

/*
MIT License

Copyright (c) 2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define DEFAULT_EXPR "x"

std::shared_ptr<OneExpr> OneExpr::create(const std::string& vLabel, const int32_t& vIdx) {
    auto ret    = std::make_shared<OneExpr>();
    ret->m_This = ret;
    if (!ret->init(vLabel, vIdx)) {
        ret.reset();
    }
    return ret;
}

bool OneExpr::init(const std::string& vLabel, const int32_t& vIdx) {
    m_Label = vLabel;
    m_Idx   = vIdx;
    m_ExprInput.SetText(DEFAULT_EXPR);
    return true;
}

void OneExpr::drawCurve() {
    ImPlot::PlotLine(m_Label.c_str(), m_axisX.data(), m_axisY.data(), OneExpr::s_COUNT_POINTS);
}

void OneExpr::drawWidgets() {
    const auto avail_width = ImGui::GetContentRegionAvail().x;
    if (m_ErrorCode != ez::ErrorCode::NONE) {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "%s", m_ErrorMsg.c_str());
    }
    if (m_ExprInput.DisplayInputText(avail_width, m_Label, DEFAULT_EXPR, false, 70.0f)) {
        m_computeExpr();
    }
    if (!m_Expr.getParsedVars().empty() && m_Expr.isParsedVariableExist("x") && m_Expr.getParsedVars().size() > 1) {
        if (m_WidgetTypes.size() != (m_Expr.getParsedVars().size() - 1U)) {
            ImGui::Text("%s", "Add a Input ?");
            for (const auto& var : m_Expr.getParsedVars()) {
                if (var.first == "x") {
                    continue;
                }
                ImGui::SameLine();
                if (m_WidgetTypes.find(var.first) == m_WidgetTypes.end()) {  // not found
                    if (ImGui::ContrastedButton(var.first.c_str())) {
                        m_Expr.set(var.first, 0.0, true);
                        m_WidgetTypes[var.first] = WidgetType::WIDGET_INPUT;
                    }
                }
            }
            ImGui::Text("%s", "Add a Slider ?");
            for (const auto& var : m_Expr.getParsedVars()) {
                if (var.first == "x") {
                    continue;
                }
                ImGui::SameLine();
                if (m_WidgetTypes.find(var.first) == m_WidgetTypes.end()) {  // not found
                    if (ImGui::ContrastedButton(var.first.c_str())) {
                        m_Expr.set(var.first, 0.0, true);
                        m_WidgetTypes[var.first] = WidgetType::WIDGET_SLIDER;
                    }
                }
            }
        }
    }
    for (auto& var : m_Expr.getDefinedVarsRef()) {
        if (var.first == "x") {
            continue;
        }
        assert(m_WidgetTypes.find(var.first) != m_WidgetTypes.end());
        switch (m_WidgetTypes.at(var.first)) {
            case WidgetType::WIDGET_INPUT: {
                if (ImGui::InputDoubleDefaultStepper(  //
                        avail_width, 70.0f, var.first.c_str(), &var.second, 0.0, 0.1, 0.5, "%.6f", "%.6f")) {
                    m_Expr.set(var.first, var.second);
                    m_computeExpr();
                }
            } break;
            case WidgetType::WIDGET_SLIDER: {
                ImGui::Text("%s", var.first.c_str());
                ImGui::SameLine(70.0f);
                ImGui::PushID(var.first.c_str());
                if (ImGui::SliderDoubleDefault(avail_width - 70.0f - ImGui::GetStyle().ItemSpacing.x, "##WIDGET_SLIDER",
                                               &var.second, -10.0, 10.0, 0.0)) {
                    m_Expr.set(var.first, var.second);
                    m_computeExpr();
                }
                ImGui::PopID();
            } break;
            case WidgetType::Count:
            default: break;
        }
    }
    ImGui::Separator();
}

void OneExpr::reComputeIfNeeded(const ImPlotRect& vRect) {
    if (vRect.Min().x != m_ChartLimits.Min().x ||  //
        vRect.Min().y != m_ChartLimits.Min().y ||  //
        vRect.Max().x != m_ChartLimits.Max().x ||  //
        vRect.Max().y != m_ChartLimits.Max().y) {
        m_ChartLimits = vRect;
        m_computeExpr();
    }
}

bool OneExpr::m_computeExpr() {
    bool ret    = false;
    m_ErrorCode = ez::ErrorCode::NONE;
    m_ErrorMsg.clear();
    try {
        m_Expr.parse(m_ExprInput.GetText());
    } catch (const ez::ExprException& ex) {
        m_ErrorCode = ex.getCode();
        m_ErrorMsg  = ex.what();
        return false;
    }
    double rangeX         = m_ChartLimits.Size().x;
    double stepX          = rangeX / s_COUNT_POINTS;
    double px             = m_ChartLimits.Min().x;
    bool have_div_by_zero = false;
    bool have_inf         = false;
    bool have_nan         = false;
    for (size_t idx = 0U; idx < s_COUNT_POINTS; ++idx) {
        m_axisX[idx] = px;
        try {
            m_axisY[idx] = m_Expr.set("x", px).eval().getResult();
        } catch (const ez::ExprException& ex) {
            m_ErrorCode = ex.getCode();
            if (m_ErrorCode == ez::ErrorCode::DIVISION_BY_ZERO) {
                have_div_by_zero = true;
            } else if (m_ErrorCode == ez::ErrorCode::EVALUATION_INF) {
                have_inf = true;
            } else if (m_ErrorCode == ez::ErrorCode::EVALUATION_NAN) {
                have_nan = true;
            }
            m_axisY[idx] = 0.0;
        }
        px += stepX;
    }
    if (have_div_by_zero) {
        m_ErrorMsg += "\nSome portions have div by zero";
    } else if (have_inf) {
        m_ErrorMsg += "\nSome portions have inf value";
    } else if (have_nan) {
        m_ErrorMsg += "\nSome portions have nan value";
    }
    return ret;
}

OneExpr::Points& OneExpr::getAxisXRef() {
    return m_axisX;
}

OneExpr::Points& OneExpr::getAxisYRef() {
    return m_axisY;
}

bool EzExprManager::init() {
    m_ChartLimits = ImPlotRect(-2, 2, -2, 2);
    m_Exprs.push_back(OneExpr::create("f0(y)", 0));
    return true;
}

void EzExprManager::unit() {
    m_Exprs.clear();
}

void EzExprManager::drawChart() {
    static ImPlotAxisFlags flags;
    if (ImPlot::BeginPlot("##MathExpr", ImVec2(-1, -1))) {
        ImPlot::SetupAxes("X", "Y", flags, flags);
        ImPlot::SetupAxisLinks(ImAxis_X1, &m_ChartLimits.X.Min, &m_ChartLimits.X.Max);
        ImPlot::SetupAxisLinks(ImAxis_Y1, &m_ChartLimits.Y.Min, &m_ChartLimits.Y.Max);
        for (auto& exprPtr : m_Exprs) {
            if (exprPtr != nullptr) {
                exprPtr->reComputeIfNeeded(m_ChartLimits);
                exprPtr->drawCurve();
            }
        }
        ImPlot::EndPlot();
    }
}

void EzExprManager::drawExpr() {
    for (auto& exprPtr : m_Exprs) {
        if (exprPtr != nullptr) {
            exprPtr->drawWidgets();
        }
    }
}
