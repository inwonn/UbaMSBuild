using System;
using System.Runtime.InteropServices;

namespace BuildSystem
{
    class Program
    {
        [DllImport("Core.dll", CharSet = CharSet.Unicode)]
        public static extern void CreateMessageChannel();

        [DllImport("Core.dll", CharSet = CharSet.Unicode)]
        public static extern void ReleaseMessageChannel();

        [DllImport("Core.dll", CharSet = CharSet.Unicode)]
        public static extern int ReadMessage(out IntPtr outMessage, int timeout = -1);

        [DllImport("Core.dll", CharSet=CharSet.Unicode)]
        public static extern int WriteMessage(string message, int timeout = -1);

        [DllImport("Core.dll", CharSet = CharSet.Unicode)]
        public static extern void FreeReadMessage(IntPtr messagePtr);

        static void Main(string[] args)
        {
            ReleaseMessageChannel();
            CreateMessageChannel();
            int error = WriteMessage("hello world!");
            if (error == 1)
            {
                Console.WriteLine("WriteMessage timed out!");
            }

            error = ReadMessage(out IntPtr messagePtr);
            if (error == 0 && messagePtr != IntPtr.Zero)
            {
                string? result = Marshal.PtrToStringUni(messagePtr);
                if (messagePtr != IntPtr.Zero)
                {
                    FreeReadMessage(messagePtr);
                }
            }

            ReleaseMessageChannel();
        }
    }
}
