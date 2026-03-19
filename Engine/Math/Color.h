#pragma once

// 색상.
enum class Color : int
{
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5, 
    Yellow = 6,
    Gray = 7,
    DarkGray = 8,
    BrightBlue = 9,
    BrightGreen = 10,
    BrightCyan = 11,
    BrightRed = 12,
    BrightMagenta = 13,
    BrightYellow = 14,
    White = 15,
    Transparent = -1
};
//16진수를 컬러로 매핑
inline Color ConvertHexCharToColor(char c) //함수 중복을 피하기 위해 inline 선언
{
    switch (toupper(c))
    {
    case '0': return Color::Black;
    case '1': return Color::Blue;
    case '2': return Color::Green;
    case '3': return Color::Cyan;
    case '4': return Color::Red;
    case '5': return Color::Magenta;
    case '6': return Color::Yellow;
    case '7': return Color::Gray;
    case '8': return Color::DarkGray;
    case '9': return Color::BrightBlue;
    case 'A': return Color::BrightGreen;
    case 'B': return Color::BrightCyan;
    case 'C': return Color::BrightRed;
    case 'D': return Color::BrightMagenta;
    case 'E': return Color::BrightYellow;
    case 'F': return Color::White;
    default:  return Color::Transparent; // 사용자 정의 또는 무시
    }
}