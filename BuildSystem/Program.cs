using System;
using System.Runtime.InteropServices;

namespace BuildSystem
{
    class Program
    {
        [DllImport("Core.dll", CharSet = CharSet.Unicode)]
        public static extern void CloseMessageChannel(string channelName);

        [DllImport("Core.dll", CharSet = CharSet.Unicode)]
        public static extern bool ReadMessage(string channelName, out IntPtr outMessage, int timeout = -1);

        [DllImport("Core.dll", CharSet=CharSet.Unicode)]
        public static extern bool WriteMessage(string channelName, string message, int timeout = -1);

        [DllImport("Core.dll", CharSet = CharSet.Unicode)]
        public static extern void FreeMessage(IntPtr messagePtr);

        static void Main(string[] args)
        {
            CloseMessageChannel("TestChannel");
            bool timeout = WriteMessage("TestChannel", "hello world!");
            if (timeout)
            {
                Console.WriteLine("WriteMessage timed out!");
            }

            timeout = ReadMessage("TestChannel", out IntPtr messagePtr);
            if (timeout == false && messagePtr != IntPtr.Zero)
            {
                string? result = Marshal.PtrToStringUni(messagePtr);
                if (messagePtr != IntPtr.Zero)
                {
                    FreeMessage(messagePtr);
                }
            }
        }
    }
}
