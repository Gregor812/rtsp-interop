using System;
using VideoNativeHelper;

namespace RtspListener
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Managed code");
            Console.WriteLine("Calling native code...");
            Wrapper.SplitStreamToTheFiles();
            Console.WriteLine("Returned successfully to the managed code");
            Console.WriteLine("Bye!");
        }
    }
}
