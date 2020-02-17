#include <napi.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <windows.h>
#include <winuser.h>

using namespace Napi;
using namespace std;

map<string, int> keysDef = {
    {"0", 0x30},
    {"1", 0x31},
    {"2", 0x32},
    {"3", 0x33},
    {"4", 0x34},
    {"5", 0x35},
    {"6", 0x36},
    {"7", 0x37},
    {"8", 0x38},
    {"9", 0x39},
    {"a", 0x41},
    {"b", 0x42},
    {"c", 0x43},
    {"d", 0x44},
    {"e", 0x45},
    {"f", 0x46},
    {"g", 0x47},
    {"h", 0x48},
    {"i", 0x49},
    {"j", 0x4A},
    {"k", 0x4B},
    {"l", 0x4C},
    {"m", 0x4D},
    {"n", 0x4E},
    {"o", 0x4F},
    {"p", 0x50},
    {"q", 0x51},
    {"r", 0x52},
    {"s", 0x53},
    {"t", 0x54},
    {"u", 0x55},
    {"v", 0x56},
    {"w", 0x57},
    {"x", 0x58},
    {"y", 0x59},
    {"z", 0x5A},
    {"+", VK_OEM_PLUS},
    {"-", VK_OEM_MINUS},
    {".", VK_OEM_PERIOD},
    {",", VK_OEM_COMMA},
    {"?", VK_OEM_2},
    {"~", VK_OEM_3},
    {"[", VK_OEM_4},
    {"]", VK_OEM_6},
    {"|", VK_OEM_5},
    {"'", VK_OEM_7},
    {"backspace", VK_BACK},
    {"delete", VK_DELETE},
    {"enter", VK_RETURN},
    {"tab", VK_TAB},
    {"escape", VK_ESCAPE},
    {"up", VK_UP},
    {"down", VK_DOWN},
    {"right", VK_RIGHT},
    {"left", VK_LEFT},
    {"home", VK_HOME},
    {"end", VK_END},
    {"pageup", VK_PRIOR},
    {"pagedown", VK_NEXT},
    {"f1", VK_F1},
    {"f2", VK_F2},
    {"f3", VK_F3},
    {"f4", VK_F4},
    {"f5", VK_F5},
    {"f6", VK_F6},
    {"f7", VK_F7},
    {"f8", VK_F8},
    {"f9", VK_F9},
    {"f10", VK_F10},
    {"f11", VK_F11},
    {"f12", VK_F12},
    {"f13", VK_F13},
    {"f14", VK_F14},
    {"f15", VK_F15},
    {"f16", VK_F16},
    {"f17", VK_F17},
    {"f18", VK_F18},
    {"f19", VK_F19},
    {"f20", VK_F20},
    {"f21", VK_F21},
    {"f22", VK_F22},
    {"f23", VK_F23},
    {"f24", VK_F24},
    {"capslock", VK_CAPITAL},
    {"alt", VK_MENU},
    {"control", VK_CONTROL},
    {"shift", VK_SHIFT},
    {"space", VK_SPACE},
    {"printscreen", VK_SNAPSHOT},
    {"insert", VK_INSERT},
    {"num_lock", VK_NUMLOCK},
    {"num_0", VK_NUMPAD0},
    {"num_0", VK_NUMPAD0},
    {"num_1", VK_NUMPAD1},
    {"num_2", VK_NUMPAD2},
    {"num_3", VK_NUMPAD3},
    {"num_4", VK_NUMPAD4},
    {"num_5", VK_NUMPAD5},
    {"num_6", VK_NUMPAD6},
    {"num_7", VK_NUMPAD7},
    {"num_8", VK_NUMPAD8},
    {"num_9", VK_NUMPAD9},
    {"num_+", VK_ADD},
    {"num_-", VK_SUBTRACT},
    {"num_*", VK_MULTIPLY},
    {"num_/", VK_DIVIDE},
    {"num_.", VK_DECIMAL}};

static Value sendKey(const CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1)
    {
        Error::New(info.Env(), "Expected exactly one argument")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    if (!info[0].IsArray())
    {
        Error::New(info.Env(), "Expected an Array")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    bool isKeyExtend = false;
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wVk = 0;
    ip.ki.dwExtraInfo = 0;
    Array keys = info[0].As<Array>();
    for (size_t i = 0; i < keys.Length(); i++)
    {
        Value vKey = keys[i];
        if (vKey.IsString())
        {
            string key = vKey.As<String>();
            switch (keysDef[key])
            {
            case VK_RCONTROL:
            case VK_SNAPSHOT:
            case VK_RMENU:
            case VK_PAUSE:
            case VK_HOME:
            case VK_UP:
            case VK_PRIOR:
            case VK_LEFT:
            case VK_RIGHT:
            case VK_END:
            case VK_DOWN:
            case VK_NEXT:
            case VK_INSERT:
            case VK_DELETE:
            case VK_LWIN:
            case VK_RWIN:
            case VK_APPS:
            case VK_VOLUME_MUTE:
            case VK_VOLUME_DOWN:
            case VK_VOLUME_UP:
            case VK_MEDIA_NEXT_TRACK:
            case VK_MEDIA_PREV_TRACK:
            case VK_MEDIA_STOP:
            case VK_MEDIA_PLAY_PAUSE:
            case VK_BROWSER_BACK:
            case VK_BROWSER_FORWARD:
            case VK_BROWSER_REFRESH:
            case VK_BROWSER_STOP:
            case VK_BROWSER_SEARCH:
            case VK_BROWSER_FAVORITES:
            case VK_BROWSER_HOME:
            case VK_LAUNCH_MAIL:
            {
                isKeyExtend = true;
                break;
            }
            }
            ip.ki.dwFlags = isKeyExtend ? KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY : KEYEVENTF_SCANCODE;
            ip.ki.wScan = MapVirtualKeyA(keysDef[key], MAPVK_VK_TO_VSC);
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(30);
            ip.ki.dwFlags = isKeyExtend ? KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY : KEYEVENTF_KEYUP;
            SendInput(1, &ip, sizeof(INPUT));
            Sleep(30);
            isKeyExtend = false;
        }
    }
    Boolean isRunning = Boolean::New(env, false);
    return isRunning;
}

static Object Init(Env env, Object exports)
{
    exports["sendKey"] = Function::New(env, sendKey);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)