using System.Runtime.InteropServices;

namespace VideoNativeHelper
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void Callback();

    public static class Wrapper
    {
        [DllImport("RtspSlicer", EntryPoint = "remux", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SplitStreamToTheFiles(Callback call);
    }
}
