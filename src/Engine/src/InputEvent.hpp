#pragma once
//
//
namespace odin
{
enum class Scancode
{
    unknown = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */

    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,

    one = 30,
    two = 31,
    three = 32,
    four = 33,
    five = 34,
    six = 35,
    seven = 36,
    eight = 37,
    nine = 38,
    zero = 39,

    return_ = 40,    // return is a reserved keyword
    escape = 41,
    backspace = 42,
    tab = 43,
    space = 44,

    minus = 45,
    equals = 46,
    leftBracket = 47,
    rightBracket = 48,
    backslash = 49,

    semicolon = 51,
    apostrophe = 52,
    grave = 53,
    comma = 54,
    period = 55,
    slash = 56,

    capslock = 57,

    f1 = 58,
    f2 = 59,
    f3 = 60,
    f4 = 61,
    f5 = 62,
    f6 = 63,
    f7 = 64,
    f8 = 65,
    f9 = 66,
    f10 = 67,
    f11 = 68,
    f12 = 69,

    printScreen = 70,
    scrollLock = 71,
    pause = 72,
    insert = 73,    //*< insert on PC, help on some Mac keyboards (but does send code 73, not 117)
    home = 74,
    pageUp = 75,
    delete_ = 76,    // delete is a reserved keyword
    end = 77,
    pageDown = 78,
    right = 79,
    left = 80,
    down = 81,
    up = 82,

    numLockClear = 83, /**< num lock on PC, clear on Mac keyboards */

    keypadDivide = 84,
    keypadMultiply = 85,
    keypadMinus = 86,
    keypadPlus = 87,
    keypadEnter = 88,
    keypadOne = 89,
    keypadTwo = 90,
    keypadThree = 91,
    keypadFour = 92,
    keypadFive = 93,
    keypadSix = 94,
    keypadSeven = 95,
    keypadEight = 96,
    keypadNine = 97,
    keypadZero = 98,
    keypadPeriod = 99,

    leftCtrl = 224,
    leftShift = 225,
    leftAlt = 226, /**< alt, option */
    leftGui = 227, /**< windows, command (apple), meta */
    rightCtrl = 228,
    rightShift = 229,
    rightAlt = 230, /**< alt gr, option */
    rightGui = 231, /**< windows, command (apple), meta */
};
enum class KeyModifier
{
    none = 0x0,
    leftShift = 0x1,
    rightShift = 0x2,
    leftCtrl = 0x40,
    rightCtrl = 0x80,
    leftAlt = 0x01'00,
    rightAlt = 0x02'00,
    leftGui = 0x04'00,
    rightGui = 0x08'00,
    altGr = 0x40'00,
    ctrl = 0x40 | 0x80,
    shift = 0x1 | 0x2,
    alt = 0x01'00 | 0x02'00,
    gui = 0x04'00 | 0x08'00
};
enum class MouseButton
{
    left = 1,
    middle = 2,
    right = 3,
    extra1 = 4,
    extra2 = 5
};
struct KeyboardEvent
{
    Scancode code;
    KeyModifier modifier;
    bool pressed;
    bool repeated;
};
struct MouseClickEvent
{
    MouseButton button;
    float x;
    float y;
    bool pressed;
    std::uint8_t numClicks;
};
struct MouseMotionEvent
{
    float x;
    float y;
    float deltaX;
    float deltaY;
    bool leftPressed;
    bool rightPressed;
    bool middlePressed;
    bool extra1Pressed;
    bool extra2Pressed;
};
struct QuitEvent
{
    bool quit;
};
}    // namespace odin
