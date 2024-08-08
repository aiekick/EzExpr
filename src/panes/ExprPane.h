#pragma once

/*
MIT License

Copyright (c) 2024 Aiekick

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

#include <ImGuiPack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class ExprPane : public AbstractPane {
private:

public:
    bool Init() override;
    void Unit() override;
    bool DrawPanes(const uint32_t& vCurrentFrame, bool* vOpened = nullptr, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    
public:  // singleton
    static std::shared_ptr<ExprPane> Instance() {
        static std::shared_ptr<ExprPane> _instance = std::make_shared<ExprPane>();
        return _instance;
    }

public:
    ExprPane();                             // Prevent construction
    ExprPane(const ExprPane&) = delete;  // Prevent construction by copying
    ExprPane& operator=(const ExprPane&) {
        return *this;
    };                      // Prevent assignment
    virtual ~ExprPane();  // Prevent unwanted destruction};
};
