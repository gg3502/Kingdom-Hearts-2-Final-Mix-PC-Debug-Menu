#pragma once
#include "YW.hpp"
#include <windows.h>  // For RECT and other Windows APIs

namespace YW {

    enum SLIDER_STATE {
        STATE_NORMAL_0 = 0,
       // STATE_TOUCH = 1,
        //STATE_DRAG = 2,
    };

    class FLOAT_EDITOR : public WIDGET {
    public:
        float Value;
        float MinValue;
        float MaxValue;
        float Step;
        int SlideX;
        SCROLLBAR_STATE State;
        void (*OnValueChanged)(float newValue);
        void (*OnSubmit)(float finalValue);

        RECT WidgetRect;  // RECT for widget position and size

        // Constructor
        FLOAT_EDITOR(float minValue, float maxValue, float step, float initialValue,
            int x, int y, int width, int height);

        // Methods to interact with the value
        void Increment();
        void Decrement();
        void SetValue(float newValue);

        // Methods for handling slider touch/drag actions
        void StartTouch(int initialX);
        void StartDrag(int newX);
        void StopDrag();
        void UpdateValue();

        // Render method
        void Render();
    };

    // Function to handle user input for the widget
    void HandleInput(FLOAT_EDITOR* editor, char input);

} // namespace YW
