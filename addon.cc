// addon.cc
#include <node.h>
#include <windows.h>
#include <winuser.h>

namespace demo
{

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

// This is the implementation of the "add" method
// Input arguments are passed using the
// const FunctionCallbackInfo<Value>& args struct
void Add(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wVk = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    ip.ki.wScan = MapVirtualKeyA(0x0D, MAPVK_VK_TO_VSC);
    SendInput(1, &ip, sizeof(INPUT));
    Sleep(50);                        // Sleep 50 milliseconds before key up
    ip.ki.dwFlags = KEYEVENTF_KEYUP;  // set the flag so the key goes up so it doesn't repeat keys
    SendInput(1, &ip, sizeof(INPUT)); // Resend the input
}

void Init(Local<Object> exports)
{
    NODE_SET_METHOD(exports, "add", Add);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

} // namespace demo