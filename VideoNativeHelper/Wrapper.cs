using System.Runtime.InteropServices;

namespace VideoNativeHelper
{
    public static class Wrapper
    {
        [DllImport("RtspSlicer", EntryPoint = "remux", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SplitStreamToTheFiles();
    }
}
