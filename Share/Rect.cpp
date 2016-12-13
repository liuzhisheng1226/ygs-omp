#include "Rect.h"

Rect::Rect() {}

Rect::Rect(int left_t, int top_t, int right_t, int bottom_t){
    left = left_t;
    top  = top_t;
    right = right_t;
    bottom= bottom_t;
    TopLeft_t.x = left_t;
    TopLeft_t.y = top_t;
    BottomRight_t.x = right_t;
    BottomRight_t.y = bottom_t;
}

void Rect::SetRect(int left_t, int top_t, int right_t, int bottom_t){
    left = left_t;
    top  = top_t;
    right = right_t;
    bottom= bottom_t;
    TopLeft_t.x = left_t;
    TopLeft_t.y = top_t;
    BottomRight_t.x = right_t;
    BottomRight_t.y = bottom_t;
}
Point &Rect::BottomRight(){

    return Rect::BottomRight_t;

}

Point &Rect::TopLeft(){

    return Rect::TopLeft_t;

}

