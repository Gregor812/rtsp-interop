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
            Wrapper.SplitStreamToTheFiles(Log);
            Console.WriteLine("Returned successfully to the managed code");
            Console.WriteLine("Bye!");
        }

        static void Log()
        {
            Console.WriteLine("File writed!");
        }
    }
}
