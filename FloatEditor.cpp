#include "FloatEditor.hpp"
#include <cstdio> // For printf
#include <string> // For std::to_string if needed
#include "patterns.hpp"
#include "function_typedefs.hpp"
#include <stdexcept>
#include <algorithm>
#include "helper.hpp"

namespace YW {


    uintptr_t CreateWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_Widget, L"Create YW Widget", 0, 1, 5);
    FunctionCalls::tWidget CreateWidgetFunction = reinterpret_cast<FunctionCalls::tWidget>(CreateWidgetFunctionAddress);

    uintptr_t DrawRectRiseWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_DrawRECTRiseWindow, L"Draw RECT Rise Window", 0, 1, 5);
    FunctionCalls::tDrawRiseWindow DrawRiseWindowFunction = reinterpret_cast<FunctionCalls::tDrawRiseWindow>(DrawRectRiseWindowFunctionAddress);
    
    uintptr_t DrawRectSinkWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_DrawRECTSinkWindow, L"Draw RECT Sink Window", 0, 1, 5);
    FunctionCalls::tDrawSinkWindow DrawSinkWindowFunction = reinterpret_cast<FunctionCalls::tDrawSinkWindow>(DrawRectSinkWindowFunctionAddress);

    FLOAT_EDITOR::FLOAT_EDITOR(float minValue, float maxValue, float step, float initialValue,
        int x, int y, int width, int height)
        : Value(initialValue), MinValue(minValue), MaxValue(maxValue), Step(step),
        OnValueChanged(nullptr), OnSubmit(nullptr) {
        // Ensure the parent (YW::WIDGET) is initialized 
        WidgetRect = { x, y, width, height };  // x, y, width, height
        // Use the CreateWidgetFunction to create the widget in the game engine


        if (CreateWidgetFunction) {
            CreateWidgetFunction(this, x, y, width, height);
        }
        else {
            throw std::runtime_error("CreateWidgetFunction is nullptr. Failed to create widget.");
        }
    }

    void FLOAT_EDITOR::Increment() {
        if (Value + Step <= MaxValue) {
            Value += Step;
            if (OnValueChanged) {
                OnValueChanged(Value);
            }
        }
    }

    void FLOAT_EDITOR::Decrement() {
        if (Value - Step >= MinValue) {
            Value -= Step;
            if (OnValueChanged) {
                OnValueChanged(Value);
            }
        }
    }

    void FLOAT_EDITOR::SetValue(float newValue) {
        if (newValue >= MinValue && newValue <= MaxValue) {
            Value = newValue;
            if (OnValueChanged) {
                OnValueChanged(Value);
            }
        }
    }

    // Handle the touch state (start of interaction)
    void FLOAT_EDITOR::StartTouch(int initialX) {
        State = STATE_TOUCH;
        SlideX = initialX;
    }

    // Handle dragging (move slider to new position)
    void FLOAT_EDITOR::StartDrag(int newX) {
        if (State == STATE_TOUCH || State == STATE_DRAG) {
            State = STATE_DRAG;
            SlideX = std::clamp(newX, static_cast<int>(MinValue), static_cast<int>(MaxValue));
            UpdateValue();
        }
    }

    // Stop the drag
    void FLOAT_EDITOR::StopDrag() {
        State = STATE_NORMAL;
    }

    // Update the value based on the slider's position
    void FLOAT_EDITOR::UpdateValue() {
        float normalized = static_cast<float>(SlideX) / (MaxValue - MinValue);
        Value = MinValue + normalized * (MaxValue - MinValue);
        if (OnValueChanged) {
            OnValueChanged(Value);
        }
    }

    YW::COLOR_0 BackgroundColor = { 255, 255, 255, 255 };  // White
    YW::COLOR_0 HandleColorNormal = { 0, 0, 0, 255 };  // Black
    YW::COLOR_0 HandleColorTouched = { 255, 0, 0, 255 };  // Red

    YW::COLOR_0 Color[3] = {
    {255, 255, 255, 255},  // White
    {0, 0, 0, 255},        // Black
    {255, 0, 0, 255}       // Red
    };

    // Render the FLOAT_EDITOR
    void FLOAT_EDITOR::Render() {
        // Calculate normalized position for the slider handle (similar to the provided code)
        float valueRange = MaxValue - MinValue;
        SlideX = static_cast<int>((Value - MinValue) / valueRange * (WidgetRect.W - 6) + 0.5f);

        // Prepare a RECT for drawing the slider background
        YW::RECT backgroundRect;
        backgroundRect.X = WidgetRect.X + 3;  // Padding from the left
        backgroundRect.Y = WidgetRect.Y + (WidgetRect.H - 4) / 2;  // Vertically center
        backgroundRect.W = WidgetRect.W - 6;  // Subtract padding from the sides
        backgroundRect.H = 4;  // Height of the slider's background

        // Draw the background (function equivalent to draw_sink_window)
        DrawSinkWindowFunction(&backgroundRect, &BackgroundColor);

        // Prepare a RECT for drawing the slider handle
        YW::RECT handleRect;
        handleRect.X = WidgetRect.X + SlideX;  // Position the handle based on SlideX
        handleRect.Y = WidgetRect.Y;
        handleRect.W = 6;  // Width of the handle
        handleRect.H = WidgetRect.H;  // Full height of the slider

        // Choose the color based on the slider state
        //const YW::COLOR_0* handleColor = HandleColorTouched;  // No error, Color decays to &Color[0]

        // Draw the slider handle (function equivalent to draw_rise_window)
        DrawRiseWindowFunction(&handleRect, &HandleColorTouched);
    }

    // Handle input events
    /*void HandleInput(FLOAT_EDITOR* editor, char input) {
        if (input == '+') {
            editor->Increment();
        }
        else if (input == '-') {
            editor->Decrement();
        }
        else if (isdigit(input) || input == '.') {
            float newValue = atof(&input);  // Convert single-character input to a float
            editor->SetValue(newValue);
        }
        else if (input == '\n') {
            if (editor->OnSubmit) {
                editor->OnSubmit(editor->Value);
            }
        }
    }*/

    // Example function to simulate dragging input and update the FLOAT_EDITOR
    /*void HandleSliderInput(FLOAT_EDITOR* editor, char input) {
        static int dragStartX = 0;

        if (input == 't') {  // Simulating touch start (example)
            editor->StartTouch(dragStartX);
        }
        else if (input == 'd') {  // Simulating drag (move by 10 units)
            int newX = dragStartX + 10;
            editor->StartDrag(newX);
        }
        else if (input == 's') {  // Stop drag
            editor->StopDrag();
        }
    }*/


} // namespace YW
