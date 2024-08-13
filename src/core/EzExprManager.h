#pragma once

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

#include <ImGuiPack/ImGuiPack.h>
#include <EzExpr/EzExpr.hpp>
#include <unordered_map>

class OneExpr;
typedef std::shared_ptr<OneExpr> OneExprPtr;
typedef std::weak_ptr<OneExpr> OneExprWeak;

enum class WidgetType {
    WIDGET_INPUT=0,
    WIDGET_SLIDER,
    Count
};

class EzExprManager;
class OneExpr {
public:
    static constexpr int32_t s_COUNT_POINTS = 1000U;
    typedef std::array<double, s_COUNT_POINTS> Points;
    static OneExprPtr create(const std::string& vLabel, const int32_t& vIdx);

private:
    OneExprWeak m_This;
    Points m_axisX{};
    Points m_axisY{};
    ez::Expr m_Expr;
    ez::ErrorCode m_ErrorCode = ez::ErrorCode::NONE;
    std::string m_ErrorMsg;
    std::string m_Label;
    int32_t m_Idx;
    ImWidgets::InputText m_ExprInput;
    std::unordered_map<ez::String, WidgetType> m_WidgetTypes;
    ImPlotRect m_ChartLimits;

public:
    bool init(const std::string& vLabel, const int32_t& vIdx);
    void drawCurve();
    void drawWidgets();
    void reComputeIfNeeded(const ImPlotRect& vRect);
    Points& getAxisXRef();
    Points& getAxisYRef();

private:
    bool m_computeExpr();
};

class EzExprManager {
public:
    typedef ImPlotRect CharLimits;

private:
    std::vector<OneExprPtr> m_Exprs;
    ImPlotRect m_ChartLimits;

public:
    bool init();
    void unit();
    void drawChart();
    void drawExpr();

public:
    static EzExprManager* instance() {
        static EzExprManager _instance;
        return &_instance;
    }
};
